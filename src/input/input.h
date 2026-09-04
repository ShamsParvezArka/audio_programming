#ifndef INPUT_H
#define INPUT_H

#define KeyboardDown(sc, in) do { (in)->kbd_down[(sc)] = true; if (!(in)->kbd_pressed[(sc)]) (in)->kbd_pressed[(sc)] = true; } while (0)
#define KeyboardUp(sc, in)   do { (in)->kbd_down[(sc)] = false; (in)->kbd_released[(sc)] = true; } while (0)
#define MouseDown(btn, in)   do { (in)->mouse_down[(btn)] = true; if (!(in)->mouse_pressed[(btn)]) (in)->mouse_pressed[(btn)] = true; } while (0)
#define MouseUp(btn, in)     do { (in)->mouse_down[(btn)] = false; (in)->mouse_released[(btn)] = true; } while (0)

typedef struct G_InputState G_InputState;
struct G_InputState
{
  F32 mouse_x;
  F32 mouse_y;
  F32 wheel;

  I32 window_width_resized;
  I32 window_height_resized;

  B32 kbd_down[SDL_SCANCODE_COUNT];
  B32 kbd_pressed[SDL_SCANCODE_COUNT];
  B32 kbd_released[SDL_SCANCODE_COUNT];

  B32 mouse_down[5];
  B32 mouse_pressed[5];
  B32 mouse_released[5];

  U8 ascii_char;
};

typedef enum G_InputMap G_InputMap;
enum G_InputMap
{
  G_InputMap_C  = SDL_SCANCODE_Z,
  G_InputMap_Cs = SDL_SCANCODE_S,
  G_InputMap_D  = SDL_SCANCODE_X,
  G_InputMap_Ds = SDL_SCANCODE_D,
  G_InputMap_E  = SDL_SCANCODE_C,
  G_InputMap_F  = SDL_SCANCODE_V,
  G_InputMap_Fs = SDL_SCANCODE_G,
  G_InputMap_G  = SDL_SCANCODE_B,
  G_InputMap_Gs = SDL_SCANCODE_H,
  G_InputMap_A  = SDL_SCANCODE_N,
  G_InputMap_As = SDL_SCANCODE_J,
  G_InputMap_B  = SDL_SCANCODE_M,
  G_InputMap_Cn = SDL_SCANCODE_COMMA,
  G_InputMap_OctaveUpShifter   = SDL_SCANCODE_BACKSLASH,
  G_InputMap_OctaveDownShifter = SDL_SCANCODE_RETURN
};

internal void g_reset_input_state(G_InputState *input);
internal void g_dispatch_event(G_InputState *input, SDL_Event *event);

#endif
