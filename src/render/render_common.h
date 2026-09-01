#ifndef COMMON_H
#define COMMON_H

#define SDL_Require(expr)                               \
  do {                                                  \
    if (!(expr))                                        \
    {                                                   \
      SDL_Log("Require failer: %s\nSDL_Error: %s\n",    \
              stringify(expr), SDL_GetError());         \
      assert((expr));                                   \
    }                                                   \
  } while (0)

#define KeyboardDown(sc, in) do { (in)->kbd_down[(sc)] = true; if (!(in)->kbd_pressed[(sc)]) (in)->kbd_pressed[(sc)] = true; } while (0)
#define KeyboardUp(sc, in)   do { (in)->kbd_down[(sc)] = false; (in)->kbd_released[(sc)] = true; } while (0)
#define MouseDown(btn, in)   do { (in)->mouse_down[(btn)] = true; if (!(in)->mouse_pressed[(btn)]) (in)->mouse_pressed[(btn)] = true; } while (0)
#define MouseUp(btn, in)     do { (in)->mouse_down[(btn)] = false; (in)->mouse_released[(btn)] = true; } while (0)

typedef struct G_Config G_Config;
struct G_Config
{
  U8 r, g, b, a;
};

typedef struct FontAtlas FontAtlas;
struct FontAtlas
{
  SDL_Texture *tex;

  I32 glyph_width;
  I32 glyph_height;
  I32 columns;
  I32 first_char;
  I32 kerning;
  I32 offset;
};

typedef struct G_InputState G_InputState;
struct G_InputState
{
  F32 mouse_x;
  F32 mouse_y;
  F32 wheel;

  B32 kbd_down[SDL_SCANCODE_COUNT];
  B32 kbd_pressed[SDL_SCANCODE_COUNT];
  B32 kbd_released[SDL_SCANCODE_COUNT];

  B32 mouse_down[5];
  B32 mouse_pressed[5];
  B32 mouse_released[5];

  U8 ascii_char;
};


typedef struct G_Context G_Context;
struct G_Context
{
  FontAtlas *font;

  SDL_Window   *window;
  U8           *window_title;
  SDL_Renderer *renderer;
  G_InputState *input;
  G_Config     *config;

  I32 window_width;
  I32 window_height;
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

  F32 outer_boundary_width;
  F32 outer_boundary_height;
  F32 line_pos_x1;
  F32 line_pos_y1;
  F32 line_pos_x2;
  F32 line_pos_y2;
};

typedef enum G_InputMap G_InputMap;
enum G_InputMap
{
  G_InputMap_C4  = SDL_SCANCODE_Z,
  G_InputMap_Cs4 = SDL_SCANCODE_S,
  G_InputMap_D4  = SDL_SCANCODE_X,
  G_InputMap_Ds4 = SDL_SCANCODE_D,
  G_InputMap_E4  = SDL_SCANCODE_C,
  G_InputMap_F4  = SDL_SCANCODE_V,
  G_InputMap_Fs4 = SDL_SCANCODE_G,
  G_InputMap_G4  = SDL_SCANCODE_B,
  G_InputMap_Gs4 = SDL_SCANCODE_H,
  G_InputMap_A4  = SDL_SCANCODE_N,
  G_InputMap_As4 = SDL_SCANCODE_J,
  G_InputMap_B4  = SDL_SCANCODE_M,
  G_InputMap_C5  = SDL_SCANCODE_COMMA
};

typedef struct UI_Context UI_Context;

internal void g_reset_input_state(G_InputState *input);
internal void g_dispatch_event(G_Context *ctx, SDL_Event *event);
internal void g_update(G_Context *ctx, G_State *state, F32 delta_time);
internal void g_render(G_Context *ctx, G_State *state, UI_Context *ui_ctx, F32 delta_time);
internal void g_quit(G_Context *ctx);
internal void g_draw_char(G_Context *ctx, U8 c, F32 x, F32 y, U32 font_size);
internal void g_draw_text(G_Context *ctx, String s, F32 x, F32 y, U32 font_size);
internal void g_draw_circle(G_Context *ctx, I32 center_x, I32 center_y, I32 radius);

#endif // COMMON_H
