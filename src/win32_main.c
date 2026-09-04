//~ Includes [h]
#include "base/base_inc.h"
#include "win32/win32_inc.h"
#include "input/input.h"
#include "audio/audio_inc.h"
#include "render/render_inc.h"
#include "ui/ui_inc.h"

//~ Includes [c]
#include "base/base_inc.c"
#include "win32/win32_inc.c"
#include "input/input.c"
#include "audio/audio.c"
#include "render/render_inc.c"
#include "ui/ui_inc.c"
#include "quick_main.c"

#ifndef BUILD_CONSOLE_INTERFACE
#  define BUILD_CONSOLE_INTERFACE 0
#endif

typedef struct HashTable HashTable;
struct HashTable
{
  _DynArrayHeader_;
  I32 *items;
};

#if BUILD_CONSOLE_INTERFACE
I32 wmain(I32 argc, wchar_t **argv, wchar_t **envp)
{
  quick_entry_point();
  return 0;
}
#else
I32 WINAPI
WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, I32 show_code)
{
  CoInitializeEx(0, COINIT_APARTMENTTHREADED);
  quick_entry_point();
  return 0;
}
#endif
