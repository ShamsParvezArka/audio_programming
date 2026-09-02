#define AUDIO_SAMPLE_RATE 48000
#define NOTE_VOICE_GAIN 0.125f

extern B32 running;

global I32 octave_shift = 0.0;

typedef enum AudioChannel AudioChannel;
enum AudioChannel
{
  AudioChannel_Mono   = 1,
  AudioChannel_Stereo = 2
};

typedef struct AudioNote AudioNote;
struct AudioNote
{
  G_InputMap associated_key;
  F32 frequency;
  F32 phase;
  F32 amplitude;
  B32 active;
};

global AudioNote audio_note_lookup[13] =
{
  {G_InputMap_C4,  261.63f, 0.0f, 0.0f, 0},
  {G_InputMap_Cs4, 277.63f, 0.0f, 0.0f, 0},
  {G_InputMap_D4,  293.66f, 0.0f, 0.0f, 0},
  {G_InputMap_Ds4, 311.13f, 0.0f, 0.0f, 0},
  {G_InputMap_E4,  329.63f, 0.0f, 0.0f, 0},
  {G_InputMap_F4,  349.23f, 0.0f, 0.0f, 0},
  {G_InputMap_Fs4, 369.99f, 0.0f, 0.0f, 0},
  {G_InputMap_G4,  392.00f, 0.0f, 0.0f, 0},
  {G_InputMap_Gs4, 415.30f, 0.0f, 0.0f, 0},
  {G_InputMap_A4,  440.00f, 0.0f, 0.0f, 0},
  {G_InputMap_As4, 466.16f, 0.0f, 0.0f, 0},
  {G_InputMap_B4,  493.88f, 0.0f, 0.0f, 0},
  {G_InputMap_C5,  523.25f, 0.0f, 0.0f, 0}
};

internal void
audio_callback(void *userdata, SDL_AudioStream *stream, I32 additional_amount, I32 total_amount)
{
  additional_amount /= sizeof(I16);
  I16 buffer[4096];
  I32 n = additional_amount < 4096 ? additional_amount : 4096;
  F32 octave_mul = powf(2.0f, (F32)octave_shift);

  for (I32 i = 0; i < n; i++)
  {
    F32 mixed = 0;
    F32 active_count = 0;

    for (I32 k = 0; k < array_count(audio_note_lookup); k++)
    {
      AudioNote *note = &audio_note_lookup[k];
      F32 target = note->active ? 1.0f : 0.0f;
      note->amplitude = lerp(note->amplitude, target, 0.05f);

      if (note->amplitude > 0.0f || note->active)
      {
        mixed += sinf(note->phase) * note->amplitude * NOTE_VOICE_GAIN;
        note->phase += note->frequency *octave_mul * PI * 2.0f / AUDIO_SAMPLE_RATE;

        if (note->phase >= PI * 2.0f)
        {
          note->phase -= PI * 2.0f;
        }
      }
    }
    buffer[i] = (I16)(32000 * mixed);
  }
  SDL_PutAudioStreamData(stream, buffer, n * sizeof(I16));
}

internal void
g_reset_input_state(G_InputState *input)
{
  memory_zero(input->kbd_pressed, sizeof(input->kbd_pressed));
  memory_zero(input->kbd_released, sizeof(input->kbd_released));
  memory_zero(input->mouse_pressed, sizeof(input->mouse_pressed));
  memory_zero(input->mouse_released, sizeof(input->mouse_released));
  input->wheel = 0;
}

