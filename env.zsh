#!/data/data/com.termux/files/usr/bin/zsh

function echo_sources ()
{
  echo "C files:"
  for cfile in $(find -type f -name "*.c")
  do
    echo $cfile
    cat $cfile
  done
  echo "H files:"
  for hfile in $(find -type f -name "*.h")
  do
    echo $hfile
    cat $hfile
  done
}

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

function debug ()
{
  gdb ./build/bin/raiz
}
