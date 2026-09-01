#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

////////////////////////////////
// NOTE: Codebase Keyword
//
#define global        static
#define local_persist static
#define internal      static

////////////////////////////////
// NOTE: Units
//
#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)

////////////////////////////////
// NOTE: Helper Macros
//
#define stringify_(S) #S
#define stringify(S) stringify_(S)
#define glue_(a, b) a##b
#define glue(a, b) glue_(a, b)
#define array_count(a) (sizeof(a) / sizeof((a)[0]))

////////////////////////////////
// NOTE: Memory Operation
//
#if defined(_MSC_VER)
#  define alignof(type) __alignof(type)
#endif
#if defined(_MSC_VER)
#  define typeof(type) __typeof__(type)
#endif
#define align_pow2(ptr, x) (((ptr) + (x) - 1) & (~((x) - 1)))
#define is_pow2(x)         ((x) != 0 && ((x) & ((x) - 1)) == 0)

#define swap(type, a, b) do {type glue(_type_, __LINE__) = a; a = b; b = glue(_type_, __LINE__);} while (0)

#define memory_compare(a, b, size) memcmp((a), (b), (size))
#define memory_match(a, b, size)  (memory_compare((a), (b), (size)) == 0)
#define memory_zero(s, size)       memset((s), 0, (size))
#define memory_c_str_length(s)     strlen((s))
#define memory_copy(dst, src, s)   memcpy((dst), (src), s)

////////////////////////////////
// NOTE: Loop macros
//
#define defer_scope(begin, end)        for (U32 glue(_i_, __LINE__) = ((begin), 0); !glue(_i_, __LINE__); glue(_i_, __LINE__) += 1, (end))
#define defer_scope_checked(begin, end) for (U32 glue(_i_, __LINE__) = 2 * !(begin); (glue(_i_, __LINE__) == 2 ? ((end), 0) : !glue(_i_, __LINE__)); glue(_i_, __LINE__) += 1, (end))

#define foreach(type_var, array)                                        \
  for (U64 glue(_i_, __LINE__) = 0, glue(_s_, __LINE__) = Carray_count(array); \
       glue(_i_, __LINE__) < glue(_s_, __LINE__);                       \
       glue(_i_, __LINE__)++)                                           \
    for (U64 _done_ = 0; !_done_; _done_ += 1)                          \
      for (type_var = (array)[glue(_i_, __LINE__)]; !_done_; _done_ += 1)

////////////////////////////////
// NOTE: Base Types
//
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t   I8;
typedef int16_t  I16;
typedef int32_t  I32;
typedef int64_t  I64;
typedef float    F32;
typedef double   F64;
typedef I32      B32;

////////////////////////////////
// NOTE: Useful structs
//
typedef struct Range Range;
struct Range
{
  I64 min;
  I64 max;
};

typedef struct DateTime DateTime;
struct DateTime
{
  U16 micro_sec;
  U16 milli_sec;
  U16 sec;
  U16 min;
  U16 hour;
  U16 day;
  U16 mon;
  U32 year;
};

global const U8 integer_symbol_reverse[128] =
{
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

internal void     win32_spawn_console();
internal DateTime date_time_from_unix_time(U64 time);

#endif // BASE_H
