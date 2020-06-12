#include "inode.hpp"
#include <fstream>
#include <iomanip>

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
    fstream << valid;
    fstream << size;
    fstream << is_directory;
    fstream << std::setfill(' ') << std::setw(sizeof(name)) << name;
    fstream << unix_time;
    for (auto&& db : direct_block)
        fstream << db;
}

void inode::read(std::fstream& fstream)
{
    fstream >> valid;
    fstream >> size;
    fstream >> is_directory;
    fstream >> name;
    fstream >> unix_time;
    for (auto&& db : direct_block)
        fstream >> db;
}

}