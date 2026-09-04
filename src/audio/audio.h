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
  String note_name;
};

global AudioNote audio_note_lookup[13] = {};

internal void audio_note_lookup_init(void);
internal void audio_callback(void *userdata, SDL_AudioStream *stream, I32 additional_amount, I32 total_amount);

#endif
