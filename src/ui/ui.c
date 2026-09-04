internal UI_ID
ui_hash_string(String s)
{
  // NOTE(arka): 14695981039346656037 is the 64-bit FNV offset basis
  U64 hash = 14695981039346656037;
  for (U64 i = 0; i < s.count; i += 1)
  {
    hash ^= (U8)s.value[i];
    // NOTE(arka): 1099511628211 is the 64-bit FNV prime
    hash *= 1099511628211;
  }
  return hash;
}

internal void
ui_init_style(UI_Context *ui)
{
  ui->style.color_active  = (SDL_Color){70, 70, 90, 255};
  ui->style.color_hot     = (SDL_Color){90, 90, 120, 255};
  ui->style.color_idle    = (SDL_Color){55, 55, 65, 255};
  ui->style.color_text    = (SDL_Color){220, 220, 220, 255};
  ui->style.padding       = UI_PADDING;
  ui->style.font_size     = UI_FONT_SIZE;
  ui->style.widget_height = UI_WIDGET_HEIGHT;
}

internal void
ui_begin_frame(UI_Context *ui, G_Context *ctx)
{
  ui->hot = 0;

  ui->cursor_x = UI_DEFAULT_CURSOR_X;
  ui->cursor_y = UI_DEFAULT_CURSOR_Y;
  ui->origin_x = UI_DEFAULT_ORIGIN_X;
  ui->origin_y = UI_DEFAULT_ORIGIN_Y;

  ui->mouse_x        = ctx->input->mouse_x;
  ui->mouse_y        = ctx->input->mouse_y;
  ui->mouse_down     = ctx->input->mouse_down[1];
  ui->mouse_pressed  = ctx->input->mouse_pressed[1];
  ui->mouse_released = ctx->input->mouse_released[1];

  ui->same_line = false;
}

internal void
ui_end_frame(UI_Context *ui)
{
  // TODO(arka): maybe do something later.
}

internal B32
ui_aabb_point_in_rect(UI_Context *ui, SDL_FRect rect)
{
  return (ui->mouse_x >= rect.x && ui->mouse_x <= rect.x + rect.w &&
          ui->mouse_y >= rect.y && ui->mouse_y <= rect.y + rect.h);
}

internal B32
ui_button_behavior(UI_Context *ui, UI_ID id, SDL_FRect rect, B32 *out_hot, B32 *out_active)
{
  B32 over = ui_aabb_point_in_rect(ui, rect);
  B32 clicked = false;

  if (over && (ui->active == 0 || ui->active == id))
  {
    ui->hot = id;
  }
  if (ui->hot == id && ui->mouse_pressed)
  {
    ui->active = id;
  }
  if (ui->active == id && ui->mouse_released)
  {
    if (ui->hot == id) { clicked = true; }
    ui->active = 0;
  }

  if (out_hot)    { *out_hot    = (ui->hot == id); }
  if (out_active) { *out_active = (ui->active == id); }

  return clicked;
}

internal F32
ui_get_text_width(UI_Context *ui, String label, F32 font_size)
{
  if (label.count == 0) { return 0.0f; }

  F32 scale = (F32)font_size / (F32)ui->font->glyph_width;
  F32 char_offset = ui->font->kerning * scale + ui->font->offset;
  return (F32)(label.count - 1) * char_offset + font_size;
}

internal void
ui_draw_label_in_rect(UI_Context *ui, G_Context *ctx, String label, SDL_FRect rect, F32 font_size, B32 center_horizontal)
{
  F32 x = rect.x;
  F32 y = rect.y + (rect.h - (F32)font_size) * 0.5f;

  if (center_horizontal)
  {
    F32 text_width = ui_get_text_width(ui, label, (F32)font_size);
    x = rect.x + (rect.w - text_width) * 0.5f;
  }

  g_draw_text(ctx, label, x, y, font_size);
}

