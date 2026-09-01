#ifndef BASE_PROFILE_H
#define BASE_PROFILE_H

#include "tracy/TracyC.h"

#define prof_scope(name)                                                \
  local_persist struct ___tracy_source_location_data glue(_tz_loc_, __LINE__) = { name, __func__, __FILE__, (U32)__LINE__, 0 }; \
  TracyCZoneCtx glue(_tz_ctx_, __LINE__);                               \
  defer_scope(                                                          \
    (glue(_tz_ctx_, __LINE__) = ___tracy_emit_zone_begin(&glue(_tz_loc_, __LINE__), 1)), \
    ___tracy_emit_zone_end(glue(_tz_ctx_, __LINE__))                    \
    )

#endif
