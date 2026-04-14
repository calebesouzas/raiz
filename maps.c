#ifndef RAIZ_MAPS_SOURCE
#define RAIZ_MAPS_SOURCE

#include "common.h"
#include "maps.h"
// #include "values.h"

unsigned int rz_hash_get(Rz_String key)
{
  unsigned long hash = 5381;
  char c;
  for (size_t i = 0; (c = *key.data++) && i < key.size; ++i)
    // same as 'hash * 33 + c'
    hash = ((hash << 5) + hash) + c;

  return hash % RAIZ_MAP_SIZE;
}

Rz_StringValuePair *rz_string_value_pair_new(Rz_String key, Rz_Value value)
{
  Rz_StringValuePair *node = malloc(sizeof(*node));

  if (node)
  {
    node->key = rz_string_clone(&key);
    node->value = value;
    node->next = NULL;
  }

  return node;
}

void rz_scope_insert(Rz_StringValueMap *map, Rz_String key, Rz_Value value)
{
  unsigned int index = rz_hash_get(key);

  Rz_StringValuePair *node = rz_string_value_pair_new(key, value);

  if (map->buckets[index] == NULL)
  {
    map->buckets[index] = node;
  }
  else
  {
    Rz_StringValuePair *temp = map->buckets[index];
    while (temp->next != NULL
        && strncmp(temp->key.data, key.data, temp->key.size)) // != 0
    {
      temp = temp->next;
    }
    // don't know if using just pointers work...
    (*temp) = *node;
  }
}

Rz_Value *rz_scope_get(Rz_StringValueMap *map, Rz_String key)
{
  unsigned int index = rz_hash_get(key);

  Rz_StringValuePair *cursor = map->buckets[index];
  if (!cursor) return NULL;

  while (cursor != NULL)
  {
    if (strncmp(cursor->key.data, key.data, key.size) == 0)
    {
      return &cursor->value;
    }
    else cursor = cursor->next;
  }

  return NULL;
}

#endif // RAIZ_MAPS_SOURCE