internal void
g_dispatch_event(G_Context *ctx, SDL_Event *event)
{
  SDL_TextInputEvent   text     = event->text;
  SDL_KeyboardEvent    keyboard = event->key;
  SDL_MouseButtonEvent mouse    = event->button;
  SDL_MouseWheelEvent  wheel    = event->wheel;
  SDL_MouseMotionEvent motion   = event->motion;
  SDL_WindowEvent      window   = event->window;

  G_InputState *input = ctx->input;

  switch (event->type)
  {
    case SDL_EVENT_QUIT:
    {
      running = false;
    } break;

    case SDL_EVENT_WINDOW_RESIZED:
    {
      ctx->window_width  = window.data1;
      ctx->window_height = window.data2;
    } break;

    ////////////////////////////////
    // NOTE: KEYBOARD EVENTS
    //
    case SDL_EVENT_KEY_DOWN:
    {
      U32 sc = keyboard.scancode;
      KeyboardDown(sc, input);
    } break;

    case SDL_EVENT_KEY_UP:
    {
      U32 sc = keyboard.scancode;
      KeyboardUp(sc, input);
    } break;

    ////////////////////////////////
    // NOTE: TEXT EVENTS
    //
    case SDL_EVENT_TEXT_INPUT:
    {
      input->ascii_char = *text.text;
    } break;

    ////////////////////////////////
    // NOTE: MOUSE EVENTS
    //
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    {
      U32 btn = mouse.button;
      MouseDown(btn, input);
    } break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
      U32 btn = mouse.button;
      MouseUp(btn, input);
    } break;

    case SDL_EVENT_MOUSE_WHEEL:
    {
      input->wheel += wheel.y;
    } break;

    case SDL_EVENT_MOUSE_MOTION:
    {
      input->mouse_x = motion.x;
      input->mouse_y = motion.y;
    } break;

    default:
    {

    } break;
  }
}

global F32 accumulated_time = 0.0f;
global F64 frame_count = 0;
global U64 smooth_fps = 0;

