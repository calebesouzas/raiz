#include "str.h"
#include <stdio.h>

typedef struct {
  enum {
    Default,
    F_File,
  } kind;
  char *value;
} Option;

int main(int argc, char *argv[]) {
  char version[] = "0.0.1-beta0";
  if (argc == 1) {
    printf("Raiz (in C) - v%s\n", version);
  } else {
    Option options[20];
    int added = 0;
    for (int i = 1; i < argc; i++) {
      char *arg = argv[i];
      if (streq(arg, "-f") || streq(arg, "--file")) {
        options[added] = (Option){.kind = F_File, .value = argv[i + 1]};
        added++;
      } else {
        printf("error: invalid argument: '%s'\n", arg);
        return 1;
      }
    }
  }

  return 0;
}
