#pragma once

typedef struct
{
  Raiz_String *items;
  size_t count, capacity;
} Raiz_Strings;

typedef struct
{
  char **items;
  size_t count, capacity;
} Raiz_CStrings;
