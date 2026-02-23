#include "rz_strings.h"

String test_char_p_to_String(char *const s) {
  uint32_t len;
  GET_STRING_LEN(len, s);

  return (String) {
    .ptr = s,
    .len = len
  };
}
