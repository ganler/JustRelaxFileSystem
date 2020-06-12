#pragma once

#include "details/inode.hpp"
#include "filesystem.hpp"
#include <string>
#include <string_view>

namespace jrfs {

class file_handler {
public:
    file_handler(inode& inode_ref, filesystem& fs_ref)
        : m_seekp(0)
        , m_inode_ref(inode_ref)
        , m_fs_ref
    {
        assert(!inode_ref.is_dir());
    }
    void seekp(int pos)
    {
        m_seekp = pos;
    }

    void write(std::string_view data)
    {
        // FIXME: Append Only.
        int i = 1;
        while (i < m_inode_ref.direct_block.size() && m_inode_ref.direct_block[i] != kNULL)
            ;
    }

    std::string read(int size)
    {
        std::string ret;
        if (m_seekp + size > m_inode_ref.size)
            throw std::logic_error("Cross Border When Reading The File: seekp = " + std::to_string(m_seekp) + ", readsize = " + std::to_string(size) + ", file size: " + std::to_string(m_inode_ref.size) + '.');

        int p = 0;

        for (int i = 1; i < m_inode_ref.direct_block.size(); ++i) {
            int index = m_inode_ref.direct_block[i];
            auto blk = m_fs_ref.get_block();
            if (p + size > m_seekp) {
                int read_size = std::min(blk.size, size - (p - m_seekp));
                std::istreambuf_iterator<char> it(m_fs_ref);
                std::copy_n(it, read_size, std::back_insert_iterator(ret));
                if (p + blk.size - m_seekp >= size)
                    break;
            }
            p += blk.size;
        }

        return ret;
    }

private:
    int m_seekp;
    inode& m_inode_ref;
    filesystem& m_fs_ref;
};

}