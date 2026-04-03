#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

struct BuildFlags { bool run; bool strict; };
struct BuildFlags build_flags = {0};

bool
build (void) { // i kinda like this formatting
  if (build_flags.strict)
  {
    Cmd cargo_clippy = {
      .items = {"cargo", "clippy", NULL},
      .count = 2,
      .capacity = 2
    };

    // Ayo, optional parameters bro! Look at it!
    if (!cmd_run(&cargo_clippy, .dont_reset = true)) return false;
  }

  if (build_flags.run)
  {
    Cmd cargo_run = {
      .items = {"cargo", "run", NULL},
      .count = 2,
      .capacity = 2
    };

    if (!cmd_run(&cargo_run, .dont_reset = true)) return false;
  }
  else
  {
    Cmd cargo_build = {
      .items = {"cargo", "build", NULL},
      .count = 2,
      .capacity = 2
    };

    if (!cmd_run(&cargo_build, .dont_reset = true)) return false;
  }

  return true;
}

int
main (int argc, char **argv) {
  GO_REBUILD_URSELF(argc, argv);

  for (int i = 1; i < argc; i++)
  { // can we omit these curly braces?
    char *arg = argv[i];
    if (strncmp(arg, "run", 3) == 0) build_flags.run = true;
    else if (strncmp(arg, "--strict", 8) == 0) build_flags.strict = true;
    else fprintf(stderr, "nob.c: unknown argument: %s\n", arg);
  }

  if (!build()) return -1; // for me it turns into 255, don't know why.
                           // Looks like the OS uses 'uint8_t' for exit codes...

  return 0;
}
