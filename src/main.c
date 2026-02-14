#include <stdio.h>

int main(int argc, char *argv[]) {
  char version[] = "0.0.1-beta0";
  if (argc == 1) {
    printf("Raiz (in C) - v%s\n", version);
  }

  return 0;
}
