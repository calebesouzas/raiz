#ifndef RAIZ_ITERATIONS_FOREACH_H
#define RAIZ_ITERATIONS_FOREACH_H

#define foreach(item, indexer, array, code)\
  for (int indexer = 0; array[indexer]; indexer++) {\
    item = array[indexer];\
    do code while (0);\
  }

#endif /* ifndef RAIZ_ITERATIONS_FOREACH_H */
