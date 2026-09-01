#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#ifndef ARENA_DEFAULT_RESERVE_SIZE
#  define ARENA_DEFAULT_RESERVE_SIZE GB(1)
#endif
#ifndef ARENA_DEFAULT_COMMIT_SIZE
#  define ARENA_DEFAULT_COMMIT_SIZE KB(4)
#endif

#define arena_push(arena, size)                      arena_push_aligned((arena), (size), 8)
#define arena_push_array(arena, type, count) (type *)arena_push_aligned((arena), sizeof(type) * (count), alignof(type))
#define arena_push_struct(arena, type)       (type *)arena_push_aligned((arena), sizeof(type), alignof(type))

typedef struct Arena Arena;
struct Arena
{
  U8 *base;
  U64 reserved;
  U64 committed;
  U64 used;
};

internal Arena *arena_alloc(U64 size);
internal void  *arena_push_aligned(Arena *arena, U64 size, U64 align);
internal void   arena_release(Arena *arena);

#endif // BASE_ARENA_H
