#pragma once

#include "details/data_block.hpp"
#include "details/inode.hpp"
#include "details/super_block.hpp"
#include <fstream>
#include <string_view>
#include <vector>

namespace jrfs {
struct filesystem {
    filesystem(const std::string& path); // Load filesystem;
    filesystem(int count_blocks, const std::string& path); // Create filesystem;
    ~filesystem();

    super_block super_block; ///> 文件系统的元数据
    std::string_view mount_point; ///> 原来镜像的位置
    std::vector<char> block_bitmap; ///> 对于全局所有block的标记，如果是空闲的则为0，否则为1
    std::vector<char> inode_bitmap; ///> 对于全局inode进行标记，如果是空闲的则为0，否则为1
    std::vector<inode> inode_list;
    std::vector<data_block> block_list;

    struct filehander{
        void seekp(int p);
        void write(std::string_view data);
        std::string read(int size);
        filehander(filesystem& fs, int ind) : m_fs_ref(fs), inode_id(ind) {}
    private:
        filesystem& m_fs_ref;
        int inode_id;
        int m_seekp = 0;
    };

    // High Level API (Safe)
    void mkdir(std::string_view path);
    void rmdir(std::string_view path);

    void fcreate(std::string_view path); // Create what hasn't existed.
    filehander fopen(std::string_view path);
    void fdelete(std::string_view path);

    // Low Level API (Dangerous)
    int path_to_inode(const std::vector<std::string>& tokens, const std::string& path); // 只支持全局路径(can be file | directory)
    int create_unlinked_file(const std::string& new_file_name);

    void load_image();
    void create_image(int count_blocks);
    void generate_image();
    void scan_bitmap();
    void mark_bitmap(int inode_id);
};
}
