#pragma once

#include "config.hpp"
#include <fstream>

namespace jrfs {

struct alignas(kSuperBlockSize) super_block { // 使用alignas可以自动补全大小。
    static constexpr int magic = 0x233333; ///< 用来标识文件系统的编号，镜像若前4byte不一致则说明不属于本文件系统；
    int block_total;
    int inode_total;

    void read(std::fstream& istream);
    void write(std::fstream& ostream);
};

}