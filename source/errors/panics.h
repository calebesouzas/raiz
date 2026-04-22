#pragma once

/**
 * @doc!
 * @name(header) errors/panics.h
 * @tags helper, errors, debugging
 */

/**
 * @doc!
 * @name(macro) PANIC
 * @tags helper, errors, panics
 * @param `...` format string and arguments passed into `fprintf`
 * @desc crashes the program after printing information about where the panic
 * was called and the custom message from variadic argument list.
 */
#define RAIZ_PANIC(...)\
  do {\
    const char *file = __FILE__;\
    const char *func = __func__;\
    const int line = __LINE__;\
    fprintf(stderr, "raiz: panicked at %s:%d:%s():\n", file, line, func);\
    fprintf(stderr, __VA_ARGS__);\
    abort();\
  } while (0)

/**
 * @doc!
 * @name(macro) TODO
 * @tags helper, panics
 * @param `message` custom simple message about what has to be done
 * @desc calls [@PANIC] with "TODO: <message>\n"
 */
#define RAIZ_TODO(message) RAIZ_PANIC("TODO: %s\n", (message))