internal B32
ui_button(UI_Context *ui, G_Context *ctx, String label)
{
  UI_ID id = ui_hash_string(label);
  F32 width  = 120.0f;
  F32 height = UI_WIDGET_HEIGHT;
  F32 text_width = ui_get_text_width(ui, label, UI_FONT_SIZE);
  SDL_FRect rect =
  {
    ui->cursor_x,
    ui->cursor_y,
    width > text_width ? width : text_width + UI_PADDING,
    height
  };

  B32 hot = false;
  B32 active = false;
  B32 clicked = ui_button_behavior(ui, id, rect, &hot, &active);

  if (active)
  {
    SDL_SetRenderDrawColor(ctx->renderer,
                           ui->style.color_active.r,
                           ui->style.color_active.g,
                           ui->style.color_active.b,
                           ui->style.color_active.a);
  }
  else if (hot)
  {
    SDL_SetRenderDrawColor(ctx->renderer,
                           ui->style.color_hot.r,
                           ui->style.color_hot.g,
                           ui->style.color_hot.b,
                           ui->style.color_hot.a);
  }
  else
  {
    SDL_SetRenderDrawColor(ctx->renderer,
                           ui->style.color_idle.r,
                           ui->style.color_idle.g,
                           ui->style.color_idle.b,
                           ui->style.color_idle.a);
  }
  SDL_RenderFillRect(ctx->renderer, &rect);

  ui_draw_label_in_rect(ui, ctx, label, rect, UI_FONT_SIZE, true);
  ui_advance(ui, rect.w, height);

  return clicked;
}

internal void
ui_advance(UI_Context *ui, F32 width, F32 height)
{
  F32 right_edge = ui->cursor_x + width;

  if (right_edge > ui->content_width_required)
  {
    ui->content_width_required = right_edge;
  }

  if (ui->same_line)
  {
    ui->cursor_x += width + UI_PADDING;
  }
  else
  {
    ui->cursor_x = ui->origin_x;
    ui->cursor_y += height + UI_PADDING;
  }
}

internal void
ui_label(UI_Context *ui, G_Context *ctx, String text)
{
  F32 height = UI_WIDGET_HEIGHT;

  SDL_FRect label_rect =
  {
    ui->cursor_x,
    ui->cursor_y,
    ui_get_text_width(ui, text, UI_FONT_SIZE),
    UI_FONT_SIZE
  };

  ui_draw_label_in_rect(ui, ctx, text, label_rect, UI_FONT_SIZE, false);
  ui_advance(ui, label_rect.w, height);
}

internal B32
ui_checkbox(UI_Context *ui, G_Context *ctx, String label, B32 *value)
{
  UI_ID id = ui_hash_string(label);

  F32 box_size = UI_WIDGET_HEIGHT - 6.0f;
  SDL_FRect box_rect = {ui->cursor_x, ui->cursor_y, box_size, box_size};

  B32 hot = false;
  B32 active = false;
  B32 clicked = ui_button_behavior(ui, id, box_rect, &hot, &active);

  if (clicked) { *value = !*value; }

  if (active)
  {
    SDL_SetRenderDrawColor(ctx->renderer,
                           ui->style.color_active.r,
                           ui->style.color_active.g,
                           ui->style.color_active.b,
                           ui->style.color_active.a);
  }
  else if (hot)
  {
    SDL_SetRenderDrawColor(ctx->renderer,
                           ui->style.color_hot.r,
                           ui->style.color_hot.g,
                           ui->style.color_hot.b,
                           ui->style.color_hot.a);
  }
  else
  {
    SDL_SetRenderDrawColor(ctx->renderer,
                           ui->style.color_idle.r,
                           ui->style.color_idle.g,
                           ui->style.color_idle.b,
                           ui->style.color_idle.a);
  }
  SDL_RenderFillRect(ctx->renderer, &box_rect);

  if (*value)
  {
    SDL_FRect fill_rect = {box_rect.x + 4, box_rect.y + 4, box_size - 8, box_size - 8};
    SDL_SetRenderDrawColor(ctx->renderer, ui->style.color_text.r, ui->style.color_text.g, ui->style.color_text.b, ui->style.color_text.a);
    SDL_RenderFillRect(ctx->renderer, &fill_rect);
  }

  SDL_FRect label_rect =
  {
    .x = box_rect.x + box_size + UI_PADDING,
    .y = box_rect.y,
    .w = ui_get_text_width(ui, label, UI_FONT_SIZE),
    .h = box_size
  };

  ui_draw_label_in_rect(ui, ctx, label, label_rect, UI_FONT_SIZE, false);
  ui_advance(ui, label_rect.w, box_size);

  return clicked;
}

