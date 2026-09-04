global I32 octave_shift = 4;
global F32 accumulated_time = 0.0f;
global F64 frame_count = 0;
global U64 fps = 0;

internal void
g_update(G_Context *ctx, G_State *state, F32 delta_time)
{
  accumulated_time += delta_time;
  frame_count++;
  if (accumulated_time >= 0.25)
  {
    fps = (U32)(ceil(frame_count / accumulated_time));
    accumulated_time = 0.0f;
    frame_count = 0;
  }

  F32 start_pos_x = state->outer_boundary_x;
  F32 end_pos_x = start_pos_x + state->outer_boundary_width;
  if (state->line_pos_x1 >= end_pos_x)
  {
    state->line_pos_x1 = start_pos_x;
    state->line_pos_x2 = start_pos_x;
  }
  state->line_pos_x1 += 600.0f * delta_time;
  state->line_pos_x2 += 600.0f * delta_time;
  state->line_pos_y1 = state->outer_boundary_y;
  state->line_pos_y2 = state->line_pos_y1 + state->outer_boundary_height;

  G_InputState *in = ctx->input;
  if (in->kbd_down[G_InputMap_OctaveUpShifter])
  {
    octave_shift = 5;
  }
  else if (in->kbd_down[G_InputMap_OctaveDownShifter])
  {
    octave_shift = 3;
  }
  else
  {
    octave_shift = 4;
  }

  for (U64 idx = 0; idx < array_count(audio_note_lookup); idx++)
  {
    audio_note_lookup[idx].active = in->kbd_down[audio_note_lookup[idx].associated_key];
  }
  for (U64 idx = 0; idx < state->circle_count; idx++)
  {
    state->circle_lifetime[idx] += delta_time;
  }
  for (U64 idx = 0; idx < state->circle_count;)
  {
    if (state->circle_lifetime[idx] >= MAX_CIRCLE_LIFETIME)
    {
      U64 last = state->circle_count - 1;
      state->circle_x[idx] = state->circle_x[last];
      state->circle_y[idx] = state->circle_y[last];
      state->circle_lifetime[idx] = state->circle_lifetime[last];
      state->circle_count--;
    }
    else
    {
      idx++;
    }
  }

  local_persist B32 was_down[array_count(audio_note_lookup)] = {};
  for (U64 idx = 0; idx < array_count(audio_note_lookup); idx++)
  {
    B32 down = in->kbd_down[audio_note_lookup[idx].associated_key];

    if (down && !was_down[idx])
    {
      if (state->circle_count < MAX_SPAWNED_CIRCLE)
      {
        state->circle_x[state->circle_count] = state->line_pos_x1;
        state->circle_y[state->circle_count] = state->line_pos_y1 + state->outer_boundary_height - 50.0f * (F32)idx - 50;
        state->circle_lifetime[state->circle_count] = 0.0f;
        state->circle_count++;
      }
    }
    was_down[idx] = down;
  }
}

internal void
g_render(G_Context *ctx, G_State *state, UI_Context *ui, F32 delta_time)
{
  G_InputState *in = ctx->input;

  ////////////////////////////////
  // NOTE(arka): Game rendering
  //
  SDL_SetRenderDrawColor(ctx->renderer, ctx->config->r, ctx->config->g, ctx->config->b, ctx->config->a);
  SDL_RenderClear(ctx->renderer);

  // NOTE(arka): boundary rendering
  SDL_FRect outer_boundary_rect =
  {
    .x = state->outer_boundary_x,
    .y = state->outer_boundary_y,
    .w = state->outer_boundary_width,
    .h = state->outer_boundary_height
  };

  SDL_SetRenderDrawColor(ctx->renderer, 255.0f, 255.0f, 255.0f, 255.0f);
  SDL_RenderRect(ctx->renderer, &outer_boundary_rect);

  // NOTE(arka): horizontal lines
  SDL_SetRenderDrawColor(ctx->renderer, 255.0f, 255.0f, 255.0f, 100.0f);
  for (U64 idx = 0; idx < 4; idx++)
  {
    F32 x = outer_boundary_rect.x;
    F32 y = outer_boundary_rect.y;
    F32 offset = state->outer_boundary_width / 4.0f;
    SDL_RenderLine(ctx->renderer,
                   x + (offset * idx), y,
                   x + (offset * idx), y + state->outer_boundary_height);
  }

  // NOTE(arka): horizontal moving line rendering
  SDL_RenderLine(ctx->renderer,
                 state->line_pos_x1, state->line_pos_y1,
                 state->line_pos_x2, state->line_pos_y2);

  ////////////////////////////////
  // NOTE(arka): audio note circle rendering part
  //
  B32 is_red = 0;
  for (U64 idx = 0; idx < state->circle_count; idx++)
  {
    F32 t = state->circle_lifetime[idx] / MAX_CIRCLE_LIFETIME;
    F32 expand_coefficient = 1.0f + t * 2;
    F32 opacity = lerp(255.0f, 0.0f, t);
    F32 radius = 20.0f;

    if (is_red)
    {
      SDL_SetRenderDrawColor(ctx->renderer, 9.0f, 90.0f, 177.0f, opacity);
      is_red = 0;
    }
    else
    {
      SDL_SetRenderDrawColor(ctx->renderer, 162.0f, 53.0f, 55.0f, opacity);
      is_red = 1;
    }

    g_draw_circle_ex(ctx,
                     state->circle_x[idx] - radius, state->circle_y[idx],
                     radius * expand_coefficient,
                     10.0f);
  }

  ////////////////////////////////
  // NOTE: UI rendering
  //
  g_draw_text(ctx,
              string_lit(c_str_fmt("Octave %d", octave_shift)),
              state->outer_boundary_x + UI_PADDING,
              state->outer_boundary_y + UI_PADDING,
              52);
  local_persist B32 note_guide = 1;
  ui_begin_panel(ui, ctx, string_lit("debug panel"), 30.0f, state->outer_boundary_y);
  {
    ui_label(ui, ctx, string_lit(c_str_fmt("fps: %d", fps)));
    ui_label(ui, ctx, string_lit(c_str_fmt("delta: %.4f", delta_time)));
    ui_checkbox(ui, ctx, string_lit("keymap guide panel"), &note_guide);
  }
  ui_end_panel(ui, ctx);

  if (note_guide)
  {
    ui_begin_panel(ui,
                   ctx,
                   string_lit("keymap guide"),
                   ui->cursor_x - UI_PADDING,
                   state->outer_boundary_y + UI_DEFAULT_PANEL_HEIGHT + UI_DEFAULT_PANEL_TITLE_BAR_HEIGHT + UI_PADDING);
    {
      for (U64 idx = 0; idx < array_count(audio_note_lookup); idx++)
      {
        ui_keymap_hint(ui,
                       ctx,
                       ctx->keymap_spritesheet,
                       in->kbd_down[audio_note_lookup[idx].associated_key] ? 1 : 0,
                       idx,
                       22,
                       audio_note_lookup[idx].note_name);
      }
    }
    ui_end_panel(ui, ctx);
  }

  SDL_RenderPresent(ctx->renderer);
}

