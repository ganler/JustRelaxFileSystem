#include "data_block.hpp"

namespace jrfs {

void data_block::read(std::fstream& fstream)
{
    fstream >> next;
    fstream >> size;
    fstream >> data_content;
}

void data_block::write(std::fstream& fstream) const
{
    fstream << next;
    fstream << size;
    fstream << data_content;
}

}