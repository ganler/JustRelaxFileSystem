//
// Created by bytedance on 2020/6/12.
//

#ifndef JUSTRELAXFILESYSTEM_INDIRECT_BLOCK_HPP
#define JUSTRELAXFILESYSTEM_INDIRECT_BLOCK_HPP

#include "config.hpp"
#include <array>
#include <fstream>

namespace jrfs{

struct indirect_block {
    std::array<int, kBlockSize / sizeof(int)> blocks_indexes;

    void read(std::fstream& istream);
    void write(std::fstream& ostream);
};

static_assert(sizeof(indirect_block) == kBlockSize, "The indirect block's size must = 512.");

}


#endif //JUSTRELAXFILESYSTEM_INDIRECT_BLOCK_HPP
