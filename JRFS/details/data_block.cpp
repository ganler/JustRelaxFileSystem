#include "data_block.hpp"
#include "../util/utility.hpp"

namespace jrfs {

void data_block::read(std::fstream& fstream)
{
    llread(fstream, next);
    llread(fstream, size);
    llread(fstream, data_content);
}

void data_block::write(std::fstream& fstream) const
{
    llwrite(fstream, next);
    llwrite(fstream, size);
    llwrite(fstream, data_content);
}

}