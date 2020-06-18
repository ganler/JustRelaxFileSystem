#include "test_util.hpp"

TEST(JRFSFileAndDir, CheckRootDir)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_EQ(0, image.path_to_inode({ "" }, "/"));
        EXPECT_TRUE(image.inode_list[0].valid);
        EXPECT_TRUE(image.inode_list[0].is_dir());
        EXPECT_EQ(image.inode_list[0].name, std::string(""));
        EXPECT_EQ(image.inode_list[0].current_dir(), 0);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}

TEST(JRFSFileAndDir, CheckPathNotFoundThrow)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_THROW(image.path_to_inode({ "", "path", "to", "nowhere" }, "/path/to/nowhere"), std::logic_error);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}

TEST(JRFSFileAndDir, CheckFileCreation)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        image.fcreate("/lrznb.txt");

        auto& inode = image.inode_list[0]; // Get Root.

        EXPECT_NE(inode.direct_block[2], jrfs::kNULL);

        auto& file_inode = image.inode_list[inode.direct_block[2]];

        EXPECT_TRUE(file_inode.valid);
        EXPECT_TRUE(!file_inode.is_dir());
        EXPECT_EQ(file_inode.current_dir(), 0);
        EXPECT_EQ(file_inode.direct_block[1], jrfs::kNULL);
        EXPECT_EQ(file_inode.name, std::string("lrznb.txt"));
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str()));
}

TEST(JRFSFileAndDir, CheckFileDeletion)
{
    // TODO.
}

TEST(JRFSFileAndDir, CheckDirectoryCreation)
{
    // TODO.
}

TEST(JRFSFileAndDir, CheckDirectoryDeletion)
{
    // TODO.
}

TEST(JRFSFileAndDir, CheckSmallFileWriting)
{
    // TODO.
}

TEST(JRFSFileAndDir, CheckSmallFileReading)
{
    // TODO.
}

TEST(JRFSFileAndDir, CheckBigFileReading)
{
    // TODO.
}

TEST(JRFSFileAndDir, CheckBigFileWriting)
{
    // TODO.
}