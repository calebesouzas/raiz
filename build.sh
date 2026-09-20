set -xe

clear
mkdir -p ./build
clang -o ./build/raiz -Iinclude source/main.c -ggdb
