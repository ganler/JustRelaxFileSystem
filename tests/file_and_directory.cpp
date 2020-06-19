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
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        image.fcreate("/lrznb.txt");
        image.fdelete("/lrznb.txt");
        EXPECT_THROW(image.fopen("/lrznb.txt"), std::logic_error);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str())); // Expect: The file is removed.
}

TEST(JRFSFileAndDir, CheckDirectoryCreation)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_NO_THROW(image.mkdir("/what"));
        EXPECT_NO_THROW( image.mkdir("/what/the"));

        EXPECT_NO_THROW(image.fcreate("/what/the/hello.txt"));

        int dir_index = image.path_to_inode("/what/the");

        EXPECT_NE(dir_index, jrfs::kNULL);
        EXPECT_GE(dir_index, 0);
        EXPECT_GE(image.inode_list.size(), dir_index);
        EXPECT_TRUE(image.inode_list[dir_index].valid);
        EXPECT_TRUE(image.inode_list[dir_index].is_dir());
        EXPECT_EQ(image.inode_list[dir_index].name, std::string("the"));
        EXPECT_NE(image.inode_list[dir_index].direct_block[2], jrfs::kNULL);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str())); // Expect: The file is removed.
}

TEST(JRFSFileAndDir, CheckDirectoryDeletion)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_NO_THROW(image.mkdir("/what"));
        EXPECT_NO_THROW( image.mkdir("/what/the"));
        EXPECT_NO_THROW(image.rmdir("/what"));

        EXPECT_THROW(image.path_to_inode("/what/the"), std::logic_error);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str())); // Expect: The file is removed.
}

TEST(JRFSFileAndDir, CheckSmallFileReadWrite)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_NO_THROW( image.fcreate("/lrznb.txt"));
        auto handler = image.fopen("/lrznb.txt");

        constexpr std::string_view test_message = "李润中是神！！！";
        EXPECT_NO_THROW(handler.write(test_message));

        handler.seekp(0);
        auto str = handler.read(test_message.size());

        EXPECT_EQ(test_message.size(), str.size());
        EXPECT_EQ(test_message, str);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str())); // Expect: The file is removed.
}

TEST(JRFSFileAndDir, CheckBigFileReadWriteSimple)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_NO_THROW( image.fcreate("/lrznb.txt"));
        auto handler = image.fopen("/lrznb.txt");

        std::string simple(12345, '6');
        EXPECT_GE(simple.size(), jrfs::data_block::kContentSize * (jrfs::inode{}.direct_block.size() - 1));
        EXPECT_NO_THROW(handler.write(simple));

        handler.seekp(0);
        auto str = handler.read(simple.size());

        EXPECT_EQ(simple.size(), str.size());
        EXPECT_EQ(simple, str);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str())); // Expect: The file is removed.
}

TEST(JRFSFileAndDir, CheckBigFileReadWriteComplex)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_NO_THROW( image.fcreate("/lrznb.txt"));
        auto handler = image.fopen("/lrznb.txt");

        std::string test_str("123456789@");
        for (int i = 0; i < 10; ++i) {
            test_str += test_str;
        }
        EXPECT_GE(test_str.size(), jrfs::data_block::kContentSize * (jrfs::inode{}.direct_block.size() - 1));
        EXPECT_NO_THROW(handler.write(test_str));

        handler.seekp(0);
        auto str = handler.read(test_str.size());

        EXPECT_EQ(test_str.size(), str.size());
        EXPECT_EQ(test_str, str);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str())); // Expect: The file is removed.
}

