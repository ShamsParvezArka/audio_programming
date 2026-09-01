internal B32
char_is_space(U8 c)
{
  return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

internal B32
char_is_lower(U8 c)
{
  return ('A' <= c && c <= 'Z');
}

internal B32
char_is_upper(U8 c)
{
  return ('a' <= c && c <= 'z');
}

internal B32
char_is_alpha(U8 c)
{
  return (CharIsUpper(c) || CharIsLower(c));
}

internal B32
char_is_slash(U8 c)
{
  return (c == '/' || c == '\\');
}

internal B32
char_is_digit(U8 c, U32 base)
{
  B32 result = 0;
  if(0 < base && base <= 16)
  {
    U8 val = integer_symbol_reverse[c];
    if(val < base)
    {
      result = 1;
    }
  }
  return result;
}

internal U8
char_make_lower(U8 c)
{
  if(CharIsUpper(c))
  {
    c += ('a' - 'A');
  }
  return c;
}

internal U8
char_make_upper(U8 c)
{
  if(CharIsLower(c))
  {
    c += ('A' - 'a');
  }
  return c;
}

internal U8
char_correct_slash(U8 c)
{
  if(CharIsSlash(c))
  {
    c = '/';
  }
  return c;
}

internal String
string_from_c_str(U8 *s)
{
  String result = {0};
  result.value = s;
  result.count = memory_c_str_length(s);

  return result;
}

internal String
string_sub(String s, I64 begin, I64 size)
{
  begin = Min(Max(begin, 0), s.count);
  size = Min(Max(size, 0), s.count - begin);

  String result = {};
  result.value = s.value + begin;
  result.count = size;

  return result;
}

internal B32
string_match(String a, String b, StringMatchFlag flag)
{
  B32 result = 0;

  if (a.count == b.count && flag == StringMatchFlag_CaseSensitive)
  {
    result = MemoryMatch(a.value, b.value, b.count);
  }
  if (a.count == b.count && flag == StringMatchFlag_CaseInsensitive)
  {
    result = 1;
    for (U64 idx = 0; idx < a.count; idx++)
    {
      U8 at = CharMakeLower(a.value[idx]);
      U8 bt = CharMakeLower(b.value[idx]);
      if (at != bt)
      {
        result = 0;
        break;
      }
    }
  }

  return result;
}

// NOTE(arka): Try not to use this procedure. This will allocated memory on the heap
// thus have to be cleaned from time to time. Otherwise, it will lead to memory leak.
internal String
string_fmt_on_heap(Arena *arena, U8 *fmt, ...)
{
  va_list args;
  va_list args_backup;
  va_start(args, fmt);
  va_copy(args_backup, args);

  U32 needed_bytes = vsnprintf(0, 0, fmt, args) + 1;

  String result = {};
  result.value = arena_push_array(arena, U8, needed_bytes);
  result.count = vsnprintf((U8 *)result.value, needed_bytes, fmt, args_backup);

  va_end(args);
  va_end(args_backup);
  return result;
}

internal U8 *
c_str_fmt(U8 *fmt, ...)
{
#ifndef MAX_CSTR_BUFFER_SIZE
#  define MAX_CSTR_BUFFER_SIZE 1024
#endif
#ifndef MAX_CSTR_BUFFERS
#  define MAX_CSTR_BUFFERS 64
#endif

  local_persist U8 buffers[MAX_CSTR_BUFFERS][MAX_CSTR_BUFFER_SIZE] = {0};
  local_persist U32 idx = 0;

  U8 *current_buffer = buffers[idx];
  memory_zero(current_buffer, MAX_CSTR_BUFFER_SIZE);

  va_list args;
  va_start(args, fmt);
  int needed_bytes = vsnprintf(current_buffer, MAX_CSTR_BUFFER_SIZE, fmt, args);
  va_end(args);

  if (needed_bytes >= MAX_CSTR_BUFFER_SIZE)
  {
    U8 *truncated_buffer = buffers[idx] + MAX_CSTR_BUFFER_SIZE - MAX_CSTR_BUFFERS;
    sprintf(truncated_buffer, "...");
  }

  idx += 1;
  if (idx >= MAX_CSTR_BUFFERS) { idx = 0; }

  return current_buffer;
}
