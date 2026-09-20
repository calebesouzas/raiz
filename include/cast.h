#ifndef RAIZ_CAST_H
#define RAIZ_CAST_H

int size_t_int(size_t number) {
  if (number > INT_MAX) {
    fprintf(stderr, "number %zu exceeds maximum integer limit\n", number);
    abort();
  }

  return (int)number;
}

#endif // RAIZ_CAST_H
