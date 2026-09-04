#ifndef COMMON_H
#define COMMON_H

#define MAX_SPAWNED_CIRCLE 1024
#define MAX_CIRCLE_LIFETIME 0.75f

#define SDL_Require(expr)                               \
  do {                                                  \
    if (!(expr))                                        \
    {                                                   \
      SDL_Log("Require failer: %s\nSDL_Error: %s\n",    \
              stringify(expr), SDL_GetError());         \
      assert((expr));                                   \
    }                                                   \
  } while (0)

typedef struct G_Config G_Config;
struct G_Config
{
  U8 r, g, b, a;
};

typedef struct G_FontAtlas G_FontAtlas;
struct G_FontAtlas
{
  SDL_Texture *tex;

  I32 glyph_width;
  I32 glyph_height;
  I32 columns;
  I32 first_char;
  I32 kerning;
  I32 offset;
};

typedef struct G_Context G_Context;
struct G_Context
{
  SDL_Window    *window;
  SDL_Renderer  *renderer;
  SDL_AudioSpec *audio_spec;
  SDL_Texture   *keymap_spritesheet;

  G_FontAtlas  *font;
  G_InputState *input;
  G_Config     *config;

  U8 *window_title;
  I32 window_width;
  I32 window_height;
  I32 keymap_width;
  I32 keymap_height;
};

typedef struct G_Camera G_Camera;
struct G_Camera
{
  F32 world_x;
  F32 world_y;
  F32 last_world_x;
  F32 last_world_y;

  F32 offset_x;
  F32 offset_y;
  F32 target_x;
  F32 target_y;

  F32 rotation;
  F32 zoom;
  F32 zoom_target;
};

typedef struct G_State G_State;
struct G_State
{
  G_Camera *camera;

  F32 outer_boundary_x;
  F32 outer_boundary_y;
  F32 outer_boundary_width;
  F32 outer_boundary_height;
  F32 line_pos_x1;
  F32 line_pos_y1;
  F32 line_pos_x2;
  F32 line_pos_y2;

  F32 circle_x[MAX_SPAWNED_CIRCLE];
  F32 circle_y[MAX_SPAWNED_CIRCLE];
  F32 circle_lifetime[MAX_SPAWNED_CIRCLE];
  U64 circle_count;
};

typedef struct UI_Context UI_Context;

internal void g_update(G_Context *ctx, G_State *state, F32 delta_time);
internal void g_render(G_Context *ctx, G_State *state, UI_Context *ui_ctx, F32 delta_time);
internal void g_quit(G_Context *ctx);
internal void g_draw_char(G_Context *ctx, U8 c, F32 x, F32 y, U32 font_size);
internal void g_draw_text(G_Context *ctx, String s, F32 x, F32 y, U32 font_size);
internal void g_draw_circle(G_Context *ctx, I32 center_x, I32 center_y, I32 radius);
internal void g_draw_circle_ex(G_Context *ctx, I32 center_x, I32 center_y, U32 radius, U32 thickness);

#endif // COMMON_H