internal void
g_quit(G_Context *ctx)
{
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
  SDL_Quit();
}

internal void
g_draw_char(G_Context *ctx, U8 c, F32 x, F32 y, U32 font_size)
{
  G_FontAtlas *font = ctx->font;

  if (c < font->first_char) { return; }

  U8 idx = c - font->first_char;
  U8 px = idx % font->columns;
  U8 py = idx / font->columns;

  SDL_FRect src =
  {
    px * font->glyph_width,
    py * font->glyph_height,
    font->glyph_width,
    font->glyph_height
  };
  SDL_FRect dst = {x, y, font_size, font_size};

  SDL_RenderTexture(ctx->renderer, font->tex, &src, &dst);
}

internal void
g_draw_text(G_Context *ctx, String s, F32 x, F32 y, U32 font_size)
{
  F32 scale = (F32)font_size / (F32)ctx->font->glyph_width;
  F32 char_offset = ctx->font->kerning * scale + ctx->font->offset;

  F32 start_y = y;

  for (U64 idx = 0; idx < s.count; idx++)
  {
    y = start_y;
    g_draw_char(ctx, s.value[idx], x, y, font_size);
    y += font_size;
    x += char_offset;
  }
}

internal void
g_draw_circle(G_Context *ctx, I32 center_x, I32 center_y, I32 radius)
{
  int x = radius;
  int y = 0;
  int midpoint = 0;

  for (;x >= y;)
  {
    SDL_RenderPoint(ctx->renderer, center_x + x, center_y + y);
    SDL_RenderPoint(ctx->renderer, center_x + y, center_y + x);
    SDL_RenderPoint(ctx->renderer, center_x - y, center_y + x);
    SDL_RenderPoint(ctx->renderer, center_x - x, center_y + y);
    SDL_RenderPoint(ctx->renderer, center_x - x, center_y - y);
    SDL_RenderPoint(ctx->renderer, center_x - y, center_y - x);
    SDL_RenderPoint(ctx->renderer, center_x + y, center_y - x);
    SDL_RenderPoint(ctx->renderer, center_x + x, center_y - y);

    if (midpoint <= 0)
    {
      y++;
      midpoint += 2*y + 1;
    }
    else
    {
      x--;
      midpoint -= 2*x + 1;
    }
  }
}

internal void
g_draw_circle_ex(G_Context *ctx, I32 center_x, I32 center_y, U32 radius, U32 thickness)
{
  I32 outer_r = radius + thickness / 2;
  I32 inner_r = radius - thickness / 2;
  I32 outer_r2 = outer_r * outer_r;
  I32 inner_r2 = inner_r * inner_r;

  for (I32 y = -outer_r; y <= outer_r; y++)
  {
    I32 y2 = y * y;
    I32 outer_dx2 = outer_r2 - y2;
    if (outer_dx2 < 0)
    {
      continue;
    }
    I32 outer_dx = (I32)sqrtf((F32)outer_dx2);

    I32 inner_dx = 0;
    I32 inner_dx2 = inner_r2 - y2;
    if (inner_dx2 > 0)
    {
      inner_dx = (I32)sqrtf((F32)inner_dx2);
    }

    for (F32 x = inner_dx + 0.5f; x <= outer_dx; x++)
    {
      SDL_RenderPoint(ctx->renderer, center_x + x, center_y + y);
      SDL_RenderPoint(ctx->renderer, center_x - x, center_y + y);
    }
  }
}
