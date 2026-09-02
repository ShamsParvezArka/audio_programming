extern I32 octave_shift;

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