internal B32
ui_slider(UI_Context *ui, G_Context *ctx, String label, F32 *value, F32 min, F32 max)
{
  UI_ID id = ui_hash_string(label);

  F32 slider_width   = 160.0f;
  F32 slider_height  = UI_WIDGET_HEIGHT - 4.0f;
  SDL_FRect slider_rect = {ui->cursor_x, ui->cursor_y, slider_width, slider_height};

  B32 hot = false;
  B32 active = false;
  ui_button_behavior(ui, id, slider_rect, &hot, &active);

  B32 changed = false;
  if (active && ui->mouse_down)
  {
    F32 t = (ui->mouse_x - slider_rect.x) / slider_width;
    if (t < 0.0f) { t = 0.0f; }
    if (t > 1.0f) { t = 1.0f; }

    *value = min + t * (max - min);
    changed = true;
  }

  SDL_SetRenderDrawColor(ctx->renderer, 55, 55, 65, 255);
  SDL_RenderFillRect(ctx->renderer, &slider_rect);

  F32 fill_t = (*value - min) / (max - min);
  SDL_FRect fill_rect = {slider_rect.x, slider_rect.y, slider_width * fill_t, slider_height};
  SDL_SetRenderDrawColor(ctx->renderer, active ? 120 : (hot ? 100 : 80), 140, 200, 255);
  SDL_RenderFillRect(ctx->renderer, &fill_rect);

  ui_draw_label_in_rect(ui, ctx, string_lit(c_str_fmt("%.2f", *value)), slider_rect, UI_FONT_SIZE, true);

  SDL_FRect label_rect =
  {
    slider_rect.x + slider_width + UI_PADDING,
    slider_rect.y,
    ui_get_text_width(ui, label, UI_FONT_SIZE),
    slider_height
  };
  ui_draw_label_in_rect(ui, ctx, label, label_rect, UI_FONT_SIZE, false);
  ui_advance(ui, label_rect.w, slider_height);

  return changed;
}

internal UI_PanelState *
ui_get_panel_state(UI_Context *ui, UI_ID id, F32 default_x, F32 default_y)
{
  for (I32 i = 0; i < ui->panel_count; i++)
  {
    if (ui->panels[i].id == id) { return &ui->panels[i]; }
  }

  UI_PanelState *panel = &ui->panels[ui->panel_count];
  ui->panel_count++;
  panel->id = id;
  panel->x = default_x;
  panel->y = default_y;
  panel->width = UI_DEFAULT_PANEL_WIDTH;
  panel->height = UI_DEFAULT_PANEL_HEIGHT;

  return panel;
}

