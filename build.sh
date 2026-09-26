set -xe

clear
rm -rf build
mkdir -p ./build
clang -o ./build/raiz -Iinclude source/main.c -ggdb
