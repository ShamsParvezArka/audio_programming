#ifndef ARRAY_H
#define ARRAY_H

#define DYN_ARRAY_GROWTH_FACTOR 1.25
#define DYN_ARRAY_INITIAL_SIZE 10

#define _DynArrayHeader_ struct { U64 count; U64 capacity; }

#define dyn_array_grow(arena, da)                               \
  do {                                                          \
    U64 old_capacity = (da).capacity;                           \
    (da).capacity = (da).capacity ?                             \
                    (U64)((da).capacity * DYN_ARRAY_GROWTH_FACTOR) :          \
                    DYN_ARRAY_INITIAL_SIZE;                                   \
    typeof(*(da).items) *mem_block = arena_push_array(arena, typeof(*(da).items), (da).capacity - old_capacity); \
    if (!(da).items)                                                    \
    {                                                                   \
      (da).items = mem_block;                                           \
    }                                                                   \
} while (0);

#define dyn_array_push(arena, da, n)             \
  do {                                          \
    if ((da).count >= (da).capacity)            \
    {                                           \
      dyn_array_grow(arena, da);                   \
    }                                           \
    (da).items[(da).count++] = n;               \
  } while (0);                                  \

#define dyn_array_insert(arena, da, p, n)                        \
  do {                                                          \
    assert(((p) <= (da).count) && "Unable to insert item");     \
    if ((da).count >= (da).capacity)                            \
    {                                                           \
      dyn_array_grow(arena, da);                                   \
    }                                                           \
                                                                \
    for (U64 idx = (da).count; idx > (p); idx--)                \
    {                                                           \
      (da).items[idx] = (da).items[idx - 1];                    \
    }                                                           \
    (da).items[(p)] = (n);                                      \
    (da).count++;                                               \
  } while (0);                                                  \


#define dyn_array_unorderer_insert(arena, da, p, n)              \
  do {                                                          \
    assert(((p) <= (da).count) && "Unable to insert item");     \
    if ((da).count >= (da).capacity)                            \
    {                                                           \
      dyn_array_grow(arena, da);                                   \
    }                                                           \
                                                                \
    (da).items[(da).count++] = (da).items[(p)];                 \
    (da).items[(p)] = (n);                                      \
  } while (0);                                                  \

#define dyn_array_pop(arena, da, p)                      \
  do {                                                  \
    assert(((p) < (da).count) && "Unable to pop item"); \
                                                        \
    for (U64 idx = (p); idx < (da).count - 1; idx++)    \
    {                                                   \
      (da).items[idx] = (da).items[idx + 1];            \
    }                                                   \
    (da).count--;                                       \
  } while (0);                                          \

#define dyn_array_unorderer_pop(arena, da, p)            \
  do {                                                  \
    assert(((p) < (da).count) && "Unable to pop item"); \
    (da).items[(p)] = (da).items[--(da).count];         \
  } while (0);                                          \

// #define dyn_array_free(arena, da)                \
//   do {                                          \
//     free(da.items);                             \
//     da.count = 0;                               \
//     da.capacity = 0;                            \
//   } while (0);                                  \

#endif // ARRAY_H