internal void
g_update(G_Context *ctx, G_State *state, F32 delta_time)
{
  accumulated_time += delta_time;
  frame_count++;
  if (accumulated_time >= 0.25)
  {
    smooth_fps = (U32)(ceil(frame_count / accumulated_time));
    accumulated_time = 0.0f;
    frame_count = 0;
  }

  state->outer_boundary_width = ctx->window_width * 0.8f;
  state->outer_boundary_height = ctx->window_height * 0.9f;

  F32 start_pos_x = ctx->window_width / 2.0f - state->outer_boundary_width / 2.0f;
  F32 end_pos_x = start_pos_x + state->outer_boundary_width;
  if (state->line_pos_x1 >= end_pos_x)
  {
    state->line_pos_x1 = start_pos_x;
    state->line_pos_x2 = start_pos_x;
  }
  state->line_pos_x1 += 600.0f * delta_time;
  state->line_pos_x2 += 600.0f * delta_time;
  state->line_pos_y1 = ctx->window_height / 2.0f - state->outer_boundary_height / 2.0f;
  state->line_pos_y2 = state->line_pos_y1 + state->outer_boundary_height;

  G_InputState *in = ctx->input;
  if (in->kbd_down[G_InputMap_OctaveUpShifter])
  {
    octave_shift = 1;
  }
  else
  {
    octave_shift = 0;
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
  ////////////////////////////////
  // NOTE: G_ rendering
  //
  SDL_SetRenderDrawColor(ctx->renderer, ctx->config->r, ctx->config->g, ctx->config->b, ctx->config->a);
  SDL_RenderClear(ctx->renderer);

  SDL_FRect outer_boundary = {
    .x = ctx->window_width / 2.0f - state->outer_boundary_width / 2.0f,
    .y = ctx->window_height / 2.0f - state->outer_boundary_height / 2.0f,
    .w = state->outer_boundary_width,
    .h = state->outer_boundary_height
  };

  SDL_SetRenderDrawColor(ctx->renderer, 255.0f, 255.0f, 255.0f, 255.0f);
  SDL_RenderRect(ctx->renderer, &outer_boundary);

  SDL_SetRenderDrawColor(ctx->renderer, 255.0f, 255.0f, 255.0f, 100.0f);
  for (U64 idx = 0; idx < 4; idx++)
  {
    F32 x = ctx->window_width / 2.0f - state->outer_boundary_width / 2.0f;
    F32 y = ctx->window_height / 2.0f - state->outer_boundary_height / 2.0f;
    F32 offset = state->outer_boundary_width / 4.0f;
    SDL_RenderLine(ctx->renderer,
                   x + (offset * idx), y,
                   x + (offset * idx), y + state->outer_boundary_height);
  }

  SDL_RenderLine(ctx->renderer,
                 state->line_pos_x1, state->line_pos_y1,
                 state->line_pos_x2, state->line_pos_y2);


  for (U64 idx = 0; idx < state->circle_count; idx++)
  {
    F32 t = state->circle_lifetime[idx] / MAX_CIRCLE_LIFETIME;
    F32 expand_coefficient = 1.0f + t;
    F32 opacity = lerp(255.0f, 0.0f, t);
    F32 radius = 20.0f;

    if (idx % 2 == 0)
    {
      SDL_SetRenderDrawColor(ctx->renderer, 9.0f, 90.0f, 177.0f, opacity);
    }
    else
    {
      SDL_SetRenderDrawColor(ctx->renderer, 162.0f, 53.0f, 55.0f, opacity);
    }

    g_draw_circle_ex(ctx,
                     state->circle_x[idx] - radius, state->circle_y[idx],
                     radius * expand_coefficient,
                     10.0f);
  }

#define FONT_TEST 0
#if FONT_TEST
  F32 font_size = 22.0f;
  G_DrawText(ctx,
             string_lit("The quick borwn fox jumps over the lazy dog"),
             20.0f, 20.0f,
             font_size);
  G_DrawText(ctx,
             string_lit("1234567890!@#$%^&*()-+_/<>:\"\\|~`"),
             20.0f, 70.0f,
             font_size);
#endif

  ////////////////////////////////
  // NOTE: UI rendering
  //
  ui_begin_panel(ui, ctx, string_lit("debug panel"), 30.0f, 30.0f);
  {
    ui_label(ui, ctx, string_lit(c_str_fmt("delta: %.4f", delta_time)));
    ui_label(ui, ctx, string_lit(c_str_fmt("fps: %d", smooth_fps)));
    if (octave_shift > 0)
    {
      ui_label(ui, ctx, string_lit(c_str_fmt("Octave: +%d", octave_shift)));
    }
    else
    {
      ui_label(ui, ctx, string_lit(c_str_fmt("Octave: %d", octave_shift)));
    }

  }
  ui_end_panel(ui, ctx);


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
  FontAtlas *font = ctx->font;

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

  while (x >= y)
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
g_draw_circle_ex(G_Context *ctx, I32 center_x, I32 center_y, I32 radius, I32 thickness)
{
  I32 outer_r = radius + thickness / 2;
  I32 inner_r = radius - thickness / 2;
  if (inner_r < 0)
  {
    inner_r = 0;
  }

  I32 outer_r2 = outer_r * outer_r;
  I32 inner_r2 = inner_r * inner_r;

  for (I32 y = -outer_r; y <= outer_r; y++)
  {
    I32 y2 = y * y;

    // outer boundary at this row
    I32 outer_dx2 = outer_r2 - y2;
    if (outer_dx2 < 0) continue;
    I32 outer_dx = (I32)sqrtf((F32)outer_dx2);

    // inner boundary at this row (0 if this row is above/below the inner circle entirely)
    I32 inner_dx = 0;
    I32 inner_dx2 = inner_r2 - y2;
    if (inner_dx2 > 0)
    {
      inner_dx = (I32)sqrtf((F32)inner_dx2);
    }

    // right side of the ring: from inner_dx to outer_dx
    for (I32 x = inner_dx; x <= outer_dx; x++)
    {
      SDL_RenderPoint(ctx->renderer, center_x + x, center_y + y);
      SDL_RenderPoint(ctx->renderer, center_x - x, center_y + y);
    }
  }
}
