#include "filesystem.hpp"
#include "details/inode.hpp"
#include <iostream>
#include "util/utility.hpp"

namespace jrfs {

void filesystem::load_image()
{
    std::fstream is(mount_point, std::ios::out | std::ios::in);
    if (!is.is_open())
        throw std::logic_error("Cannot Open Image File: " + std::string(mount_point));

    super_block.read(is);

    // TODO: CHECK FILE SIZE.

    inode_list.reserve(super_block.inode_total);
    for (int i = 0; i < super_block.inode_total; ++i) {
        inode inode;
        inode.read(is);
        inode_list.push_back(inode);
    }

    block_list.reserve(super_block.block_total);
    for (int j = 0; j < super_block.block_total; ++j) {
        data_block block;
        block.read(is);
        block_list.push_back(block);
    }
}

filesystem::~filesystem() {
    generate_image();
}

filesystem::filehander filesystem::fopen(std::string_view path_) {
    std::string path(path_);
    auto tokens = utility::split(path, '/');
    auto inode_index = path_to_inode(tokens, path);
    return filehander(*this, inode_index);
}


void filesystem::fcreate(std::string_view path_) {
    std::string path(path_);
    auto tokens = utility::split(path, '/');

    auto new_file_name = std::move(tokens.back());

    if (new_file_name.length() >= sizeof(inode{}.name))
        throw std::logic_error("The Filename Length Must Be Less Than " + std::to_string(sizeof(inode{}.name) - 1));

    tokens.pop_back();

    int dir = path_to_inode(tokens, path);
    assert(inode_list[dir].is_dir());
    assert(inode_bitmap[dir]);

    auto& directory_inode = inode_list[dir];

    int next_slot = 2;
    for(; next_slot < directory_inode.direct_block.size() && directory_inode.direct_block[next_slot] != kNULL; ++next_slot);

    if (next_slot == directory_inode.direct_block.size())
        throw std::logic_error("A Directory Can Only Contain " + std::to_string(directory_inode.direct_block.size()) + " At Most.");

    directory_inode.direct_block[next_slot] = create_unlinked_file(path);
}

int filesystem::create_unlinked_file(const std::string &new_file_name) {
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
    new_inode.direct_block[0] = new_inode_index;
    new_inode.size = 0;

    return new_inode_index;
}

int filesystem::path_to_inode(const std::vector<std::string>& tokens, const std::string& path) {
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

void filesystem::generate_image() {
    std::fstream os(mount_point, std::ios::ate | std::ios::out);
    if (!os.is_open())
        throw std::logic_error("Cannot Write To Image File: " + std::string(mount_point));

    super_block.write(os);

    for(auto&& inode : inode_list)
        inode.write(os);

    for(auto&& blk : block_list)
        blk.write(os);
}

void filesystem::create_image(int count_blocks) {
    super_block.block_total = count_blocks;
    super_block.inode_total = std::max(1, static_cast<int>(count_blocks * kInodePercent));

    inode_list = std::vector<inode>(super_block.inode_total);
    block_list = std::vector<data_block>(super_block.block_total);

    inode_list.front().valid = true;
    inode_list.front().size = 0;
    inode_list.front().is_directory = true;
    inode_list.front().unix_time = std::time(nullptr);
    inode_list.front().current_dir() = 0;
    inode_list.front().last_level_dir() = -1;

    this->generate_image();


    block_bitmap = decltype(block_bitmap)(super_block.block_total, false);
    block_bitmap[0] = true;
    inode_bitmap = decltype(inode_bitmap)(super_block.inode_total, false);
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

void filesystem::scan_bitmap() {
    mark_bitmap(0);
}

}