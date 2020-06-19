#pragma once

#include "details/data_block.hpp"
#include "details/inode.hpp"
#include "details/super_block.hpp"
#include <fstream>
#include <string_view>
#include <vector>

namespace jrfs {

/// \brief　文件系统类，包含对整个文件系统的系统调用。
struct filesystem {
    ///
    /// \param path
    filesystem(const std::string& path); // Load filesystem;

    ///
    /// \param count_blocks
    /// \param path
    filesystem(int count_blocks, const std::string& path); // Create filesystem;

    /// \brief 文件系统析构函数，会最后对文件系统进行一次整体同步
    ~filesystem();

    super_block meta_data; ///> 文件系统的元数据
    const std::string& mount_point; ///> 原来镜像的位置
    std::vector<char> block_bitmap; ///> 对于全局所有block的标记，如果是空闲的则为0，否则为1
    std::vector<char> inode_bitmap; ///> 对于全局inode进行标记，如果是空闲的则为0，否则为1
    std::vector<inode> inode_list; ///> 
    std::vector<data_block> block_list;

    struct filehander {
        void seekp(int p);
        void write(const std::string_view data);
        std::string read(int size) const;
        int node_id() const;
        filehander(filesystem& fs, int ind)
            : m_fs_ref(fs)
            , m_inode_id(ind)
        {
        }

    private:
        filesystem& m_fs_ref;
        const int m_inode_id;
        int m_seekp = 0;
    };

    // High Level API (Safe)
    void mkdir(std::string_view path);
    void rmdir(std::string_view path);

    void fcreate(std::string_view path); // Create what hasn't existed.
    filehander fopen(std::string_view path);
    void fdelete(std::string_view path);

    // Low Level API (Dangerous)
    void delete_file_inode(int index);
    void delete_directory_inode(int index);
    int path_to_inode(const std::vector<std::string>& tokens, const std::string& path); // 只支持全局路径(can be file | directory)
    int path_to_inode(const std::string& path);
    int create_unlinked_file(const std::string& new_file_name, int dir_index);
    int create_unlinked_directory(const std::string& new_dir_name, int dir_index);

    void load_image();
    void create_image(int count_blocks);
    void generate_image();
    void scan_bitmap();
    void mark_bitmap(int inode_id);
};
}