internal void
ui_begin_panel(UI_Context *ui, G_Context *ctx, String title, F32 default_x, F32 default_y)
{
  UI_ID id = ui_hash_string(title);
  UI_PanelState *panel = ui_get_panel_state(ui, id, default_x, default_y);
  panel->title = title;

  F32 width = panel->width;
  F32 height = panel->height;
  F32 title_bar_height = UI_DEFAULT_PANEL_TITLE_BAR_HEIGHT;
  SDL_FRect title_rect = {panel->x, panel->y, width, title_bar_height};

  B32 hot    = false;
  B32 active = false;
  ui_button_behavior(ui, id, title_rect, &hot, &active);

  if (hot && ui->mouse_pressed)
  {
    panel->drag_offset_x = ui->mouse_x - panel->x;
    panel->drag_offset_y = ui->mouse_y - panel->y;
  }
  if (active && ui->mouse_down)
  {
    panel->x = ui->mouse_x - panel->drag_offset_x;
    panel->y = ui->mouse_y - panel->drag_offset_y;
  }

  // NOTE: Panel background
  SDL_FRect body_rect = {panel->x, panel->y + title_bar_height, width, height}; // fixed height for now
  SDL_SetRenderDrawColor(ctx->renderer, 35, 35, 42, 230);
  SDL_RenderFillRect(ctx->renderer, &body_rect);

  // NOTE: Panel title bar
  SDL_SetRenderDrawColor(ctx->renderer, active ? 80 : 60, active ? 80 : 60, 100, 255);
  SDL_RenderFillRect(ctx->renderer, &title_rect);

  title_rect.x += UI_PADDING;
  ui_draw_label_in_rect(ui, ctx, title, title_rect, UI_FONT_SIZE, false);

  F32 padding = 10.0f;
  ui->origin_x = panel->x + padding;
  ui->origin_y = panel->y + padding + title_bar_height;
  ui->cursor_x = ui->origin_x;
  ui->cursor_y = ui->origin_y;
  ui->content_width_required = ui->origin_x;

  ui->current_panel = panel;
}

internal void
ui_end_panel(UI_Context *ui, G_Context *ctx)
{
  if (!ui->current_panel) { return; }

  F32 content_height = (ui->cursor_y - ui->origin_y) + UI_PADDING;
  if (content_height < UI_DEFAULT_PANEL_HEIGHT)
  {
    content_height = UI_DEFAULT_PANEL_HEIGHT;
  }

  F32 content_width = (ui->content_width_required - ui->current_panel->x) + UI_PADDING;
  if (content_width < UI_DEFAULT_PANEL_WIDTH)
  {
    content_width = UI_DEFAULT_PANEL_WIDTH;
  }

  ui->current_panel->width = content_width;
  ui->current_panel->height = content_height;
  ui->current_panel = NULL;
}

internal void
ui_set_cursor(UI_Context *ui, F32 x, F32 y)
{
  ui->cursor_x = x;
  ui->cursor_y = y;
}

internal void
ui_begin_same_line(UI_Context *ui)
{
  ui->same_line = true;
}

internal void
ui_end_same_line(UI_Context *ui)
{
  ui->same_line = false;
  ui->cursor_x = ui->origin_x;
  ui->cursor_y += UI_WIDGET_HEIGHT + UI_PADDING;
}

internal UI_ColorPickerState *
ui_get_color_picker_state(UI_Context *ui, UI_ID id, F32 default_x, F32 default_y)
{
  for (I32 idx = 0; idx < ui->color_picker_count; idx++)
  {
    if (ui->color_pickers[idx].id == id) { return &ui->color_pickers[idx]; }
  }

  UI_ColorPickerState *new_state = &ui->color_pickers[ui->color_picker_count];
  ui->color_picker_count++;
  new_state->id = id;
  new_state->x = default_x;
  new_state->y = default_y;

  return new_state;
}

internal SDL_Color
ui_get_bilinear_color_from_point(SDL_Color top_left, SDL_Color top_right, SDL_Color bottom_left, SDL_Color bottom_right, F32 px, F32 py)
{
  SDL_Color result;

  F32 top_r = top_left.r + (top_right.r - top_left.r) * px;
  F32 top_g = top_left.g + (top_right.g - top_left.g) * px;
  F32 top_b = top_left.b + (top_right.b - top_left.b) * px;

  F32 bot_r = bottom_left.r + (bottom_right.r - bottom_left.r) * px;
  F32 bot_g = bottom_left.g + (bottom_right.g - bottom_left.g) * px;
  F32 bot_b = bottom_left.b + (bottom_right.b - bottom_left.b) * px;

  result.r = (U8)(top_r + (bot_r - top_r) * py);
  result.g = (U8)(top_g + (bot_g - top_g) * py);
  result.b = (U8)(top_b + (bot_b - top_b) * py);
  result.a = 255;

  return result;
}

