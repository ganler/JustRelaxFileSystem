#!/bin/bash
set -e

format_dir_common() {
    find $1 -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -style=file -i {} \;
}

format_dir_darwin() {
    find -E $1 -regex '.*\.\(cpp\|hpp\|cc\|cxx\)' -exec clang-format -style=file -i {} \;
}

cd $(dirname $0)

if [[ "${OSTYPE}" == "darwin"* ]]; then
  format_dir_darwin ./JRFS
else
  format_dir_common ./JRFS
fi