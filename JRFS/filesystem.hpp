#ifndef JUSTRELAXFILESYSTEM_FILESYSTEM_HPP
#define JUSTRELAXFILESYSTEM_FILESYSTEM_HPP

#include <fstream>
#include <vector>
#include <string_view>
#include "details/super_block.hpp"
#include "details/inode.hpp"
#include "details/indirect_block.hpp"

namespace jrfs{
struct filesystem {
    filesystem(const std::string& path);                   // Load filesystem;
    filesystem(int count_blocks, const std::string& path); // Create filesystem;

    super_block super_block; ///> 文件系统的元数据
    std::string_view mount_point; ///> 原来镜像的位置
    std::vector<bool> block_bitmap; ///> 对于全局所有block的标记，如果是空闲的则为0，否则为1
    std::vector<bool> inode_bitmap; ///> 对于全局inode进行标记，如果是空闲的则为0，否则为1
private:
    std::fstream m_fstream;

    void load_image(const std::string& path);
    void create_image(int count_blocks, const std::string& path);
    void scan_bitmap();
    void mark_bitmap(int inode_id);
    inode get_root();
    inode get_inode(int id);
    indirect_block get_indirect_block(int id);

    int inode_begin_index();
    int block_begin_index();
};
}


#endif //JUSTRELAXFILESYSTEM_FILESYSTEM_HPP
