////////////////////////////////
// NOTE: memory allocation procedures are taken from the `raddebugger` codebase
//
internal void *
memory_reserve(U64 size)
{
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return result;
}

internal void *
memory_commit(void *ptr, U64 size)
{
  void *result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
  return result;
}

internal void
memory_decommit(void *ptr, U64 size)
{
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal void
memory_release(void *ptr, U64 size)
{
  // NOTE(rjf): size not used - not necessary on Windows, but necessary for other OSes.
  VirtualFree(ptr, 0, MEM_RELEASE);
}
