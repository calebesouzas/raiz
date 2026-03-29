#ifndef RAIZ_MAPS_SOURCE
#define RAIZ_MAPS_SOURCE

#include "common.h"
#include "maps.h"
// #include "values.h"

unsigned int rz_hash_get(const char *key)
{
  unsigned long hash = 5381;
  char c;
  while ((c = *key++))
    // same as 'hash * 33 + c'
    hash = ((hash << 5) + hash) + c;

  return hash % RAIZ_MAP_SIZE;
}

Rz_StringIntPair *rz_string_int_pair_new(const char *key, int *value)
{
  Rz_StringIntPair *node = malloc(sizeof(*node));

  if (node)
  {
    node->key = strdup(key);
    node->value = value;
    node->next = NULL;
  }

  return node;
}

void rz_scope_insert(Rz_StringIntMap *map, const char *key, int *value)
{
  unsigned int index = rz_hash_get(key);

  Rz_StringIntPair *node = rz_string_int_pair_new(key, value);

  if (map->buckets[index] == NULL)
  {
    map->buckets[index] = node;
  }
  else
  {
    Rz_StringIntPair *temp = map->buckets[index];
    size_t len = strlen(key);
    while (temp->next != NULL && strncmp(temp->key, key, len) != 0)
    {
      temp = temp->next;
    }
    // don't know if using just pointers work...
    (*temp) = *node;
  }
}

int *rz_scope_get(Rz_StringIntMap *map, const char *key)
{
  unsigned int index = rz_hash_get(key);

  Rz_StringIntPair *cursor = map->buckets[index];
  if (!cursor) return NULL;

  size_t len = strlen(key);

  while (cursor != NULL)
  {
    if (strncmp(cursor->key, key, len) == 0)
    {
      return cursor->value;
    }
    else cursor = cursor->next;
  }

  return NULL;
}

#endif // RAIZ_MAPS_SOURCE
