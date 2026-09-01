internal Arena *
arena_alloc(U64 size)
{
  void *base = memory_commit(0, size);

  Arena *arena     = base;
  arena->base      = (U8 *)base;
  arena->reserved  = size;
  arena->committed = ARENA_DEFAULT_COMMIT_SIZE;
  arena->used      = sizeof(Arena);

  return arena;
}

internal void *
arena_push_aligned(Arena *arena, U64 size, U64 align)
{
  U64 memptr = (U64)(arena->base + arena->used);
  U64 memptr_aligned = align_pow2(memptr, align);
  U64 offset = (memptr_aligned - (U64)arena->base);
  U64 new_used = offset + size;

  assert(new_used <= arena->reserved && "error: arena out of memory.");

  if (new_used > arena->committed)
  {
    U64 new_commit_mem = min(align_pow2(new_used, ARENA_DEFAULT_COMMIT_SIZE), arena->reserved);
    memory_commit((U8 *)arena->base + arena->committed, new_commit_mem - arena->committed);
    arena->committed = new_commit_mem;
  }

  arena->used = new_used;
  return (void *)memptr_aligned;
}

internal void
arena_release(Arena *arena)
{
  memory_release(arena->base, 0);
  memory_release(arena, 0);
}
