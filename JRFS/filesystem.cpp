#include "filesystem.hpp"
#include "details/inode.hpp"
#include "util/utility.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace jrfs {

void filesystem::load_image()
{
    std::fstream is(mount_point, std::ios::out | std::ios::in);
    if (!is.is_open())
        throw std::logic_error("Cannot Open Image File: " + std::string(mount_point));

    meta_data.read(is);

    // TODO: CHECK FILE SIZE.

    inode_list.reserve(meta_data.inode_total);
    for (int i = 0; i < meta_data.inode_total; ++i) {
        inode inode;
        inode.read(is);
        inode_list.push_back(inode);
    }

    block_list.reserve(meta_data.block_total);
    for (int j = 0; j < meta_data.block_total; ++j) {
        data_block block;
        block.read(is);
        block_list.push_back(block);
    }
}

filesystem::~filesystem()
{
    generate_image();
}

filesystem::filehander filesystem::fopen(std::string_view path_)
{
    std::string path(path_);
    auto tokens = utility::split(path, '/');
    auto inode_index = path_to_inode(tokens, path);
    return filehander(*this, inode_index);
}

void filesystem::fcreate(std::string_view path_)
{
    std::string path(path_);
    auto tokens = utility::split(path, '/');

    auto new_file_name = std::move(tokens.back());

    if (new_file_name.length() >= sizeof(inode{}.name))
        throw std::logic_error("The Filename Length Must Be Less Than " + std::to_string(sizeof(inode{}.name) - 1));

    tokens.pop_back();

    int dir = path_to_inode(tokens, path);

    assert(inode_list[dir].valid);
    assert(inode_list[dir].is_dir());
    assert(inode_bitmap[dir]);

    auto& directory_inode = inode_list[dir];

    int next_slot = 2;
    for (; next_slot < directory_inode.direct_block.size() && directory_inode.direct_block[next_slot] != kNULL; ++next_slot)
        ;

    if (next_slot == directory_inode.direct_block.size())
        throw std::logic_error("A Directory Can Only Contain " + std::to_string(directory_inode.direct_block.size()) + " At Most.");

    directory_inode.direct_block[next_slot] = create_unlinked_file(path, dir);
}

void filesystem::delete_directory_inode(int index)
{
    auto& inode = inode_list[index];

    assert(inode.valid);
    assert(inode.is_dir());
    assert(inode.unix_time != 0);

    if (inode.direct_block[1] == -1)
        throw std::logic_error("Cannot Remove Root Directory!");

    inode.valid = false; // Invalid the flag.

    // Remove The Bitmap.
    inode_bitmap[index] = false;

    // Remove From Father Directory.
    auto father_index = inode.direct_block[1];
    auto& father_inode = inode_list[father_index];
    for (int i = 2; i < father_inode.direct_block.size(); ++i) {
        if (father_inode.direct_block[i] == index) {
            for (int j = i + 1; j < father_inode.direct_block.size(); ++j)
                if (father_inode.direct_block[j] != kNULL)
                    std::swap(father_inode.direct_block[j], father_inode.direct_block[j - 1]);
            return;
        }
    }

    for (int i = 2; i < inode.direct_block.size(); ++i) {
        int sub_index = inode.direct_block[i];
        if (kNULL == sub_index)
            break;
        auto& sub_node = inode_list[sub_index];
        if (sub_node.is_dir())
            delete_directory_inode(sub_index);
        else
            delete_file_inode(sub_index);
    }
}

void filesystem::fdelete(std::string_view path_)
{
    std::string path(path_);
    auto tokens = utility::split(path, '/');
    auto inode_index = path_to_inode(tokens, path);
    delete_file_inode(inode_index);
}

void filesystem::rmdir(std::string_view path_)
{
    std::string path(path_);
    auto tokens = utility::split(path, '/');
    auto inode_index = path_to_inode(tokens, path);
    delete_directory_inode(inode_index);
}

