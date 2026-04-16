if [ ! -e "./tools/nob" ]; then
  cc -x c -o ./tools/nob ./tools/nob.c
fi

rm -rf ./build/

mkdir ./build/

./tools/nob $@
