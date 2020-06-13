#pragma once

#include "config.hpp"
#include "data_block.hpp"
#include <array>
#include <ctime>

namespace jrfs {

struct alignas(kInodeSize) inode {
    int valid = kNULL; ///> 是否当前inode正在被使用
    int size = 0; ///> inode数据的逻辑字节大小
    int is_directory = false; ///> 是否是文件夹

    char name[32] = "";
    uint32_t unix_time {};

    std::array<int, 20> direct_block {}; ///> 直接索引的数据块
    // Dir:  [Curr] [UpLevel] [...]
    // File: [Curr] [...]

    static constexpr int kMaxFileSize = sizeof(direct_block) / sizeof(int) * data_block{}.kContentSize;
public:
    bool is_dir() const;

    inline int& current_dir()
    {
        assert(is_directory);
        return direct_block[0];
    }

    inline int& last_level_dir()
    {
        assert(is_directory);
        return direct_block[1];
    }

    inline const int& current_dir() const
    {
        assert(is_directory);
        return direct_block[0];
    }

    inline const int& last_level_dir() const
    {
        assert(is_directory);
        return direct_block[1];
    }

    void read(std::fstream& istream);
    void write(std::fstream& ostream);
};

inode make_empty_dir();

static_assert(sizeof(inode) == kInodeSize, "A inode must be less than or equal to 128 bytes.");

}