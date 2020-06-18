#include <JRFS/filesystem.hpp>
#include <gtest/gtest.h>
#include <fstream>

TEST(JRFSImage, Creation) {
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(1000, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}

TEST(JRFSImage, CheckSuperBlock) {
    std::string test_image = "./gtest_image.jrfs";
    constexpr int block_total = 1000;

    {
        jrfs::filesystem image(block_total, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));
    }

    { // Read The Headers.
        std::fstream image(test_image, std::ios::in | std::ios::binary);
        jrfs::super_block check_block;
        EXPECT_NO_THROW([&]{
            try {
                check_block.read(image);
            } catch (const std::exception& err) {
                std::cerr << err.what();
                throw;
            }
        }());

        EXPECT_EQ(check_block.block_total, 1000);
        EXPECT_LE(check_block.inode_total, check_block.block_total);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}