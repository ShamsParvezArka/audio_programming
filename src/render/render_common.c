#define AUDIO_SAMPLE_RATE 48000
#define NOTE_VOICE_GAIN 0.125f

extern B32 running;

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
  I16 buffer[4096 * 4];
  I32 n = additional_amount < 4096 * 4 ? additional_amount : 4096 * 4;

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
        note->phase += note->frequency * PI * 2.0f / AUDIO_SAMPLE_RATE;

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

internal void
g_update(G_Context *ctx, G_State *state, F32 delta_time)
{
  F32 s_pos_x = ctx->window_width / 2.0f - state->outer_boundary_width / 2.0f;
  F32 e_pos_x = s_pos_x + state->outer_boundary_width;
  if (state->line_pos_x1 >= e_pos_x)
  {
    state->line_pos_x1 = s_pos_x;
    state->line_pos_x2 = s_pos_x;
  }
  state->line_pos_x1 += 600.0f * delta_time;
  state->line_pos_x2 += 600.0f * delta_time;

  G_InputState *in = ctx->input;
  for (U64 idx = 0; idx < array_count(audio_note_lookup); idx++)
  {
    audio_note_lookup[idx].active = in->kbd_down[audio_note_lookup[idx].associated_key];
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

  for (U64 idx = 0; idx < 4; idx++)
  {
    F32 x = ctx->window_width / 2.0f - state->outer_boundary_width / 2.0f;
    F32 y = ctx->window_height / 2.0f - state->outer_boundary_height / 2.0f;
    F32 offset = state->outer_boundary_width / 4.0f;
    SDL_RenderLine(ctx->renderer,
                   x + (offset * idx),
                   y,
                   x + (offset * idx),
                   y + state->outer_boundary_height);
  }

  SDL_RenderLine(ctx->renderer,
                 state->line_pos_x1, state->line_pos_y1,
                 state->line_pos_x2, state->line_pos_y2);

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
