#include <errno.h>
#include "rebuild.h"

int needs_rebuild(Raiz_CStrings *source_paths, char **argv);
void rebuild(char **argv);
void run_new_version(char **argv);
void regen_sources(Raiz_CStrings *source_paths);
static inline bool is_c_file(char *path);
static inline bool is_not_source_dot_c(char *path);
void add_source(Raiz_StringBuilder *source_builder, char *source_path);

void
raiz_rebuild_urself(char **argv)
{
  Raiz_CStrings source_paths = raiz_dir_get_relative_file_paths("./source");
  int ret = needs_rebuild(&source_paths, argv);
  if (ret >= 1)
  {
    regen_sources(&source_paths);
    rebuild(argv);
  }
  else if (ret < 0)
  {
    RAIZ_LOG(RAIZ_LOG_NONE, "failed to rebuild, running the old version.\n");
  }
}

int
needs_rebuild(Raiz_CStrings *source_paths, char **argv)
{
  //@todo find a way to check whenever a file in source/ has changed
  // RAIZ_TODO("implement needs_rebuild()\n");

  /* I could get all paths inside source/ folder then try to do something
   * to iterate those paths simultaneously, checking if the last modification 
   * time of each source or header file is greater than the binary's time... */
  
  errno = 0;
  int binary_time = raiz_file_get_last_mod_time(argv[0]);

  if (errno != 0)
  {
    RAIZ_LOG(RAIZ_LOG_ERROR,
        "couldn't get last modification time for file '%s'\n"
        "Be sure that it's relative to the root of Raiz compiler folder\n",
        argv[0]);
    return -1;
  }

  RAIZ_LOG(RAIZ_LOG_INFO, "%s time: %d\n", argv[0], binary_time);

  // if did pass the check above, we don't need to check for paths anymore
  for (size_t i = 0; i < source_paths->count; i++)
  {
    char *file_path = source_paths->items[i];
    int file_time = raiz_file_get_last_mod_time(file_path);

    RAIZ_LOG(RAIZ_LOG_INFO, "%s time: %d\n", file_path, file_time);

    if (file_time > binary_time) return 1;
  }

  return 0;
}

void
rebuild(char **argv)
{
  pid_t clang_pid = fork();

  if (clang_pid == 0)
  {
    RAIZ_LOG(RAIZ_LOG_INFO,
        "running command 'clang -o ./raiz -I./source/ ./source/main.c'\n");
    execlp("clang", // command, should be in $PATH, then the `argv`:
        "clang", "-o", "./raiz", "-I./source/", "./source/main.c", NULL);
  }

  int clang_status = 0;
  (void) waitpid(clang_pid, &clang_status, WNOHANG);

  if (WIFEXITED(clang_status))
  {
    RAIZ_LOG(RAIZ_LOG_INFO,
        "compiler command exited with code %d\n", WEXITSTATUS(clang_status));
    if (WEXITSTATUS(clang_status) != 0)
    {
      RAIZ_LOG(RAIZ_LOG_ERROR,
          "compiler command failed, running the old version\n");
      return;
    }

    run_new_version(argv);
  }
  else if (WIFSIGNALED(clang_status))
  {
    RAIZ_LOG(RAIZ_LOG_INFO,
        "compiler command terminated with signal %d\n", WTERMSIG(clang_status));
  }

  /* Rebuilding should be easy, check if it's needed, then do.
   * But it's actually not like that. We have to take care when we run the
   * Raiz binary after building successfully, else, it'll just rebuild it's
   * self indefinitelly. Thus, may blow up the RAM. Just kidding, it would
   * take a very long time... But is not impossible. */
}

void
run_new_version(char **argv)
{
  pid_t raiz_pid = fork();

  if (raiz_pid == 0)
  {
    execv(argv[0], argv);
  }

  int raiz_status = 0;
  (void) waitpid(raiz_pid, &raiz_status, 0);

  if (WIFEXITED(raiz_status))
  {
    RAIZ_LOG(RAIZ_LOG_INFO, "exited with code %d\n", WEXITSTATUS(raiz_status));
  }
  else if (WIFSIGNALED(raiz_status))
  {
    RAIZ_LOG(RAIZ_LOG_INFO,
        "terminated with signal %d\n", WTERMSIG(raiz_status));
  }
}

void
regen_sources(Raiz_CStrings *source_paths)
{
  FILE *source_dot_c_p = fopen("./source/sources.c", "w+");

  if (source_dot_c_p == NULL)
  {
    RAIZ_LOG(RAIZ_LOG_ERROR,
        "failed to open sources.c: %s\n", strerror(errno));
    return;
  }

  Raiz_StringBuilder source_dot_c_builder = {0};
  for (size_t i = 0; i < source_paths->count; i++)
  {
    char *path = source_paths->items[i];
    if (is_c_file(path) && is_not_source_dot_c(path))
    {
      add_source(&source_dot_c_builder, path);
    }
  }

  size_t written_bytes = fwrite(
      source_dot_c_builder.items,
      sizeof(char), source_dot_c_builder.count,
      source_dot_c_p);

  if (written_bytes < source_dot_c_builder.count)
  {
    RAIZ_LOG(RAIZ_LOG_ERROR, "could not write all bytes to sources.c");
  }

  free(source_dot_c_builder.items);
  fclose(source_dot_c_p);
}

void
add_source(
    Raiz_StringBuilder *sources_builder,
    char *source_path)
{
  Raiz_StringBuilder file_content = raiz_file_read(source_path);
  Raiz_String string_cast = {
    .items = file_content.items,
    .count = file_content.count
  };
  raiz_strb_append(sources_builder, &string_cast);
  free(file_content.items);
}

static inline bool
is_c_file(char *path)
{
  return path[strlen(path) - 1] == 'c';
}

static inline bool
is_not_source_dot_c(char *path)
{
  return strcmp(path, "./source/sources.c") != 0;
}
