if [ ! -e "./tools/nob" ]; then
  cc -x c -o ./tools/nob ./tools/nob.c
fi

./tools/nob $@
