#ifndef RAIZ_DEBUG_H
#define RAIZ_DEBUG_H

// NOTE: Adding ANSI escapes (colors and formatting) would be cool
typedef enum {
  RZ_LOG_NONE,
  RZ_LOG_INFO,
  RZ_LOG_HELP,
  RZ_LOG_WARNING,
  RZ_LOG_ERROR,
} Rz_LogLevel;

#define RAIZ_DEBUG_MODE
#if defined(RAIZ_DEBUG_MODE) && !defined(RAIZ_DISABLE_LOGS)
#define RZ_LOG(level, ...)\
  do\
  {\
    char *kind = NULL;\
    switch (level)\
    {\
      case RZ_LOG_NONE:    kind = "";          break;\
      case RZ_LOG_INFO:    kind = "(info)";    break;\
      case RZ_LOG_HELP:    kind = "(help)";    break;\
      case RZ_LOG_WARNING: kind = "(warning)"; break;\
      case RZ_LOG_ERROR:   kind = "(error)";   break;\
    }\
    fprintf(stderr, "raiz%s:\n", kind);\
    fprintf(stderr, __VA_ARGS__);\
  } while (0)
#else
#define RZ_LOG(...)
#endif // RZ_DEBUG_MODE and !RZ_DISABLE_LOGS

#define RZ_PANIC(...)\
  do\
  {\
    fprintf(stderr, "raiz: panicked at %s line %d, function %s()\n",\
        __FILE__, __LINE__, __FUNCTION__);\
    fprintf(stderr, __VA_ARGS__);\
    abort();\
  } while (0)

#define RZ_TODO(message)\
  RZ_PANIC("TODO: %s", (message))

#define RZ_UNREACHABLE(message)\
  RZ_PANIC("reached unreachable code: %s\n", (message))

#define RZ_ASSERT_EQ(left, right)\
  do\
  {\
    if ((left) != (right))\
    {\
      RZ_PANIC("assertion failed\n%s != %s\n", #left, #right);\
    }\
  } while (0)

#endif // RAIZ_DEBUG_H
