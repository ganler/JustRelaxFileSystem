#include "super_block.hpp"

namespace jrfs {

void super_block::read(std::fstream& istream)
{
    int check_magic;
    istream >> check_magic;
    if (check_magic != magic)
        throw std::logic_error(
            "Magic Number Not Match! Unrecognizable superblock! Expected : " + std::to_string(magic) + ", however got: " + std::to_string(check_magic));

    istream >> block_total;
    istream >> inode_total;
}

void super_block::write(std::fstream& ostream)
{
    ostream << magic;
    ostream << block_total;
    ostream << inode_total;
}

}