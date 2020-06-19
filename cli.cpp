#include <chrono>
#include <iostream>
#include <string_view>
#include <thread>

#include "JRFS/util/term_style.hpp"
#include "JRFS/util/utility.hpp"

#include <gflags/gflags.h>

DEFINE_string(mount_path, "", "Path to mount the image.");
DEFINE_bool(create, false, "Whether to create new image.");
DEFINE_int32(block_size, 2048, "Total numbers of blocks in new image.");

namespace jrfs {
class cli {
public:
    int ls();
    int rm(std::string_view dest);
    int mkdir(std::string_view dest);
    int touch(std::string_view dest);
    int cat(std::string_view path);
    int append(std::string_view dest, std::string_view content);
    int echo(std::string_view input);
    int to_jrfs(std::string_view from, std::string_view to);
    int from_jrfs(std::string_view from, std::string_view to);
    int exit();

    void shell()
    {
        std::string input_line;
        user_prompt();
        std::getline(std::cin, input_line);
        auto string_vector = jrfs::utility::split(input_line, ' ');
        if (string_vector.empty())
            return;

        int return_code = [this, &string_vector] {
            try {
                if (string_vector[0] == "ls") {
                    if (string_vector.size() != 1)
                        throw std::logic_error("Invalid use of `ls`. Just type `ls` and all.");
                    return ls();
                } else if (string_vector[0] == "rm") {
                    if (string_vector.size() != 2)
                        throw std::logic_error("Invalid use of `rm`. Run like this: `rm ${FILE_PATH}`");
                    return rm(string_vector[1]);
                } else if (string_vector[0] == "touch") {
                    if (string_vector.size() != 2)
                        throw std::logic_error("Invalid use of `touch`. Run like this: `touch ${FILE_PATH}`");
                    return rm(string_vector[1]);
                } else if (string_vector[0] == "cat") {
                    if (string_vector.size() != 2)
                        throw std::logic_error("Invalid use of `cat`. Run like this: `cat ${FILE_PATH}`");
                    return rm(string_vector[1]);
                } else if (string_vector[0] == "mkdir") {
                    if (string_vector.size() != 2)
                        throw std::logic_error("Invalid use of `mkdir`. Run like this: `mkdir ${FILE_PATH}`");
                    return mkdir(string_vector[1]);
                } else if (string_vector[0] == "echo") {
                    if (string_vector.size() != 2)
                        throw std::logic_error("Invalid use of `echo`. Run like this: `echo ${Whatever U wanna say}`");
                    return echo(string_vector[1]);
                } else if (string_vector[0] == "to_jrfs") {
                    if (string_vector.size() != 3)
                        throw std::logic_error("Invalid use of `to_jrfs`. Run like this: `to_jrfs ${src_local} ${dest_image}`");
                    return to_jrfs(string_vector[1], string_vector[2]);
                } else if (string_vector[0] == "from_jrfs") {
                    if (string_vector.size() != 3)
                        throw std::logic_error("Invalid use of `from_jrfs`. Run like this: `from_jrfs ${src_image} ${dest_image}`");
                    return from_jrfs(string_vector[1], string_vector[2]);
                }else if (string_vector[0] == "append") {
                    if (string_vector.size() != 3)
                        throw std::logic_error("Invalid use of `append`. Run like this: `append ${dest_file} ${string}`");
                    return from_jrfs(string_vector[1], string_vector[2]);
                } else if (string_vector[0] == "exit") {
                    if (string_vector.size() != 1)
                        throw std::logic_error("Invalid use of `exit`. Just type `exit` and all.");
                    return exit();
                } else {
                    throw std::logic_error("No command named like: " + string_vector[0]);
                }
            } catch (const std::exception& err) {
                error_report(err.what());
                return -1;
            }
        }();

        report_return_code(return_code);
    }

private:
    void user_prompt()
    {
        auto style = pt::CYAN.style({ pt::Style::UNDERLINE, pt::Style::BOLD, pt::Style::REVERSE });
        std::cout << style << ":: JRFS: USER   >" << pt::CLEAN << '\t';
    }

    void error_report(std::string_view message)
    {
        auto style = pt::RED.style({ pt::Style::UNDERLINE, pt::Style::BOLD, pt::Style::REVERSE  });
        std::cout << style << ":: JRFS: ERROR  >" << pt::CLEAN << '\t';
        std::cout << pt::RED.style(pt::Style::UNDERLINE) << message << pt::CLEAN << std::endl;
    }

    void report_return_code(int code)
    {
        auto style = pt::WHITE.style({ pt::Style::UNDERLINE, pt::Style::BOLD, pt::Style::REVERSE  });
        std::cout << style << ":: JRFS: RETURN >" << pt::CLEAN << '\t' << code << std::endl;
    }
};
}

struct alignas(128) a {
    int x;
};

int main(int argc, char* argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    jrfs::cli command_line;
    while (true) {
        command_line.shell();
    }
}

namespace jrfs {
int cli::echo(std::string_view input)
{
    std::cout << input << '\n';
    return 0;
}

int cli::exit()
{
    for (int i = 0; i < pt::get_term_length(); ++i) {
        std::cout << pt::GREEN.style(pt::Style::BOLD) << '>' << std::flush;
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5ms);
    }
    std::cout << std::endl;
    std::cout << "Thank U for using JRFS!!!\n";
    std::exit(0);
}

int cli::from_jrfs(std::string_view from, std::string_view to)
{
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}

int cli::mkdir(std::string_view dest)
{
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}

int cli::rm(std::string_view dest)
{
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}

int cli::to_jrfs(std::string_view from, std::string_view to)
{
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}

int cli::ls()
{
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}

int cli::touch(std::string_view dest) {
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}
int cli::cat(std::string_view path) {
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}

int cli::append(std::string_view dest, std::string_view content) {
    std::cout << __PRETTY_FUNCTION__ << ": Not Implemented.\n";
    return 0;
}

}