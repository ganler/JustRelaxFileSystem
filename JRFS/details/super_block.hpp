#pragma once

#include "config.hpp"
#include <fstream>

namespace jrfs {

/// \brief 文件系统元数据
struct super_block {
    static constexpr int magic = 0x233333; ///< 用来标识文件系统的编号，镜像若前4byte不一致则说明不属于本文件系统；
    int block_total; ///< block总数
    int inode_total; ///< inode总数

    /// 从镜像中读出super block
    /// \param istream 镜像fstream
    void read(std::fstream& istream);

    /// 将super block写入镜像
    /// \param ostream 镜像fstream
    void write(std::fstream& ostream) const;
};

static_assert(sizeof(super_block) + 4 == kSuperBlockSize, "Invalid Super Block Size!");

}