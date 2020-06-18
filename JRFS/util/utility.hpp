#pragma once

#include <string>
#include <vector>

namespace jrfs {

template <typename S, typename T>
void llread(S& stream, T&& v)
{ // Low Level Read.
    stream.read(reinterpret_cast<char*>(&v), sizeof(v));
}

template <typename S, typename T>
void llwrite(S& stream, const T& v)
{ // Low Level Write.
    stream.write(reinterpret_cast<const char*>(&v), sizeof(v));
}

namespace utility {
    std::vector<std::string> split(std::string strtem, char a);
}
}