void filesystem::mkdir(std::string_view path_)
{
    std::string path(path_);
    auto tokens = utility::split(path, '/');
    auto new_dir_name = std::move(tokens.back());

    if (new_dir_name.length() >= sizeof(inode{}.name))
        throw std::logic_error("The Directory Name Length Must Be Less Than " + std::to_string(sizeof(inode{}.name) - 1));
    tokens.pop_back();

    int father_dir = path_to_inode(tokens, path);
    auto& directory_inode = inode_list[father_dir];

    assert(inode_list[father_dir].valid);
    assert(inode_list[father_dir].is_dir());
    assert(inode_bitmap[father_dir]);

    // OK, we got the root path now. Let's create a new one.
    int next_slot = 2;
    for (; next_slot < directory_inode.direct_block.size() && directory_inode.direct_block[next_slot] != kNULL; ++next_slot)
        ;

    if (next_slot == directory_inode.direct_block.size())
        throw std::logic_error("A Directory Can Only Contain " + std::to_string(directory_inode.direct_block.size()) + " At Most.");

    directory_inode.direct_block[next_slot] = create_unlinked_directory(path, father_dir);
}

void filesystem::delete_file_inode(int index)
{
    auto& inode = inode_list[index];

    assert(inode.valid);
    assert(!inode.is_dir());
    assert(inode.unix_time != 0);

    inode.valid = false; // Invalid the flag.

    // Clean Block Bitmap First.
    int direct_block_index = 1;
    while (direct_block_index < inode.direct_block.size() && inode.direct_block[direct_block_index] != kNULL) {
        block_bitmap[inode.direct_block[direct_block_index++]] = false;
    }

    if (inode.direct_block.size() == direct_block_index) {
        // Linked List Mode.
        auto& block = block_list[inode.direct_block.back()];
        int block_index = block.next;

        while (block_index != kNULL) {
            block_bitmap[block_index] = false;
            block_index = block_list[block_index].next;
        }
    }

    // Block Data Cleaned. Now lets clean the inode data.
    auto& father_inode = inode_list[inode.direct_block[0]];
    for (int i = 2; i < father_inode.direct_block.size(); ++i) {
        if (father_inode.direct_block[i] == index) {
            for (int j = i + 1; j < father_inode.direct_block.size(); ++j)
                if (father_inode.direct_block[j] != kNULL)
                    std::swap(father_inode.direct_block[j], father_inode.direct_block[j - 1]);
            return;
        }
    }

    assert(false); // Cannot find current inode in his father directory.
}

int filesystem::create_unlinked_file(const std::string& new_file_name, int dir_ind)
{
    auto it = std::find(inode_bitmap.begin(), inode_bitmap.end(), true);
    if (it == inode_bitmap.end())
        throw std::logic_error("There's Not Enough Inodes Now!");

    int new_inode_index = std::distance(inode_bitmap.begin(), it);

    inode_bitmap[new_inode_index] = true;
    auto& new_inode = inode_list[new_inode_index];

    new_inode.valid = true;
    new_inode.is_directory = false;
    new_file_name.copy(new_inode.name, new_file_name.length());
    new_inode.unix_time = std::time(nullptr);
    new_inode.direct_block[0] = dir_ind;
    new_inode.size = 0;

    return new_inode_index;
}

int filesystem::create_unlinked_directory(const std::string& new_dir_name, int dir_index)
{
    auto it = std::find(inode_bitmap.begin(), inode_bitmap.end(), true);
    if (it == inode_bitmap.end())
        throw std::logic_error("There's Not Enough Inodes Now!");

    int new_inode_index = std::distance(inode_bitmap.begin(), it);

    inode_bitmap[new_inode_index] = true;
    auto& new_inode = inode_list[new_inode_index];

    new_inode.valid = true;
    new_inode.is_directory = true;
    new_dir_name.copy(new_inode.name, new_dir_name.length());
    new_inode.unix_time = std::time(nullptr);
    new_inode.direct_block[0] = new_inode_index;
    new_inode.direct_block[1] = dir_index;
    new_inode.size = 0;

    return new_inode_index;
}

