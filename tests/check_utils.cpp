#include <JRFS/util/utility.hpp>
#include <gtest/gtest.h>

template <typename CL, typename CR>
bool equal_container(const CL& l, const CR& r) {
    return std::equal(l.cbegin(), l.cend(), r.cbegin());
}

TEST(Utility, CheckSplitFunction0) {
    std::string src = "/";
    auto tokens = jrfs::utility::split(src, '/');
    EXPECT_TRUE(equal_container(tokens, std::vector<std::string>{""}));
}

TEST(Utility, CheckSplitFunction1) {
    std::string src = "/hello.txt";
    auto tokens = jrfs::utility::split(src, '/');
    EXPECT_TRUE(equal_container(tokens, std::vector<std::string>{"", "hello.txt"}));
}

TEST(Utility, CheckSplitFunction2) {
    std::string src = "/what/the/f";
    auto tokens = jrfs::utility::split(src, '/');
    EXPECT_TRUE(equal_container(tokens, std::vector<std::string>{"", "what", "the", "f"}));
}