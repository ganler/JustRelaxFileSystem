#pragma once

#include "details/data_block.hpp"
#include "details/inode.hpp"
#include "details/super_block.hpp"
#include <fstream>
#include <string_view>
#include <vector>

namespace jrfs {

/// \brief　文件系统类，包含对整个文件系统的系统调用
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
    std::vector<inode> inode_list; ///> 文件系统inode部分对应内存的映射
    std::vector<data_block> block_list; ///> 文件系统存储块部分对应内存的映射

    /// \brief 文件系统用于操控文件读写的API，类似于C++的std::fstream和C标准库的fread/fwrite操作
    struct filehander {

        /// 将文件视为逻辑上的一个byte block，seekp用于选择当前文件读写指针定位
        /// \note 初始化为0
        /// \param p 读写指针定位处
        void seekp(int p);

        /// 向文件写入数据
        /// \throws std::logic_error
        /// \param data 被写入的数据字节流
        void write(const std::string_view data);

        /// 从文件中读取数据
        /// \throws std::logic_error
        /// \param size 读取数据字节流的大小
        /// \return 直接以字符串的形式返回
        std::string read(int size) const;

        /// 当前对应的inode下标
        /// \return 当前对应的inode下标
        int node_id() const;

        /// 构造函数
        /// \param fs 绑定的文件系统对象
        /// \param ind 对应的文件inode下标
        inline filehander(filesystem& fs, int ind)
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

    /// \breif [高层API] 创建文件夹
    /// \throws std::logic_error
    /// \param path 文件夹路径，如`/path/to/dir`
    void mkdir(std::string_view path);

    /// \breif [高层API] 删除文件夹
    /// \throws std::logic_error
    /// \param path 文件夹路径，如`/path/to/dir`
    void rmdir(std::string_view path);

    /// \breif [高层API] 创建文件
    /// \throws std::logic_error
    /// \param path 文件路径，如`/path/to/file`
    void fcreate(std::string_view path);

    /// \breif [高层API] 打开文件夹，返回handler
    /// \param path 件路径，如`/path/to/file`
    /// \throws std::logic_error
    /// \return 返回filehander对象，可对其进行文件读写
    filehander fopen(std::string_view path);

    /// \breif [高层API] 删除文件
    /// \throws std::logic_error
    /// \param path 件路径，如`/path/to/file`
    void fdelete(std::string_view path);

    // Low Level API (Dangerous)
    /// \breif [底层API] 删除文件对应的inode
    /// \param index inode下标
    void delete_file_inode(int index);

    /// \breif [底层API] 删除文件夹对应的inode
    /// \param index inode下标
    void delete_directory_inode(int index);

    /// \breif [底层API] 将文件（夹）路径转化为inode下标
    /// \param tokens 路径字符串数组
    /// \param path 源路径（用于报错）
    /// \return inode下标
    int path_to_inode(const std::vector<std::string>& tokens, const std::string& path); // 只支持全局路径(can be file | directory)

    /// \breif [底层API] 将文件（夹）路径转化为inode下标
    /// \param path 文件（夹）路径
    /// \return inode下标
    int path_to_inode(const std::string& path);

    /// \breif [底层API] 创建文件inode
    /// \param new_file_name 文件名
    /// \param dir_index 所在文件夹的inode下标
    /// \return inode下标
    int create_file_inode(const std::string& new_file_name, int dir_index);

    /// \breif [底层API] 创建文件夹inode
    /// \param new_dir_name 文件夹名
    /// \param dir_index 当前文件夹的inode下标
    /// \return inode下标
    int create_dir_inode(const std::string& new_dir_name, int dir_index);

    /// \breif [底层API] 加载镜像
    /// \note 所需的数据信息（如文件位置）已经在filesystem类初始化的时候得到
    void load_image();

    /// \breif [底层API] 构建镜像
    /// \param count_blocks 镜像所需的block的大小
    void create_image(int count_blocks);

    /// \breif [底层API] 同步内存与磁盘中的镜像
    void sync_image();

    /// \breif [底层API] 检查bitmap和当前文件系统是否一致
    void scan_bitmap();

    /// \breif [底层API] 标记一个inode（文件夹/文件）下所对应的所有inode和block块
    /// \param inode_id inode下标
    void mark_bitmap(int inode_id);
};
}
