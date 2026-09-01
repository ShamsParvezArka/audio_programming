#ifndef WIN32_BASE_H
#define WIN32_BASE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdi32.lib")

internal void *memory_commit(void *ptr, U64 size);
internal void  memory_decommit(void *ptr, U64 size);
internal void *memory_reserve(U64 size);
internal void  memory_release(void *ptr, U64 size);



#endif
