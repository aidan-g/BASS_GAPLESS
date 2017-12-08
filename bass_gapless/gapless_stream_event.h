#include "bass_gapless.h"

BOOL gapless_stream_event_raise(GS_EVENT_ARGS event_args);

BOOL gapless_stream_event_is_enabled();

BOOL gapless_stream_event_attach(GSEVENTPROC* handler);

BOOL gapless_stream_event_detach();