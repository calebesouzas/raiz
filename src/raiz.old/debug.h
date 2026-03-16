#ifndef RAIZ_DEBUG_H
#define RAIZ_DEBUG_H

#ifdef RAIZ_DEBUG
#define RAIZ_LOG(...)\
  do {\
    printf("[RAIZ]: ");\
    printf(__VA_ARGS__);\
    printf("\n");\
  } while (0)

#define RAIZ_STRING_VAR_NAME(var) (#var)
#else
#define RAIZ_LOG(...) // do nothing
#define RAIZ_STRING_VAR_NAME(var)
#endif


#endif // RAIZ_DEBUG_LOGS_H
