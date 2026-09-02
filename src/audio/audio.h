#ifndef AUDIO_H
#define AUDIO_H

#define AUDIO_SAMPLE_RATE 48000
#define NOTE_VOICE_GAIN 0.125f

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

internal void audio_callback(void *userdata, SDL_AudioStream *stream, I32 additional_amount, I32 total_amount);

#endif
