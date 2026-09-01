#define string_lit(s) string_from_c_str((s))
#define string_arg(s) (I32)(s).count, (s).value

#define string_builder_push(sb, n) DynArrayPush(sb)
#define string_builder_insert(sb, p, n) DynArrayInsert(sb, p, n)
#define string_builder_unorderer_insert(sb, p, n) DynArrayUnorderedInsert(sb, p, n)
#define string_builder_pop(sb, p) DynArrayPop(sb, p)
#define string_builder_unorderer_pop(sb, p) DynArrayUnorderedPop(sb, p)

typedef enum StringMatchFlag StringMatchFlag;
enum StringMatchFlag
{
  StringMatchFlag_CaseSensitive   = 1 << 0,
  StringMatchFlag_CaseInsensitive = 1 << 1
};

typedef struct String String;
struct String
{
  U8 *value;
  U64 count;
};

typedef struct StringBuilder StringBuilder;
struct StringBuilder
{
  _DynArrayHeader_;
  union
  {
    U8 *items;
    U8 *value;
  };
};

internal B32 char_is_space(U8 c);
internal B32 char_is_lower(U8 c);
internal B32 char_is_upper(U8 c);
internal B32 char_is_alpha(U8 c);
internal B32 char_is_slash(U8 c);
internal B32 char_is_digit(U8 c, U32 base);
internal B32 string_match(String a, String b, StringMatchFlag flag);

internal U8  char_make_lower(U8 c);
internal U8  char_make_upper(U8 c);
internal U8  char_correct_slash(U8 c);
internal U8 *c_str_fmt(U8 *fmt, ...);

internal String string_from_c_str(U8 *s);
internal String string_sub(String s, I64 begin, I64 size);
internal String string_fmt_on_heap(Arena *arena, U8 *fmt, ...);
