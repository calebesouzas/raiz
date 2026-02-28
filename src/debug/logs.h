#ifndef RAIZ_DEBUG_LOGS_H
#define RAIZ_DEBUG_LOGS_H

#ifdef RAIZ_DEBUG
#define RAIZ_LOG(...)\
  printf("[RAIZ]: ");\
  printf(__VA_ARGS__);\
  printf("\n")
#else
#define RAIZ_LOG(...) // do nothing
#endif


#endif /* ifndef RAIZ_DEBUG_LOGS_H */