internal SDL_Color
ui_color_picker(UI_Context *ui, G_Context *ctx, String label)
{
#define COLOR_PICKER_WIDTH 270.0f
#define COLOR_PICKER_HEIGHT 290.0f

  SDL_Color result = {};
  UI_ID id = ui_hash_string(label);

  SDL_FRect color_rect_region =
  {
    .x = ui->cursor_x,
    .y = ui->cursor_y,
    .w = COLOR_PICKER_WIDTH,
    .h = COLOR_PICKER_HEIGHT
  };

  UI_ColorPickerState *state = ui_get_color_picker_state(ui, id, 0.5f, 0.5f);

  B32 hot = false;
  B32 active = false;
  ui_button_behavior(ui, id, color_rect_region, &hot, &active);

  if (ui_aabb_point_in_rect(ui, color_rect_region) && active)
  {
    state->x = clamp((ui->mouse_x - color_rect_region.x) / color_rect_region.w, 0.0f, 1.0f);
    state->y = clamp((ui->mouse_y - color_rect_region.y) / color_rect_region.h, 0.0f, 1.0f);
  }

  SDL_Color corner_top_left     = {255, 0,   0,   255};   // r
  SDL_Color corner_top_right    = {0,   0,   255, 255};   // b
  SDL_Color corner_bottom_right = {0,   255, 0,   255};   // g
  SDL_Color corner_bottom_left  = {255, 255, 255,   255}; // y
  result = ui_get_bilinear_color_from_point(corner_top_left, corner_top_right,
                                            corner_bottom_left, corner_bottom_right,
                                            state->x, state->y);

  SDL_Vertex vertices[4] = {};

  // NOTE: Top left
  vertices[0].position.x = color_rect_region.x;
  vertices[0].position.y = color_rect_region.y;
  vertices[0].color.r = 1.0;
  vertices[0].color.g = 0.0;
  vertices[0].color.b = 0.0;
  vertices[0].color.a = 1.0;

  // NOTE: Top right
  vertices[1].position.x = color_rect_region.x + color_rect_region.w;
  vertices[1].position.y = color_rect_region.y;
  vertices[1].color.r = 0.0;
  vertices[1].color.g = 0.0;
  vertices[1].color.b = 1.0;
  vertices[1].color.a = 1.0;

  // NOTE: Bottom right
  vertices[2].position.x = color_rect_region.x + color_rect_region.w;
  vertices[2].position.y = color_rect_region.y + color_rect_region.h;
  vertices[2].color.r = 0.0;
  vertices[2].color.g = 1.0;
  vertices[2].color.b = 0.0;
  vertices[2].color.a = 1.0;

  // NOTE: Bottom left
  vertices[3].position.x = color_rect_region.x;
  vertices[3].position.y = color_rect_region.y + color_rect_region.h;
  vertices[3].color.r = 1.0;
  vertices[3].color.g = 1.0;
  vertices[3].color.b = 1.0;
  vertices[3].color.a = 1.0;

  int indices[6] = {0, 1, 2,  0, 2, 3};

  SDL_RenderGeometry(ctx->renderer,
                     NULL,
                     vertices,
                     array_count(vertices),
                     indices,
                     array_count(indices));

  SDL_SetRenderDrawColor(ctx->renderer, 255.0f, 255.0f, 255.0f, 255.0f);
  // NOTE: Horizontal line
  SDL_RenderLine(ctx->renderer,
                 color_rect_region.x, color_rect_region.y + state->y * color_rect_region.h,
                 color_rect_region.x + color_rect_region.w, color_rect_region.y + state->y * color_rect_region.h);
  // NOTE: Vertical line
  SDL_RenderLine(ctx->renderer,
                 color_rect_region.x + state->x * color_rect_region.w, color_rect_region.y,
                 color_rect_region.x + state->x * color_rect_region.w, color_rect_region.y + color_rect_region.h);

  ui_advance(ui, color_rect_region.w, color_rect_region.h);

  F32 rgb_width = (color_rect_region.w - 2.0f * UI_PADDING) / 3.0f;
  SDL_FRect r_rect = {ui->cursor_x, ui->cursor_y, rgb_width, UI_WIDGET_HEIGHT};
  SDL_FRect g_rect = {r_rect.x + r_rect.w + UI_PADDING, ui->cursor_y, rgb_width, UI_WIDGET_HEIGHT};
  SDL_FRect b_rect = {g_rect.x + g_rect.w + UI_PADDING, ui->cursor_y, rgb_width, UI_WIDGET_HEIGHT};

  SDL_SetRenderDrawColor(ctx->renderer, 55, 55, 65, 255);
  SDL_RenderFillRect(ctx->renderer, &r_rect);
  SDL_RenderFillRect(ctx->renderer, &g_rect);
  SDL_RenderFillRect(ctx->renderer, &b_rect);
  ui_draw_label_in_rect(ui, ctx, string_lit(c_str_fmt("R: %d", result.r)), r_rect, UI_FONT_SIZE, false);
  ui_draw_label_in_rect(ui, ctx, string_lit(c_str_fmt("G: %d", result.g)), g_rect, UI_FONT_SIZE, false);
  ui_draw_label_in_rect(ui, ctx, string_lit(c_str_fmt("B: %d", result.b)), b_rect, UI_FONT_SIZE, false);

  ui_advance(ui, r_rect.w, r_rect.h);

  SDL_FRect hex_rect = {ui->cursor_x, ui->cursor_y, color_rect_region.w, UI_WIDGET_HEIGHT};
  String hex_value = string_lit(c_str_fmt("HEX: #%02x%02x%02x", result.r, result.g, result.b));
  SDL_RenderFillRect(ctx->renderer, &hex_rect);
  ui_draw_label_in_rect(ui, ctx, hex_value, hex_rect, UI_FONT_SIZE, false);

  ui_advance(ui, hex_rect.w, hex_rect.h);

  return result;
}

