#!/data/data/com.termux/files/usr/bin/zsh

function compile_db ()
{
  echo "\n  Removing old 'compile_commands.json'..."
  rm -f compile_commands.json
  echo "[" > compile_commands.json
  directory="$PWD"
  first=true
  echo "    Files:"
  for file in $(find src -type f -name "*.c")
  do
    if [[ ! first ]]; then
      
      echo "," >> compile_commands.json
    fi
    echo "      $file"
    echo "{\"directory\":\"$directory\","\
      "\"file\":\"$file\","\
      "\"command\":\"gcc -c $file -o build/${file/src/obj}.o" \
      "-Werror -Wall -Wextra -Wpedantic -Iinclude -g\"}"\
      >> compile_commands.json
    first=false
  done
  echo "]" >> compile_commands.json
}

function start ()
{
  echo "Setting up Raiz in C dev env..."
  alias raiz="$HOME/dev/raiz/build/bin/raiz"
}

function build ()
{
  echo "  Removing old build folder..."
  rm -rf build
  mkdir -p build build/obj build/bin
  echo "\n  Compiling files..."
  for FILE in $(find src -type f -name "*.c")
  do
    NEW_PATH="build/${FILE/src/obj}.o"
    echo "    Compiling $FILE into $NEW_PATH"
    gcc -c $FILE -o $NEW_PATH \
      -Werror -Wall -Wextra -Wpedantic -Iinclude -g
  done
  echo "\n  Building final binary..."
  OBJECTS=$(find build/obj -type f -name "*.c.o")
  echo "    Objects:"
  for OBJECT in $(find build/obj/ -type f -name "*.c.o")
  do
    echo "      $OBJECT"
  done
  echo "\n  Built program at build/bin/raiz"
  OBJECTS=$(echo "$OBJECTS" | tr '\n' ' ')
  sh -c "gcc $OBJECTS -o build/bin/raiz"
  compile_db
}
