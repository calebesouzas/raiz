#ifndef RAIZ_DYNAMIC_ARRAYS_H
#define RAIZ_DYNAMIC_ARRAYS_H

#define da_make(TypeName, TypeItems)\
  typedef struct {\
    size_t len, cap;\
    TypeItems dat;\
  } TypeName
#define DEFAULT_DA_CAP 256
#define da_size(da) (sizeof((da)->dat[0])*(da)->cap)
// you know what? Yep, I just stole it from an "AI" or someone in a forum
#define da_for(item, da)\
  for (size_t i_##item = 0;\
       (i_##item < (da)->len) && ((item) = &(da)->dat[i_##item]);\
       i_##item++)
#define da_add(da, val)\
  do{\
    if ((da)->cap == 0) (da)->cap = DEFAULT_DA_CAP;\
    if ((da)->dat == NULL) (da)->dat = malloc(da_size(da));\
    else if ((da)->len >= (da)->cap) {\
      (da)->cap *= 2;\
      (da)->dat = realloc((da)->dat, da_size(da));\
    }\
    if ((da)->dat != NULL) (da)->dat[(da)->len++] = (val);\
  } while (0)

#endif // RAIZ_DYNAMIC_ARRAYS_H
