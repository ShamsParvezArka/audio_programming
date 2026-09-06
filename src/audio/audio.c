extern I32 octave_shift;

global AudioNote audio_note_lookup[13] = {};
global F32 audio_gain = 1.0f;

internal void
audio_note_lookup_init(void)
{
  audio_note_lookup[0]  = (AudioNote){G_InputMap_C,  AudioEventStage_Idle, 16.35f, 0.0f, 0.0f, 0, string_lit("NOTE C")};
  audio_note_lookup[1]  = (AudioNote){G_InputMap_Cs, AudioEventStage_Idle, 17.32f, 0.0f, 0.0f, 0, string_lit("NOTE C#")};
  audio_note_lookup[2]  = (AudioNote){G_InputMap_D,  AudioEventStage_Idle, 18.35f, 0.0f, 0.0f, 0, string_lit("NOTE D")};
  audio_note_lookup[3]  = (AudioNote){G_InputMap_Ds, AudioEventStage_Idle, 19.45f, 0.0f, 0.0f, 0, string_lit("NOTE D#")};
  audio_note_lookup[4]  = (AudioNote){G_InputMap_E,  AudioEventStage_Idle, 20.60f, 0.0f, 0.0f, 0, string_lit("NOTE E")};
  audio_note_lookup[5]  = (AudioNote){G_InputMap_F,  AudioEventStage_Idle, 21.83f, 0.0f, 0.0f, 0, string_lit("NOTE F")};
  audio_note_lookup[6]  = (AudioNote){G_InputMap_Fs, AudioEventStage_Idle, 23.12f, 0.0f, 0.0f, 0, string_lit("NOTE F#")};
  audio_note_lookup[7]  = (AudioNote){G_InputMap_G,  AudioEventStage_Idle, 24.50f, 0.0f, 0.0f, 0, string_lit("NOTE G")};
  audio_note_lookup[8]  = (AudioNote){G_InputMap_Gs, AudioEventStage_Idle, 25.96f, 0.0f, 0.0f, 0, string_lit("NOTE G#")};
  audio_note_lookup[9]  = (AudioNote){G_InputMap_A,  AudioEventStage_Idle, 27.50f, 0.0f, 0.0f, 0, string_lit("NOTE A")};
  audio_note_lookup[10] = (AudioNote){G_InputMap_As, AudioEventStage_Idle, 29.14f, 0.0f, 0.0f, 0, string_lit("NOTE A#")};
  audio_note_lookup[11] = (AudioNote){G_InputMap_B,  AudioEventStage_Idle, 30.87f, 0.0f, 0.0f, 0, string_lit("NOTE B")};
  audio_note_lookup[12] = (AudioNote){G_InputMap_Cn, AudioEventStage_Idle, 32.70f, 0.0f, 0.0f, 0, string_lit("NOTE C(UP)")};
};

internal void
audio_callback(void *userdata, SDL_AudioStream *stream, I32 additional_amount, I32 total_amount)
{
  additional_amount /= sizeof(I16);
  I16 buffer[4096];
  I32 n = additional_amount < 4096 ? additional_amount : 4096;
  F32 octave_mul = powf(2.0f, (F32)octave_shift);
  F32 octave_stabilizer = (octave_shift > 4) ? 0.7f : 1.0f;

  for (I32 i = 0; i < n; i++)
  {
    F32 mixed = 0;
    F32 active_count = 0;

    for (I32 k = 0; k < array_count(audio_note_lookup); k++)
    {
      AudioNote *note = &audio_note_lookup[k];

      if (note->active && note->stage == AudioEventStage_Idle)
      {
        note->stage = AudioEventStage_Attack;
      }
      else if (!note->active &&
               note->stage != AudioEventStage_Idle &&
               note->stage != AudioEventStage_Release)
      {
        note->stage = AudioEventStage_Release;
      }

      F32 target = 0.0f;
      F32 dt = 0.0f;
      switch (note->stage)
      {
        case AudioEventStage_Attack:
        {
          target = 1.0f;
          dt = AUDIO_ATTACK_RATE;
          if (abs(1.0f - note->amplitude) < AUDIO_EVENT_EPSILON)
          {
            note->stage = AudioEventStage_Decay;
          }
        } break;

        case AudioEventStage_Decay:
        {
          target = AUDIO_SUSTAIN_LEVEL;
          dt = AUDIO_DECAY_RATE;
          if (abs(AUDIO_SUSTAIN_LEVEL - note->amplitude) < AUDIO_EVENT_EPSILON)
          {
            note->stage = AudioEventStage_Sustain;
          }
        } break;

        case AudioEventStage_Sustain:
        {
          target = AUDIO_SUSTAIN_LEVEL;
        } break;

        case AudioEventStage_Release:
        {
          target = 0.0f;
          dt = AUDIO_RELEASE_RATE;
          if (note->amplitude < AUDIO_EVENT_EPSILON)
          {
            note->stage = AudioEventStage_Idle;
            note->amplitude = 0.0f;
          }
        } break;

        default:
        {
        } break;
      }
      note->amplitude = lerp(note->amplitude, target, dt);

      if (note->stage != AudioEventStage_Idle)
      {
        mixed += sinf(note->phase) * note->amplitude * NOTE_VOICE_GAIN * octave_stabilizer;
        note->phase += note->frequency * octave_mul * PI * 2.0f / AUDIO_SAMPLE_RATE;
        if (note->phase >= PI * 2.0f)
        {
          note->phase -= PI * 2.0f;
        }
      }
    }

    F32 sample = 32000 * mixed;
    buffer[i] = (I16)clamp(sample, -32000, 32000);
  }

  SDL_PutAudioStreamData(stream, buffer, n * sizeof(I16));
}
