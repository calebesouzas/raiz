#ifndef RAIZ_DEBUG_H
#define RAIZ_DEBUG_H

#ifndef RAIZ_LOG_FILE
#define RAIZ_LOG_FILE stderr
#endif

#define DEBUG_MODE
#if defined(DEBUG_MODE) && !defined(DISABLE_LOGS)
#define LOG(...)\
  do\
  {\
    fprintf(RAIZ_LOG_FILE, "raiz(info): ");\
    fprintf(RAIZ_LOG_FILE, __VA_ARGS__);\
  } while (0)
#else
#define LOG(...)
#endif // DEBUG_MODE and !DISABLE_LOGS

#define PANIC(...)\
  do\
  {\
    fprintf(RAIZ_LOG_FILE, "raiz: panicked at %s:%d\n", __FILE__, __LINE__);\
    fprintf(RAIZ_LOG_FILE, __VA_ARGS__);\
    abort();\
  } while (0)

#define TODO(message) PANIC("TODO at %s", (message))

#define UNREACHABLE(message) PANIC("reached unreachable code at %s", (message))

#define ASSERT_EQ(left, right)\
  do\
  {\
    if ((left) != (right))\
    {\
      PANIC("assertion failed\n%s != %s\n", #left, #right);\
    }\
  } while (0)

#endif // RAIZ_DEBUG_H
