#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h> // strerror()
#include "raiz_core.h"
#include "raiz_errors.h" // RAIZ_PANIC() and RAIZ_PRINT_ERR()
#include <errno.h> // errno

/* Function `main`
 * Params:
 *   `int argc` -> the argument count
 *   `char *argv[]` -> the arguments passed to our program by the OS
 * Returns `int` -> the exit code (non-zero means EXIT_FAILURE)
 * Desc: the entry point of the program...
 * */
int
main(int argc, char *argv[]) {
  if (argc == 1) { // only the program name
    printf("Raiz - v%s - Creative Commons Zero\n", RAIZ_VERSION);
    return 0;
  }

  FILE *p_file = NULL;
  int i;
  for (i = 0; i < argc; ++i) {
    char *arg = argv[i];

    switch (arg[0]) {
    case '-': // potentialy an option (if not only '--')
      switch (arg[1]) {
      case 'f': // '-f', shortcut for '--file'
        if (i + 1 >= argc) { // if we're in the last argument
          RAIZ_PANIC("raiz: expected file path after '-f'\n");
        }

        p_file = fopen(argv[i + 1], "r");
        if (p_file == NULL) {
          RAIZ_PANIC( // using less than 80 columns in a line is hard...
            "raiz: failed to open file \"%s\":\n%s.\n",
            argv[i + 1], strerror(errno)
          );
        }

        break; // switch (arg[1]) case 'f'
      }

      break; // switch (arg[0]) case '-'

    default:
      break;
    }
  }

  if (p_file != NULL) {
    fclose(p_file);
  }

  return 0;
}
