set -xe

clear
mkdir -p ./build
clang -o ./build/raiz raiz.c -ggdb