int filesystem::path_to_inode(const std::vector<std::string>& tokens, const std::string& path)
{
    if (tokens.empty() || !tokens.front().empty())
        throw std::logic_error("Path Error[We Only Support Global Path!]: Can Not Recognize Root Path.");

    int last_dir_index = 0;
    for (size_t i = 1; i < tokens.size(); ++i) {
        const auto& inode_ = inode_list.at(last_dir_index);
        assert(inode_.valid && inode_.is_dir());
        for (int j = 2; j < inode_.direct_block.size(); ++j) {
            int next_index = inode_.direct_block[j];
            if (next_index == kNULL || inode_list[next_index].name != tokens[i])
                throw std::logic_error("Cannot Find Directory [" + tokens[i] + "] in [" + path + "]");
            last_dir_index = next_index;
        }
    }
    return last_dir_index;
}

void filesystem::generate_image()
{
    std::fstream os(mount_point, std::ios::ate | std::ios::out);
    if (!os.is_open())
        throw std::logic_error("Cannot Write To Image File: " + std::string(mount_point));

    meta_data.write(os);

    for (auto&& inode : inode_list)
        inode.write(os);

    for (auto&& blk : block_list)
        blk.write(os);
}

void filesystem::create_image(int count_blocks)
{
    meta_data.block_total = count_blocks;
    meta_data.inode_total = std::max(1, static_cast<int>(count_blocks * kInodePercent));

    inode_list = std::vector<inode>(meta_data.inode_total);
    block_list = std::vector<data_block>(meta_data.block_total);

    inode_list.front().valid = true;
    inode_list.front().size = 0;
    inode_list.front().is_directory = true;
    inode_list.front().unix_time = std::time(nullptr);
    inode_list.front().current_dir() = 0;
    inode_list.front().last_level_dir() = -1;

    this->generate_image();

    // MK ROOT DIR.
    block_bitmap = decltype(block_bitmap)(meta_data.block_total, false);
    block_bitmap[0] = true;
    inode_bitmap = decltype(inode_bitmap)(meta_data.inode_total, false);
    inode_bitmap[0] = true;

    std::cout << "Successfully Created Filesystem : " << mount_point << std::endl;
}

filesystem::filesystem(const std::string& path)
    : mount_point(path)
{
    this->load_image();
    this->scan_bitmap();
}

filesystem::filesystem(int count_blocks, const std::string& path)
    : mount_point(path)
{
    this->create_image(count_blocks);
}

void filesystem::mark_bitmap(int inode_id)
{
    auto root = inode_list[inode_id];
    assert(root.valid);

    inode_bitmap[inode_id] = true;

    if (root.is_dir()) {
        int i = 2; // For dir.
        while (i < root.direct_block.size() && root.direct_block[i] != kNULL)
            mark_bitmap(root.direct_block[i]);
    } else { // For file.
        int i = 1;
        while (i < root.direct_block.size() && root.direct_block[i] != kNULL)
            block_bitmap[root.direct_block[i]] = true;
        if (root.direct_block[i] != kNULL) {
            // Linked List Mode.
            auto block = block_list[root.direct_block[i]];
            while (block.next != kNULL) {
                block_bitmap[block.next] = true;
                block = block_list[block.next];
            }
        }
    }
}

void filesystem::scan_bitmap()
{
    mark_bitmap(0);
}

std::string filesystem::filehander::read(int size)
{
    std::string ret;
    const auto& inode = m_fs_ref.inode_list[m_inode_id];

    assert(inode.valid);
    assert(!inode.is_dir());
    assert(inode.unix_time != 0);

    if (m_seekp + size > inode.size)
        throw std::logic_error(
            "Overflow When Reading File. Your File Only Has " + std::to_string(inode.size) + " Bytes. But You Want To Read " + std::to_string(size) + " Bytes From Point " + std::to_string(m_seekp));

    int curr_point = 0;
    for (int i = 1; i < inode.direct_block.size(); ++i) {
        const auto& blk = m_fs_ref.block_list[inode.direct_block[i]];

        if (curr_point + blk.size >= m_seekp) { // Now We Can Read!
            if (curr_point > m_seekp + size) // No More Bytes To Read...
                break;
            int bytes_to_read_in_this_block = std::min(blk.size, m_seekp + size - curr_point);
            std::string_view data_view(blk.data_content, bytes_to_read_in_this_block);
            ret += data_view;
        }

        curr_point += blk.size;
    }

    return ret;
}

