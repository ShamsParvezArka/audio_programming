internal void
win32_spawn_console()
{
  AllocConsole();
  FILE *buffer;
  freopen_s(&buffer, "CONOUT$", "w", stdout);
  freopen_s(&buffer, "CONOUT$", "w", stderr);
  freopen_s(&buffer, "CONIN$",  "r", stdin);
}

internal DateTime
date_time_from_unix_time(U64 time)
{
  DateTime result = {};
  result.micro_sec = time%1000;
  time /= 1000;
  result.milli_sec = time%1000;
  time /= 1000;
  result.sec = time%60;
  time /= 60;
  result.min = time%60;
  time /= 60;
  result.hour = time%24;
  time /= 24;
  result.day = time%31;
  time /= 31;
  result.mon = time%12;
  time /= 12;
  ////////////////////////////
  //~ TODO(arka): make this bullet proof
  // Assert(time <= max_U32);
  result.year = (U32)time;

  return result;
}
