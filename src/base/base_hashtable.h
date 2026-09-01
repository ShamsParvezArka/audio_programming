#ifndef BASE_HASHTABLE_H
#define BASE_HASHTABLE_H

typedef struct Pair Pair;
struct Pair
{
  String key;
  U64 value;
};

#define ht_insert(arena, ht, k, v)         \
  do {                                           \
    if ((ht).count >= (ht).capacity)             \
    {                                            \
      dyn_array_grow(arena, (ht));               \
    }                                            \
    U64 hash = djb2_hash(k) % (ht).capacity;     \
    for (;(ht).items[hash] != 0;)                \
    {                                            \
      hash++;                                    \
      if (hash >= (ht).capacity)                 \
      {                                          \
        dyn_array_grow(arena, (ht));             \
      }                                          \
    }                                            \
    (ht).items[hash] = v;                        \
    (ht).count++;                                \
  } while (0);                                   \

#define ht_get_by_key(arena, ht, k) (ht).items[djb2_hash((k)) % (ht).capacity]

internal U64 djb2_hash(String key);

#endif