internal void
ui_image(UI_Context *ui, G_Context *ctx, SDL_Texture *texture, F32 scale)
{
  F32 texture_width  = 0.0f;
  F32 texture_height = 0.0f;
  SDL_GetTextureSize(texture, &texture_width, &texture_height);

  SDL_FRect src = {0.0f, 0.0f, texture_width, texture_height};
  SDL_FRect dst = {ui->cursor_x, ui->cursor_y, texture_width * scale, texture_height * scale};

  SDL_RenderTexture(ctx->renderer, texture, &src, &dst);
  ui_advance(ui, dst.w, dst.h);
}

internal void
ui_keymap_hint(UI_Context *ui, G_Context *ctx, SDL_Texture *texture, U64 row, U64 col, U64 size, String label)
{
  F32 texture_width  = 0.0f;
  F32 texture_height = 0.0f;
  SDL_GetTextureSize(texture, &texture_width, &texture_height);

  SDL_FRect src =
  {
    .x = 0.0f + ctx->keymap_width * col,
    .y = 0.0f + row * ctx->keymap_height,
    .h = ctx->keymap_width,
    .w = ctx->keymap_height
  };
  SDL_FRect dst = {ui->cursor_x, ui->cursor_y, size, size};
  SDL_RenderTexture(ctx->renderer, texture, &src, &dst);

  SDL_FRect rect = {dst.x + size + UI_PADDING, dst.y, ui_get_text_width(ui, label, UI_FONT_SIZE), size};
  ui_draw_label_in_rect(ui, ctx, label, rect, UI_FONT_SIZE, 0);

  ui_advance(ui, dst.w + rect.w, rect.h);
}
