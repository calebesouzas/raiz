#ifndef RAIZ_DEBUG_H
#define RAIZ_DEBUG_H

#ifndef RAIZ_LOG_FILE
#define RAIZ_LOG_FILE stderr
#endif

#define RAIZ_DEBUG_MODE
#if defined(RAIZ_DEBUG_MODE) && !defined(RAIZ_DISABLE_LOGS)
#define RZ_LOG(...)\
  do\
  {\
    fprintf(RAIZ_LOG_FILE, "raiz(info): ");\
    fprintf(RAIZ_LOG_FILE, __VA_ARGS__);\
  } while (0)
#else
#define RZ_LOG(...)
#endif // DEBUG_MODE and !DISABLE_LOGS

#define RZ_PANIC(...)\
  do\
  {\
    fprintf(RAIZ_LOG_FILE, "raiz: panicked at %s:%d\n", __FILE__, __LINE__);\
    fprintf(RAIZ_LOG_FILE, __VA_ARGS__);\
    abort();\
  } while (0)

#define RZ_TODO(message) RZ_PANIC("TODO at %s", (message))

#define RZ_UNREACHABLE(message)\
  RZ_PANIC("reached unreachable code at %s", (message))

#define RZ_ASSERT_EQ(left, right)\
  do\
  {\
    if ((left) != (right))\
    {\
      RZ_PANIC("assertion failed\n%s != %s\n", #left, #right);\
    }\
  } while (0)

#endif // RAIZ_DEBUG_H
