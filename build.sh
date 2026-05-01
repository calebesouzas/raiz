set -xe

regen_source ()
{
  for c_file in $(find "./source/" -type f -name "*.c"); do
    if [ $c_file = "./source/main.c" ]; then
      continue
    elif [ $c_file = "./source/sources.c" ]; then
      continue
    fi

    cat $c_file >> "./source/sources.c"
  done
}

clang -o raiz "-I./source/" "source/main.c"
