#pragma once

#include "config.hpp"

#include <fstream>

namespace jrfs {

/// \brief 数据块的内存映像
struct data_block {
    static constexpr int kContentSize = kBlockSize - sizeof(int) * 2; ///> 数据块最大容量

    int next = kNULL; ///< 后继数据块节点
    int size = 0; ///< 数据块大小
    char data_content[kContentSize]; ///> 数据块内容

    /// \param fstream 文件系统镜像流
    /// \brief 从文件系统中读取数据块
    void read(std::fstream& fstream);

    /// \param fstream 文件系统镜像流
    /// \brief 将数据块写入二级文件系统
    void write(std::fstream& fstream) const;
};

static_assert(sizeof(data_block) == kBlockSize, "Block Size Is not 512!");

}