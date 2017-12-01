#include <windows.h>

typedef enum {
	GS_NONE = 0,
	GS_START = 1,
	GS_CHANGE = 2,
	GS_END = 3
} GS_EVENT_TYPE;

typedef struct {
	GS_EVENT_TYPE event_type;
	DWORD channel_1;
	DWORD channel_2;
} GS_EVENT_ARGS;

typedef DWORD(CALLBACK GSEVENTPROC)(GS_EVENT_ARGS args);

BOOL  gapless_stream_event_begin();

BOOL  gapless_stream_event_end();

BOOL gapless_stream_event_raise(GS_EVENT_ARGS event_args);

BOOL gapless_stream_event_attach(GSEVENTPROC* handler);