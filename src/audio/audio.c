extern I32 octave_shift;

internal void
audio_note_lookup_init(void)
{
  audio_note_lookup[0]  = (AudioNote){G_InputMap_C,  16.35f, 0.0f, 0.0f, 0, string_lit("NOTE C")};
  audio_note_lookup[1]  = (AudioNote){G_InputMap_Cs, 17.32f, 0.0f, 0.0f, 0, string_lit("NOTE C#")};
  audio_note_lookup[2]  = (AudioNote){G_InputMap_D,  18.35f, 0.0f, 0.0f, 0, string_lit("NOTE D")};
  audio_note_lookup[3]  = (AudioNote){G_InputMap_Ds, 19.45f, 0.0f, 0.0f, 0, string_lit("NOTE D#")};
  audio_note_lookup[4]  = (AudioNote){G_InputMap_E,  20.60f, 0.0f, 0.0f, 0, string_lit("NOTE E")};
  audio_note_lookup[5]  = (AudioNote){G_InputMap_F,  21.83f, 0.0f, 0.0f, 0, string_lit("NOTE F")};
  audio_note_lookup[6]  = (AudioNote){G_InputMap_Fs, 23.12f, 0.0f, 0.0f, 0, string_lit("NOTE F#")};
  audio_note_lookup[7]  = (AudioNote){G_InputMap_G,  24.50f, 0.0f, 0.0f, 0, string_lit("NOTE G")};
  audio_note_lookup[8]  = (AudioNote){G_InputMap_Gs, 25.96f, 0.0f, 0.0f, 0, string_lit("NOTE G#")};
  audio_note_lookup[9]  = (AudioNote){G_InputMap_A,  27.50f, 0.0f, 0.0f, 0, string_lit("NOTE A")};
  audio_note_lookup[10] = (AudioNote){G_InputMap_As, 29.14f, 0.0f, 0.0f, 0, string_lit("NOTE A#")};
  audio_note_lookup[11] = (AudioNote){G_InputMap_B,  30.87f, 0.0f, 0.0f, 0, string_lit("NOTE B")};
  audio_note_lookup[12] = (AudioNote){G_InputMap_Cn, 32.70f, 0.0f, 0.0f, 0, string_lit("NOTE C(UP)")};
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
        note->phase += note->frequency * octave_mul * PI * 2.0f / AUDIO_SAMPLE_RATE;

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
