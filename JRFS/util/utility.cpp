#include "utility.hpp"

namespace jrfs {
namespace utility {
    std::vector<std::string> split(std::string strtem, char a)
    {
        using namespace std;
        if (strtem.size() == 1 && strtem.front() == a)
            return { "" };

        vector<string> strvec;
        string::size_type pos1, pos2;
        pos2 = strtem.find(a);
        pos1 = 0;
        while (string::npos != pos2) {
            strvec.push_back(strtem.substr(pos1, pos2 - pos1));

            pos1 = pos2 + 1;
            pos2 = strtem.find(a, pos1);
        }

        strvec.push_back(strtem.substr(pos1));

        if (strvec.back().empty())
            strvec.pop_back();
        return strvec;
    }
}
}