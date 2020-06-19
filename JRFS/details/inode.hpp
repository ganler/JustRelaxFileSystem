#pragma once

#include "config.hpp"
#include "data_block.hpp"
#include <array>
#include <cassert>
#include <ctime>

namespace jrfs {

/// \brief 文件描述节点（文件元数据）
struct alignas(kInodeSize) inode {
    int valid = kNULL; ///> 是否当前inode正在被使用
    int size = 0; ///> inode数据的逻辑字节大小
    int is_directory = false; ///> 是否是文件夹

    char name[32] = ""; ///> 文件名
    uint32_t unix_time{ 0 }; ///> 文件上一次修改时间

    std::array<int, 20> direct_block{}; ///> 直接索引的数据块
public:
    /// \brief 判断当前inode是否是文件夹
    /// \return 是否是文件夹
    bool is_dir() const;

    /// \brief 查找当前所在文件夹
    /// \return 当前文件夹下标
    inline int& current_dir()
    {
        return direct_block[0];
    }

    /// \brief 上一级文件夹
    /// \return 上一级文件夹下标
    inline int& last_level_dir()
    {
        assert(is_directory);
        return direct_block[1];
    }

    /// \brief 查找当前所在文件夹
    /// \return 当前文件夹下标
    inline const int& current_dir() const
    {
        return direct_block[0];
    }

    /// \brief 上一级文件夹
    /// \return 上一级文件夹下标
    inline const int& last_level_dir() const
    {
        assert(is_directory);
        return direct_block[1];
    }

    /// \param istream 文件系统镜像流
    /// \brief 从文件系统中读取inode块
    void read(std::fstream& istream);

    /// \param ostream 文件系统镜像流
    /// \brief 将inode块写入二级文件系统
    void write(std::fstream& ostream);
};

inode make_empty_dir();

static_assert(sizeof(inode) == kInodeSize, "A inode must be less than or equal to 128 bytes.");

}