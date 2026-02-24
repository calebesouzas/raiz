#include <stdio.h>
#include <stdlib.h> // exit(), malloc() and free()
#include <string.h> // strerror()
#include "raiz_core.h" // RAIZ_VERSION and RAIZ_FILE_SIZE_LIMIT
#include "raiz_memory.h" // byte type
#include <errno.h> // errno
#include <sys/types.h>

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
  }

  FILE *p_file = NULL;
  byte *source_code_buffer = NULL;

  if (argc > 1) {
    p_file = fopen(argv[1], "r");
    if (p_file == NULL) {
      fprintf(stderr,
        "raiz: failed to open file \"%s\": %s\n",
        argv[1],
        strerror(errno)
      );
      exit(errno);
    }

    // getting the file size (in bytes)
    fseek(p_file, 0, SEEK_END);
    int file_size = ftell(p_file);

    // put the cursor back at the begining
    fseek(p_file, 0, SEEK_SET);

    if (file_size > RAIZ_FILE_SIZE_LIMIT) {
      fprintf(
        stderr,
        "raiz: file '%s' exceeds size limit of %d bytes\n",
        argv[1],
        RAIZ_FILE_SIZE_LIMIT
      );
      exit(-1);
    }

    source_code_buffer = malloc(file_size);
    for (int i = 0; i < file_size; i++) {
      source_code_buffer[i] = fgetc(p_file);
    }
  }

  if (p_file != NULL) {
    fclose(p_file);
    p_file = NULL;
  }



  if (source_code_buffer != NULL) {
    free(source_code_buffer);
    source_code_buffer = NULL;
  }

  return 0;
}
