#!/bin/bash
set -e

format_dir() {
    find $1 -name '*.cpp' -exec clang-format -style=file -i {} \;
    find $1 -name '*.hpp' -exec clang-format -style=file -i {} \;
}

cd $(dirname $0)
format_dir ./JRFS
format_dir ./tests