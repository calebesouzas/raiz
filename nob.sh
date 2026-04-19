#!/bin/sh

# I'm not a Shell programmer...

set -xe

if [ ! -e "./tools/nob" ]; then
  cc -x c -o ./tools/nob ./tools/nob.c
fi

rm -rf ./build/

mkdir ./build/

if [ $1 = "test" ]; then
  rm -rf ./build/tests
  mkdir ./build/tests
  clang -o build/tests/dir_walk "tests/dir_walk.c" "source/filesystem/dirs.c" \
    "-I./source/" -ggdb
  ./build/tests/dir_walk
else
  ./tools/nob $@
fi
