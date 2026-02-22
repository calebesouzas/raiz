#include <stdio.h>
#include "raiz_core.h"
#include "iterations/foreach_macro.h"

int
main(int argc, char *argv[]) {
  
  if (argc == 1) {
    printf("Raiz - v%s - Creative Commons Zero\n", RAIZ_VERSION);
  }
  else { // argc == 1
    foreach(char *arg, i, argv, {
      if (arg[0] == '-') { // is an option
        switch (arg[0]) {
        case 'f': // to specify file path

          break;
        }
      }
    })
  }

  return 0;
}
