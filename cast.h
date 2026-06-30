#ifndef RAIZ_CAST_H
#define RAIZ_CAST_H

int size_t_int(size_t number, int fallback) {
  if (number > INT_MAX) {
    fprintf(stderr,
            "number %zu exceeds maximum integer limit, falling back to %d\n",
            number, fallback);
    return fallback;
  }

  return (int)number;
}

#endif // RAIZ_CAST_H
