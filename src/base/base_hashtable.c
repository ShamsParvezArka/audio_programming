internal U64
djb2_hash(String key)
{
  U64 hash = 5381;

  for (U64 idx = 0; idx < key.count; idx++)
  {
    hash = ((hash << 5) + hash) + key.value[idx]; /* hash * 33 + c */
  }

  return hash;
}
