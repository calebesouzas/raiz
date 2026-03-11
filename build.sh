rm -rf build/
mkdir -p build/
clang -o build/raiz src/main.c -Wextra -Wpedantic -Werror -Wall -ggdb
