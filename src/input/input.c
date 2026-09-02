extern B32 running;

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
g_dispatch_event(G_InputState *input, SDL_Event *event)
{
  SDL_TextInputEvent text     = event->text;
  SDL_KeyboardEvent keyboard  = event->key;
  SDL_MouseButtonEvent mouse  = event->button;
  SDL_MouseWheelEvent wheel   = event->wheel;
  SDL_MouseMotionEvent motion = event->motion;
  SDL_WindowEvent window      = event->window;

  switch (event->type)
  {
    case SDL_EVENT_QUIT:
    {
      running = false;
    } break;

    case SDL_EVENT_WINDOW_RESIZED:
    {
      input->window_width_resized = window.data1;
      input->window_height_resized = window.data2;
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
