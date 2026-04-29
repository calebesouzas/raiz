#pragma once

typedef enum
{
  RAIZ_LOG_NONE,
  RAIZ_LOG_INFO,
  RAIZ_LOG_NOTE,
  RAIZ_LOG_HELP,
  RAIZ_LOG_WARNING,
  RAIZ_LOG_ERROR,
} Raiz_LogLevel;

#ifndef RAIZ_DISABLE_LOGS
#define RAIZ_LOG(level, ...)\
  do\
  {\
    fprintf(stderr, "raiz: ");\
    switch ((level))\
    {\
    case RAIZ_LOG_NONE: break;\
    case RAIZ_LOG_INFO:    fprintf(stderr, "info: ");    break;\
    case RAIZ_LOG_NOTE:    fprintf(stderr, "note: ");    break;\
    case RAIZ_LOG_HELP:    fprintf(stderr, "help: ");    break;\
    case RAIZ_LOG_WARNING: fprintf(stderr, "warning: "); break;\
    case RAIZ_LOG_ERROR:   fprintf(stderr, "error: ");   break;\
    }\
    fprintf(stderr, __VA_ARGS__);\
  } while (0)
#else
#define RAIZ_LOG
#endif