TEST(JRFSFileAndDir, CheckBigFileReadWriteVeryComplex)
{
    std::string test_image = "./gtest_image.jrfs";

    {
        jrfs::filesystem image(100, test_image);

        EXPECT_EQ(0, system(("ls " + test_image).c_str()));

        // Check If Root Node Index == 0
        EXPECT_NO_THROW( image.fcreate("/lrznb.txt"));
        auto handler = image.fopen("/lrznb.txt");

        constexpr std::string_view test_message = R"(李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 我们都知道，只要有意义，那么就必须慎重考虑。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 在这种困难的抉择下，本人思来想去，寝食难安。 塞涅卡曾经说过，生命如同寓言，其价值不在与长短，而在与内容。这启发了我， 我们不得不面对一个非常尴尬的事实，那就是， 所谓李润中牛逼，关键是李润中牛逼需要如何写。 既然如何， 布尔沃在不经意间这样说过，要掌握书，莫被书掌握；要为生而读，莫为读而生。这句话语虽然很短，但令我浮想联翩。 每个人都不得不面对这些问题。 在面对这种问题时， 我认为， 带着这些问题，我们来审视一下李润中牛逼。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 既然如此。
　　经过上述讨论池田大作曾经说过，不要回避苦恼和困难，挺起身来向它挑战，进而克服它。这句话语虽然很短，但令我浮想联翩。 我们不得不面对一个非常尴尬的事实，那就是， 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 一般来说， 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 带着这些问题，我们来审视一下李润中牛逼。 李润中牛逼，到底应该如何实现。 一般来讲，我们都必须务必慎重的考虑考虑。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 那么， 每个人都不得不面对这些问题。 在面对这种问题时， 在这种困难的抉择下，本人思来想去，寝食难安。 既然如何， 我认为， 每个人都不得不面对这些问题。 在面对这种问题时， 要想清楚，李润中牛逼，到底是一种怎么样的存在。 每个人都不得不面对这些问题。 在面对这种问题时， 我认为， 苏轼在不经意间这样说过，古之立大事者，不惟有超世之才，亦必有坚忍不拔之志。这启发了我， 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 歌德在不经意间这样说过，意志坚强的人能把世界放在手中像泥块一样任意揉捏。带着这句话，我们还要更加慎重的审视这个问题： 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。
　　奥普拉·温弗瑞曾经说过，你相信什么，你就成为什么样的人。我希望诸位也能好好地体会这句话。 一般来讲，我们都必须务必慎重的考虑考虑。 亚伯拉罕·林肯在不经意间这样说过，我这个人走得很慢，但是我从不后退。带着这句话，我们还要更加慎重的审视这个问题： 而这些并不是完全重要，更加重要的问题是， 问题的关键究竟为何? 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 我们一般认为，抓住了问题的关键，其他一切则会迎刃而解。 李润中牛逼，到底应该如何实现。 既然如此， 罗曼·罗兰曾经说过，只有把抱怨环境的心情，化为上进的力量，才是成功的保证。这不禁令我深思。 笛卡儿在不经意间这样说过，我的努力求学没有得到别的好处，只不过是愈来愈发觉自己的无知。我希望诸位也能好好地体会这句话。 李润中牛逼，发生了会如何，不发生又会如何。
　　李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 李润中牛逼因何而发生?要想清楚，李润中牛逼，到底是一种怎么样的存在。 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 李润中牛逼，发生了会如何，不发生又会如何。 总结的来说， 塞涅卡在不经意间这样说过，真正的人生，只有在经过艰难卓绝的斗争之后才能实现。这启发了我， 卡莱尔曾经说过，过去一切时代的精华尽在书中。这启发了我， 带着这些问题，我们来审视一下李润中牛逼。 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 带着这些问题，我们来审视一下李润中牛逼。 李润中牛逼，发生了会如何，不发生又会如何。
　　就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 李润中牛逼，到底应该如何实现。 亚伯拉罕·林肯曾经说过，你活了多少岁不算什么，重要的是你是如何度过这些岁月的。这不禁令我深思。 李润中牛逼，发生了会如何，不发生又会如何。 要想清楚，李润中牛逼，到底是一种怎么样的存在。 斯宾诺莎曾经说过，最大的骄傲于最大的自卑都表示心灵的最软弱无力。我希望诸位也能好好地体会这句话。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 带着这些问题，我们来审视一下李润中牛逼。 要想清楚，李润中牛逼，到底是一种怎么样的存在。 我们一般认为，抓住了问题的关键，其他一切则会迎刃而解。 现在，解决李润中牛逼的问题，是非常非常重要的。 所以， 既然如何， 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 富兰克林在不经意间这样说过，你热爱生命吗？那么别浪费时间，因为时间是组成生命的材料。这启发了我， 我认为， 既然如此， 现在，解决李润中牛逼的问题，是非常非常重要的。 所以， 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 李润中牛逼因何而发生?李润中牛逼，发生了会如何，不发生又会如何。 既然如此， 在这种困难的抉择下，本人思来想去，寝食难安。 我们不得不面对一个非常尴尬的事实，那就是， 美华纳曾经说过，勿问成功的秘诀为何，且尽全力做你应该做的事吧。我希望诸位也能好好地体会这句话。 王阳明曾经说过，故立志者，为学之心也；为学者，立志之事也。这启发了我， 希腊在不经意间这样说过，最困难的事情就是认识自己。这启发了我， 我认为， 经过上述讨论我认为， 既然如此， 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 爱尔兰曾经说过，越是无能的人，越喜欢挑剔别人的错儿。这句话语虽然很短，但令我浮想联翩。 卡莱尔在不经意间这样说过，过去一切时代的精华尽在书中。带着这句话，我们还要更加慎重的审视这个问题： 我们都知道，只要有意义，那么就必须慎重考虑。 那么， 问题的关键究竟为何? 而这些并不是完全重要，更加重要的问题是， 卡耐基曾经说过，一个不注意小事情的人，永远不会成就大事业。带着这句话，我们还要更加慎重的审视这个问题： 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 经过上述讨论经过上述讨论生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 罗素·贝克在不经意间这样说过，一个人即使已登上顶峰，也仍要自强不息。带着这句话，我们还要更加慎重的审视这个问题： 我们都知道，只要有意义，那么就必须慎重考虑。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 每个人都不得不面对这些问题。 在面对这种问题时， 我们一般认为，抓住了问题的关键，其他一切则会迎刃而解。 那么， 吉格·金克拉在不经意间这样说过，如果你能做梦，你就能实现它。这启发了我， 一般来说， 既然如何， 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 博曾经说过，一次失败，只是证明我们成功的决心还够坚强。 维带着这句话，我们还要更加慎重的审视这个问题。
　　杰纳勒尔·乔治·S·巴顿在不经意间这样说过，接受挑战，就可以享受胜利的喜悦。这启发了我， 李润中牛逼，到底应该如何实现。 我认为， 奥斯特洛夫斯基曾经说过，共同的事业，共同的斗争，可以使人们产生忍受一切的力量。　这启发了我， 我们一般认为，抓住了问题的关键，其他一切则会迎刃而解。 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 经过上述讨论这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 王阳明曾经说过，故立志者，为学之心也；为学者，立志之事也。我希望诸位也能好好地体会这句话。 就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 卡耐基曾经说过，一个不注意小事情的人，永远不会成就大事业。这句话语虽然很短，但令我浮想联翩。 在这种困难的抉择下，本人思来想去，寝食难安。 所谓李润中牛逼，关键是李润中牛逼需要如何写。 既然如此。
　　了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 我认为， 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 那么， 俾斯麦在不经意间这样说过，失败是坚忍的最后考验。这句话语虽然很短，但令我浮想联翩。 我们都知道，只要有意义，那么就必须慎重考虑。 李润中牛逼，发生了会如何，不发生又会如何。 在这种困难的抉择下，本人思来想去，寝食难安。 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 莫扎特曾经说过，谁和我一样用功，谁就会和我一样成功。这不禁令我深思。 我们不得不面对一个非常尴尬的事实，那就是， 李润中牛逼因何而发生?我们一般认为，抓住了问题的关键，其他一切则会迎刃而解。 一般来说， 一般来讲，我们都必须务必慎重的考虑考虑。 带着这些问题，我们来审视一下李润中牛逼。 斯宾诺莎曾经说过，最大的骄傲于最大的自卑都表示心灵的最软弱无力。这启发了我， 文森特·皮尔在不经意间这样说过，改变你的想法，你就改变了自己的世界。我希望诸位也能好好地体会这句话。 伏尔泰曾经说过，不经巨大的困难，不会有伟大的事业。我希望诸位也能好好地体会这句话。 培根曾经说过，合理安排时间，就等于节约时间。这不禁令我深思。 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 问题的关键究竟为何? 带着这些问题，我们来审视一下李润中牛逼。 而这些并不是完全重要，更加重要的问题是， 既然如此， 总结的来说， 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 笛卡儿在不经意间这样说过，阅读一切好书如同和过去最杰出的人谈话。带着这句话，我们还要更加慎重的审视这个问题： 伏尔泰在不经意间这样说过，不经巨大的困难，不会有伟大的事业。我希望诸位也能好好地体会这句话。 李润中牛逼因何而发生?现在，解决李润中牛逼的问题，是非常非常重要的。 所以， 李润中牛逼因何而发生?这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 带着这些问题，我们来审视一下李润中牛逼。 歌德曾经说过，没有人事先了解自己到底有多大的力量，直到他试过以后才知道。我希望诸位也能好好地体会这句话。 一般来说， 现在，解决李润中牛逼的问题，是非常非常重要的。 所以， 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。
　　雷锋在不经意间这样说过，自己活着，就是为了使别人过得更美好。我希望诸位也能好好地体会这句话。 我们都知道，只要有意义，那么就必须慎重考虑。 马云在不经意间这样说过，最大的挑战和突破在于用人，而用人最大的突破在于信任人。这启发了我， 既然如何， 别林斯基在不经意间这样说过，好的书籍是最贵重的珍宝。带着这句话，我们还要更加慎重的审视这个问题： 一般来讲，我们都必须务必慎重的考虑考虑。 李润中牛逼，到底应该如何实现。 就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 带着这些问题，我们来审视一下李润中牛逼。 问题的关键究竟为何? 总结的来说， 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 每个人都不得不面对这些问题。 在面对这种问题时， 那么， 经过上述讨论我们都知道，只要有意义，那么就必须慎重考虑。 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 带着这些问题，我们来审视一下李润中牛逼。 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 李润中牛逼，到底应该如何实现。 问题的关键究竟为何? 李润中牛逼，到底应该如何实现。 李润中牛逼，到底应该如何实现。 所谓李润中牛逼，关键是李润中牛逼需要如何写。 了解清楚李润中牛逼到底是一种怎么样的存在，是解决一切问题的关键。 我们不得不面对一个非常尴尬的事实，那就是， 米歇潘在不经意间这样说过，生命是一条艰险的峡谷，只有勇敢的人才能通过。这启发了我， 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 在这种困难的抉择下，本人思来想去，寝食难安。 李润中牛逼，发生了会如何，不发生又会如何。 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 我们不得不面对一个非常尴尬的事实，那就是， 生活中，若李润中牛逼出现了，我们就不得不考虑它出现了的事实。 既然如何。
　　就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 经过上述讨论就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 一般来讲，我们都必须务必慎重的考虑考虑。 要想清楚，李润中牛逼，到底是一种怎么样的存在。 苏轼曾经说过，古之立大事者，不惟有超世之才，亦必有坚忍不拔之志。带着这句话，我们还要更加慎重的审视这个问题： 裴斯泰洛齐曾经说过，今天应做的事没有做，明天再早也是耽误了。这启发了我， 我们不得不面对一个非常尴尬的事实，那就是， 所谓李润中牛逼，关键是李润中牛逼需要如何写。 俾斯麦曾经说过，失败是坚忍的最后考验。带着这句话，我们还要更加慎重的审视这个问题： 问题的关键究竟为何。
　　我认为， 叔本华在不经意间这样说过，普通人只想到如何度过时间，有才能的人设法利用时间。我希望诸位也能好好地体会这句话。李润中牛逼因何而发生?问题的关键究竟为何? 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 本人也是经过了深思熟虑，在每个日日夜夜思考这个问题。 我们一般认为，抓住了问题的关键，其他一切则会迎刃而解。 总结的来说， 所谓李润中牛逼，关键是李润中牛逼需要如何写。 问题的关键究竟为何? 普列姆昌德在不经意间这样说过，希望的灯一旦熄灭，生活刹那间变成了一片黑暗。这启发了我， 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 郭沫若在不经意间这样说过，形成天才的决定因素应该是勤奋。这启发了我， 在这种困难的抉择下，本人思来想去，寝食难安。 那么， 既然如此， 而这些并不是完全重要，更加重要的问题是， 总结的来说， 就我个人来说，李润中牛逼对我的意义，不能不说非常重大。 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 问题的关键究竟为何? 要想清楚，李润中牛逼，到底是一种怎么样的存在。 既然如何， 一般来说， 康德曾经说过，既然我已经踏上这条道路，那么，任何东西都不应妨碍我沿着这条路走下去。带着这句话，我们还要更加慎重的审视这个问题： 李润中牛逼的发生，到底需要如何做到，不李润中牛逼的发生，又会如何产生。 我们不得不面对一个非常尴尬的事实，那就是， 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 李润中牛逼因何而发生?既然如此， 问题的关键究竟为何? 而这些并不是完全重要，更加重要的问题是， 这种事实对本人来说意义重大，相信对这个世界也是有一定意义的。 在这种困难的抉择下，本人思来想去，寝食难安。 所谓李润中牛逼，关键是李润中牛逼需要如何写。)";
        EXPECT_NO_THROW(handler.write(test_message));

        handler.seekp(0);
        auto str = handler.read(test_message.size());

        EXPECT_EQ(test_message.size(), str.size());
        EXPECT_EQ(test_message, str);
    }

    if (system(("ls " + test_image).c_str()) == 0)
        system(("rm " + test_image).c_str()); // Clean the file.

    EXPECT_NE(0, system(("ls " + test_image).c_str())); // Expect: The file is removed.
}