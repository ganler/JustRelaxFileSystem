#pragma once

#include "config.hpp"

#include <fstream>

namespace jrfs {
struct data_block {
    static constexpr int kContentSize = kBlockSize - sizeof(int) * 2;

    int next = kNULL;
    int size = 0;
    char data_content[kContentSize];

    void read(std::fstream& fstream);
    void write(std::fstream& fstream) const;
};

static_assert(sizeof(data_block) == kBlockSize, "Block Size Is not 512!");

}