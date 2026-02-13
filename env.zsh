function a
{
  if [[ $1 == "core" ]]; then
    nvim ./crates/raiz-core/src/
  elif [[ $1 == "parser" ]]; then
    nvim ./crates/raiz-parser/src/
  fi
}
