#ifndef UI_H
#define UI_H

#define UI_DEFAULT_CURSOR_X 20
#define UI_DEFAULT_CURSOR_Y 20
#define UI_DEFAULT_ORIGIN_X 20
#define UI_DEFAULT_ORIGIN_Y 20

#define UI_DEFAULT_PANEL_WIDTH 320
#define UI_DEFAULT_PANEL_HEIGHT 280
#define UI_DEFAULT_PANEL_TITLE_BAR_HEIGHT 28

#ifndef UI_FONT_SIZE
#  define UI_FONT_SIZE 22.0f
#endif
#define UI_PADDING 8.0f
#define UI_WIDGET_HEIGHT 28.0f
#define UI_MAX_PANELS 16

typedef U64 UI_ID;

typedef struct UI_PanelState UI_PanelState;
struct UI_PanelState
{
  UI_ID id;
  String title;

  F32 x;
  F32 y;
  F32 width;
  F32 height;
  F32 drag_offset_x;
  F32 drag_offset_y;
};

typedef struct UI_ColorPickerState UI_ColorPickerState;
struct UI_ColorPickerState
{
  UI_ID id;
  F32 x;
  F32 y;
};

typedef struct UI_Style UI_Style;
struct UI_Style
{
  SDL_Color color_idle;
  SDL_Color color_hot;
  SDL_Color color_active;
  SDL_Color color_text;

  F32 padding;
  F32 font_size;
  F32 widget_height;
};

typedef struct UI_Context UI_Context;
struct UI_Context
{
  G_FontAtlas *font;

  UI_Style style;
  UI_ID hot;
  UI_ID active;

  F32 content_width_required;

  F32 cursor_x;
  F32 cursor_y;
  F32 origin_x;
  F32 origin_y;

  F32 mouse_x;
  F32 mouse_y;
  B32 mouse_down;
  B32 mouse_pressed;
  B32 mouse_released;

  UI_PanelState panels[UI_MAX_PANELS];
  UI_PanelState *current_panel;
  I32 panel_count;

  UI_ColorPickerState color_pickers[UI_MAX_PANELS];
  I32 color_picker_count;

  B32 same_line;
};

internal UI_ID ui_hash_string(String s);

internal void ui_init_style(UI_Context *ui);
internal void ui_begin_frame(UI_Context *ui, G_Context *ctx);
internal void ui_end_frame(UI_Context *ui);
internal void ui_draw_label_in_rect(UI_Context *ui, G_Context *ctx, String label, SDL_FRect rect, F32 font_size, B32 center_horizontal);
internal void ui_label(UI_Context *ui, G_Context *ctx, String text);
internal void ui_begin_panel(UI_Context *ui, G_Context *ctx, String title, F32 default_x, F32 default_y);
internal void ui_end_panel(UI_Context *ui, G_Context *ctx);
internal void ui_set_cursor(UI_Context *ui, F32 x, F32 y);
internal void ui_begin_same_line(UI_Context *ui);
internal void ui_end_same_line(UI_Context *ui);
internal void ui_advance(UI_Context *ui, F32 width, F32 height);
internal void ui_image(UI_Context *ui, G_Context *ctx, SDL_Texture *texture, F32 scale);
internal void ui_keymap_hint(UI_Context *ui, G_Context *ctx, SDL_Texture *texture, U64 row, U64 col, U64 size, String label);

internal F32 ui_get_text_width(UI_Context *ui, String label, F32 font_size);

internal B32 ui_mouse_over_rect(UI_Context *ui, SDL_FRect rect);
internal B32 ui_button_behavior(UI_Context *ui, UI_ID id, SDL_FRect rect, B32 *out_hot, B32 *out_active);
internal B32 ui_button(UI_Context *ui, G_Context *ctx, String label);
internal B32 ui_checkbox(UI_Context *ui, G_Context *ctx, String label, B32 *value);
internal B32 ui_slider(UI_Context *ui, G_Context *ctx, String label, F32 *value, F32 min, F32 max);
internal B32 ui_aabb_point_in_rect(UI_Context *ui, SDL_FRect rect);

internal UI_PanelState *ui_get_panel_state(UI_Context *ui, UI_ID id, F32 default_x, F32 default_y);
internal UI_ColorPickerState *ui_get_color_picker_state(UI_Context *ui, UI_ID id, F32 default_x, F32 default_y);

internal SDL_Color ui_color_picker(UI_Context *ui, G_Context *ctx, String label);
internal SDL_Color ui_get_bilinear_color_from_point(SDL_Color top_left, SDL_Color top_right, SDL_Color bottom_left, SDL_Color bottom_right, F32 px, F32 py);

#endif // UI_H
