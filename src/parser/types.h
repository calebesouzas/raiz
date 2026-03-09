#ifndef RAIZ_PARSER_TYPES_H
#define RAIZ_PARSER_TYPES_H

typedef enum {
  RAIZ_TYPE_SINGLE,   // val x int = 15!
  RAIZ_TYPE_TUPLE,    // val pair (string, int) = ("Calebe", 16)!
  RAIZ_TYPE_ARRAY,    // var scores [float] = [7.5, 8, 6.7, 9.8, 10]!
  RAIZ_TYPE_PACK,     // val rooms [int, 10] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]!
  RAIZ_TYPE_MULTIPLE, // var locales Dict[string, string]!
  RAIZ_TYPE_NESTED,   // var save Option[Dict[string, any]] = try_get_save()!
} TypeKind;

typedef struct Type Type;
struct Type {
  TypeKind kind;
  Type* items; // dynamic array
};

#endif // RAIZ_PARSER_TYPES_H
