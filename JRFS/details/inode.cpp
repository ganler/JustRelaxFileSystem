#include "inode.hpp"
#include <fstream>
#include <iomanip>
#include "../util/utility.hpp"
namespace jrfs {

inode make_empty_dir()
{
    inode ret;
    ret.is_directory = true;
    ret.valid = 1;
    ret.unix_time = std::time(nullptr);
    return ret;
}

bool inode::is_dir() const
{
    return is_directory;
}

void inode::write(std::fstream& fstream)
{
    llwrite(fstream, valid);
    llwrite(fstream, size);
    llwrite(fstream, is_directory);
    llwrite(fstream, name);
    llwrite(fstream, unix_time);
    for (auto&& db : direct_block)
        llwrite(fstream, db);
}

void inode::read(std::fstream& fstream)
{
    llread(fstream, valid);
    llread(fstream, size);
    llread(fstream, is_directory);
    llread(fstream, name);
    llread(fstream, unix_time);
    for (auto&& db : direct_block)
        llread(fstream, db);
}

}