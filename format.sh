find -E "$(dirname $0)/JRFS" -regex '.*\.(hpp|cpp)' -exec clang-format -style=file -i {} \;
clang-format -i -style=file *.cpp