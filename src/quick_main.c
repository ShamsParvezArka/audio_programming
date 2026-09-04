#ifndef USE_VIRTUAL_RESOLUTION
#  define USE_VIRTUAL_RESOLUTION false
#endif

global B32 running = true;
global F32 time = 0;

internal void
quick_entry_point(void)
{
  SDL_Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));

  Arena *game_memory = arena_alloc(MB(50));

  G_Config *game_config = arena_push_struct(game_memory, G_Config);
  game_config->r = 24;
  game_config->g = 24;
  game_config->b = 24;
  game_config->a = 255;

  G_Context *ctx = arena_push_struct(game_memory, G_Context);

  ctx->config = game_config;
  ctx->window_title  = "Demo Window";
  ctx->window_width  = (F32)(GetSystemMetrics(SM_CXSCREEN) * 0.7f);
  ctx->window_height = (F32)(GetSystemMetrics(SM_CYSCREEN) * 0.7f);

  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
  SDL_CreateWindowAndRenderer(ctx->window_title,
                              ctx->window_width,
                              ctx->window_height,
                              flags,
                              &ctx->window,
                              &ctx->renderer);
  SDL_SetRenderVSync(ctx->renderer, 1);
  SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);

  I32 actual_window_width = 0;
  I32 actual_window_height = 0;
  SDL_GetWindowSize(ctx->window, &actual_window_width, &actual_window_height);
  ctx->window_width = (F32)actual_window_width;
  ctx->window_height = (F32)actual_window_height;

#if USE_VIRTUAL_RESOLUTION
  I32 virtual_width = 640;
  I32 virtual_height = 480;
  SDL_SetRenderLogicalPresentation(ctx->renderer,
                                   virtual_width,
                                   virtual_height,
                                   SDL_LOGICAL_PRESENTATION_STRETCH);
#endif // USE_VIRTUAL_RESOLUTION

  ctx->input = arena_push_struct(game_memory, G_InputState);;
  SDL_Require(SDL_StartTextInput(ctx->window));

  ctx->font = arena_push_struct(game_memory, G_FontAtlas);
  ctx->font->tex = IMG_LoadTexture(ctx->renderer, "../assets/proggy.png");
  SDL_Require(ctx->font->tex);
  ctx->font->glyph_width = 32;
  ctx->font->glyph_height = 32;
  ctx->font->columns = 16;
  ctx->font->first_char = 32;
  ctx->font->kerning = 14;
  ctx->font->offset = 3;
  SDL_SetTextureScaleMode(ctx->font->tex, SDL_SCALEMODE_NEAREST);

  ctx->keymap_spritesheet = IMG_LoadTexture(ctx->renderer, "../assets/keymap.png");
  ctx->keymap_width = 48;
  ctx->keymap_height = 48;

  UI_Context *ui = arena_push_struct(game_memory, UI_Context);
  ui->font = ctx->font ;
  ui_init_style(ui);

  G_State *state = arena_push_struct(game_memory, G_State);
  state->camera = arena_push_struct(game_memory, G_Camera);
  state->camera->zoom = 1.0f;
  state->camera->zoom_target = 1.0f;

  state->outer_boundary_width = ctx->window_width * 0.8f;
  state->outer_boundary_height = ctx->window_height * 0.9f;
  state->line_pos_x1 = ctx->window_width / 2.0f - state->outer_boundary_width / 2.0f;
  state->line_pos_y1 = ctx->window_height / 2.0f - state->outer_boundary_height / 2.0f;
  state->line_pos_x2 = state->line_pos_x1;
  state->line_pos_y2 = state->line_pos_y1 + state->outer_boundary_height;

  ctx->audio_spec = arena_push_struct(game_memory, SDL_AudioSpec);
  ctx->audio_spec->format = SDL_AUDIO_S16;
  ctx->audio_spec->freq = AUDIO_SAMPLE_RATE;
  ctx->audio_spec->channels = AudioChannel_Mono;

  audio_note_lookup_init();
  SDL_AudioStream *audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                                            ctx->audio_spec,
                                                            audio_callback,
                                                            0);
  SDL_ResumeAudioStreamDevice(audio_stream);

  while (running)
  {
    g_reset_input_state(ctx->input);
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      g_dispatch_event(ctx->input, &event);
    }

    local_persist U64 last_frame_time = 0;
    U64 current_frame_time = SDL_GetPerformanceCounter();
    F32 dt = (F32)(current_frame_time - last_frame_time) / (F32)SDL_GetPerformanceFrequency();
    last_frame_time = current_frame_time;

    ui_begin_frame(ui, ctx);
    g_update(ctx, state, dt);
    g_render(ctx, state, ui, dt);
    ui_end_frame(ui);
  }

  SDL_StopTextInput(ctx->window);
  g_quit(ctx);
  arena_release(game_memory);
}
