//
// Created by bytedance on 2020/6/12.
//

#include "indirect_block.hpp"
namespace jrfs {

void indirect_block::read(std::fstream &istream) {
    for(auto&& index : blocks_indexes)
        istream >> index;
}

void indirect_block::write(std::fstream &ostream) {
    for(auto&& index : blocks_indexes)
        ostream << index;
}

}