void filesystem::filehander::write(std::string_view data)
{ // Currently This Is Implemented In Append Fashion.
    int read_index = 0;
    auto& inode = m_fs_ref.inode_list[m_inode_id];

    assert(inode.valid);
    assert(!inode.is_dir());
    assert(inode.unix_time != 0);

    int index_in_inode = 1;
    for (; index_in_inode < inode.direct_block.size(); ++index_in_inode) {
        if (inode.direct_block[index_in_inode] == kNULL) {
            // Check If Current Block Is The Answer.
            if (index_in_inode == 1 || m_fs_ref.block_list[inode.direct_block[index_in_inode - 1]].size == data_block::kContentSize)
                break;
            --index_in_inode;
            break;
        }
    }

    // Pad The Unfilled Block.
    if (index_in_inode < inode.direct_block.size() && inode.direct_block[index_in_inode] != kNULL) { // This Means Padding.
        auto& blk = m_fs_ref.block_list[inode.direct_block[index_in_inode]];
        assert(blk.size != data_block::kContentSize);

        const int read_amount = std::min(blk.kContentSize - blk.size, static_cast<int>(data.size() - read_index));
        data.copy(blk.data_content + blk.size, read_amount);
        read_index += read_amount;

        ++index_in_inode;
    }

    // Entire Blocks.
    int left_amount = data.size() - read_index;
    int blk_num_needed = (left_amount + data_block::kContentSize - 1) / data_block::kContentSize;

    if (blk_num_needed == 0)
        return;

    // Allocate Blocks.
    std::vector<int> blk_indexes{};
    for (int i = 1; i < m_fs_ref.block_bitmap.size() && blk_indexes.size() < blk_num_needed; ++i) {
        if (!m_fs_ref.block_bitmap[i])
            blk_indexes.push_back(i);
    }

    if (blk_indexes.size() < blk_num_needed)
        throw std::logic_error("Blocks Not Enough! " + std::to_string(blk_num_needed - blk_indexes.size()) + " required.");

    // Mark the bitmap.
    for (int ind : blk_indexes)
        m_fs_ref.block_bitmap[ind] = true;

    // Fill the blocks.
    int link_index = 0;

    // Connect the indexes in the inode.
    for (; index_in_inode < inode.direct_block.size() && link_index < blk_indexes.size(); ++index_in_inode) {
        inode.direct_block[index_in_inode] = blk_indexes[link_index++];
    }

    if (index_in_inode == inode.direct_block.size()) { // Linked List Mode.
        // Let's Find The Tail First.
        int next_blk_id = inode.direct_block.back();

        while (true) {
            auto& blk = m_fs_ref.block_list[next_blk_id];
            if (blk.next != kNULL) {
                next_blk_id = blk.next;
            } else
                break;
        }

        for (; link_index < blk_indexes.size(); ++link_index) {
            auto& blk = m_fs_ref.block_list[next_blk_id];
            blk.next = link_index;
            next_blk_id = blk_indexes[link_index];
        }
    }

    for (const auto& ind : blk_indexes) { // Fill The Contents. // !Size
        auto& blk = m_fs_ref.block_list[ind];
        const int read_amount = std::min(blk.kContentSize, static_cast<int>(data.size() - read_index));

        std::string_view data_(data.data() + read_index, data.size() - read_index);
        data_.copy(blk.data_content, read_amount);
        blk.size = read_amount;

        read_index += read_amount;
    }
}

void filesystem::filehander::seekp(int p)
{
    m_seekp = p;
}

}