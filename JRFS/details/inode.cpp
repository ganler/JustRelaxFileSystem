#include "inode.hpp"
#include <iomanip>
#include <fstream>

namespace jrfs {

    bool inode::is_dir() const {
        return is_directory;
    }

    void inode::write(std::fstream& fstream) {
        fstream << valid;
        fstream << size;
        fstream << is_directory;
        fstream << std::setfill(' ') << std::setw(sizeof(name)) << name;
        fstream << unix_time;
        for(auto&& db : direct_block)
            fstream << db;
        for(auto&& id : indirect_block)
            fstream << id;
    }

    void inode::read(std::fstream& fstream) {
        fstream >> valid;
        fstream >> size;
        fstream >> is_directory;
        fstream >> name;
        fstream >> unix_time;
        for(auto&& db : direct_block)
            fstream >> db;
        for(auto&& id : indirect_block)
            fstream >> id;
    }

}