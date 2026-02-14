#include "str.h"

int streq(const char *left, const char *right) {
  int eq = 0;
  for (int i = 0; left[i] != '\0' || right[i] != '\0'; i++) {
    if (left[i] != right[i]) {
      eq = 0;
      break;
    } else {
      eq = 1;
    }
  }
  return eq;
}
