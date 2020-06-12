#include "filesystem.hpp"
#include <iostream>
#include "details/inode.hpp"

namespace jrfs{

void filesystem::load_image(const std::string& path){
    m_fstream = std::fstream(path, std::ios::out | std::ios::in);
    if (!m_fstream.is_open())
        throw std::logic_error("Cannot Open Image File: " + path);

    super_block.read(m_fstream);
}

void filesystem::create_image(int count_blocks, const std::string& path) {
    m_fstream = std::fstream(path, std::ios::out | std::ios::in);

    if (!m_fstream.is_open())
        throw std::logic_error("Cannot Create Image File: " + path);

    super_block.block_total = count_blocks;
    super_block.inode_free = super_block.inode_total = std::max(1, static_cast<int>(count_blocks * kInodePercent));
    super_block.block_free = super_block.block_total - super_block.inode_total;

    super_block.write(m_fstream); // Write to filesystem.

    {
        m_fstream.seekp(inode_begin_index());
        inode dummy;
        for (int i = 0; i < super_block.inode_total; ++i)
            dummy.write(m_fstream);
    }

    m_fstream.seekp(inode_begin_index());
    auto root = make_empty_dir();
    root.current_dir() = 0;
    root.last_level_dir() = -1;
    root.write(m_fstream);

    m_fstream.seekp(kBlockSize * count_blocks - 1);
    m_fstream.write("", 1);

    std::cout << "Successfully Created Filesystem : " << path << std::endl;
}

filesystem::filesystem(const std::string& path) : mount_point(path) {
    this->load_image(path);
    this->scan_bitmap();
}

filesystem::filesystem(int count_blocks, const std::string& path) : mount_point(path) {
    this->create_image(count_blocks, path);
    block_bitmap = decltype(block_bitmap)(super_block.block_total, false);
    block_bitmap[0] = false;
}

int filesystem::inode_begin_index(){
    return sizeof(super_block);
}

int filesystem::block_begin_index(){
    return inode_begin_index() + super_block.inode_total * sizeof(inode);
}

inode filesystem::get_inode(int inode_id){
    m_fstream.seekp(inode_begin_index() + sizeof(inode) * inode_id);
    inode root;

    root.read(m_fstream);

    return root;
}

inode filesystem::get_root(){
    auto root = get_inode(0);
    assert(root.is_dir());
    if (!root.is_dir())
        throw std::logic_error("Root is not a directory? Error JRFS format!");
    return root;
}

void filesystem::mark_bitmap(int inode_id) {
    auto root = this->get_inode(inode_id);
    assert(root.valid);

    inode_bitmap[inode_id] = true;

    if (root.is_dir()) {
        for (int i = 2; i < root.direct_block.size(); ++i) {
            if (root.direct_block[i] != kNULL) { // Sth. there.
                mark_bitmap(root.direct_block[i]);
            }
        }

        for (int j = 0; j < root.indirect_block.size(); ++j) {
            if (root.indirect_block[j] != kNULL) { // Sth. there.
                mark_bitmap(root.indirect_block[j]);
            }
        }
    } else { // File.
        for (int i = 1; i < root.direct_block.size(); ++i) {
            if (root.direct_block[i] != kNULL) { // Sth. there
                block_bitmap[root.direct_block[i]] = true;
            }
        }

        for (const auto& iblock : root.indirect_block) {
            block_bitmap[iblock] = true;
            if (iblock != kNULL) {
                auto blk = get_indirect_block(iblock);
                for (auto&& id : blk.blocks_indexes)
                    if (id != kNULL)
                        block_bitmap[id] = true;
            }
        }
    }
}

void filesystem::scan_bitmap(){
    mark_bitmap(0);
}

indirect_block filesystem::get_indirect_block(int id){
    m_fstream.seekp(block_begin_index() + kBlockSize * id);
    indirect_block root;
    root.read(m_fstream);
    return root;
}

}