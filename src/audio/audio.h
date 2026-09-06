#ifndef AUDIO_H
#define AUDIO_H

#define AUDIO_SAMPLE_RATE 48000
#define NOTE_VOICE_GAIN 0.125f

#define AUDIO_ATTACK_RATE   0.008f
#define AUDIO_RELEASE_RATE  0.004f
#define AUDIO_DECAY_RATE    0.01f
#define AUDIO_SUSTAIN_LEVEL 0.7f
#define AUDIO_EVENT_EPSILON 0.001f

typedef enum AudioChannel AudioChannel;
enum AudioChannel
{
  AudioChannel_Mono   = 1,
  AudioChannel_Stereo = 2
};

typedef enum AudioEventStage AudioEventStage;
enum AudioEventStage
{
  AudioEventStage_Idle,
  AudioEventStage_Attack,
  AudioEventStage_Decay,
  AudioEventStage_Sustain,
  AudioEventStage_Release
};

typedef struct AudioNote AudioNote;
struct AudioNote
{
  G_InputMap associated_key;
  AudioEventStage stage;
  F32 frequency;
  F32 phase;
  F32 amplitude;
  B32 active;
  String note_name;
};

internal void audio_note_lookup_init(void);
internal void audio_callback(void *userdata, SDL_AudioStream *stream, I32 additional_amount, I32 total_amount);

#endif
