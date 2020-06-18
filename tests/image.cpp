#include "test_util.hpp"

TEST(JRFSImage, CheckCreation) {
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(1000, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}

TEST(JRFSImage, CheckCapacity) {
    for (int i = 0; i < 10; ++i) {
        std::string test_image = "./gtest_image.jrfs";
        {
            jrfs::filesystem image(100 + i * 1000, test_image);

            EXPECT_EQ(0, system(("ls " + test_image).c_str()));
        }
        if (system(("ls " + test_image).c_str()) == 0)
            system(("rm " + test_image).c_str()); // Clean the file.
        EXPECT_NE(0, system(("ls " + test_image).c_str()));
    }
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

TEST(JRFSImage, CheckImageSize) {
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(1000, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));
    }

    {
        // Check File Size.
        std::fstream image(test_image, std::ios::in | std::ios::binary);
        const size_t begin = image.tellg();

        jrfs::super_block check_block;
        check_block.read(image);

        image.seekg(0, std::ios::end);
        const size_t end = image.tellg();

        EXPECT_EQ(end - begin, jrfs::kSuperBlockSize + check_block.block_total * jrfs::kBlockSize + check_block.inode_total * jrfs::kInodeSize);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}

TEST(JRFSImage, CheckLoad) {
    std::string test_image = "./gtest_image.jrfs";
    constexpr int block_total = 1000;

    {
        jrfs::filesystem image(block_total, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));
    }

    { // Read Image.
        jrfs::filesystem fs(test_image);

        EXPECT_EQ(fs.meta_data.block_total, 1000);

        EXPECT_EQ(fs.block_bitmap.size(), fs.meta_data.block_total);
        EXPECT_EQ(fs.block_list.size(), fs.meta_data.block_total);

        EXPECT_EQ(fs.inode_list.size(), fs.meta_data.inode_total);
        EXPECT_EQ(fs.inode_bitmap.size(), fs.meta_data.inode_total);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}