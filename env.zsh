#!/data/data/com.termux/files/usr/bin/zsh

function compile_db ()
{
  rm -f compile_commands.json
  echo "[" > compile_commands.json
  directory="$PWD"
  first=true
  for file in $(find src -type f -name "*.c")
  do
    if [[ ! first ]]; then
      
      echo "," >> compile_commands.json
    fi
    echo "{\"directory\":\"$directory\","\
      "\"file\":\"$file\","\
      "\"command\":\"gcc -c $file -o build/${file/src/obj}.o" \
      "-Werror -Wall -Wpedantic -Iinclude -g\"}"\
      >> compile_commands.json
    first=false
  done
  echo "]" >> compile_commands.json
}

function start ()
{
  echo "Setting up Raiz in C dev env..."
  alias raiz="$HOME/dev/craiz/build/bin/raiz"
}

function build ()
{
  # echo "Generating dirs at 'build/obj'..."
  # for DIR in $(find src -type d -wholename "src/*")
  # do
  #   NEW_DIR=${DIR/src/obj}
  #   mkdir -p ./build/$NEW_DIR
  # done
  echo "Removing old build folder..."
  rm -rf build
  mkdir -p build build/obj build/bin
  echo "Compiling files..."
  for FILE in $(find src -type f -name "*.c")
  do
    NEW_PATH="build/${FILE/src/obj}.o"
    echo "  Compiling $FILE into $NEW_PATH"
    gcc -c $FILE -o $NEW_PATH \
      -Werror -Wall -Wpedantic -Iinclude -g
  done
  echo "Building final binary..."
  OBJECTS=$(find build/obj -type f -name "*.c.o")
  echo "Objects:"
  for OBJECT in $OBJECTS
  do
    echo "  $OBJECT"
  done
  gcc $OBJECTS -o build/bin/raiz
}
