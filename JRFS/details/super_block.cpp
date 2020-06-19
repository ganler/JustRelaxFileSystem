#include "super_block.hpp"
#include "../util/utility.hpp"
#include <bitset>
#include <iostream>

namespace jrfs {

void super_block::read(std::fstream& istream)
{
    int check_magic;
    llread(istream, check_magic);
    if (check_magic != magic)
        throw std::logic_error(
            "Magic Number Not Match! Unrecognizable superblock! Expected : " + std::to_string(magic) + "(" + std::bitset<sizeof(int) * 8>(magic).to_string() + "), however got: " + std::to_string(check_magic) + "(" + std::bitset<sizeof(int) * 8>(check_magic).to_string() + ")");

    llread(istream, block_total);
    llread(istream, inode_total);
}

void super_block::write(std::fstream& ostream) const
{
    llwrite(ostream, magic);
    llwrite(ostream, block_total);
    llwrite(ostream, inode_total);
}

}