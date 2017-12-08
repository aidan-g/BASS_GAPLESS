#include "gapless_stream_event.h"

typedef struct
{
	HANDLE thread_handle;
	GS_EVENT_ARGS args;
} GS_EVENT_HANDLER;

GSEVENTPROC* gapless_stream_event_handler = NULL;

DWORD WINAPI gapless_stream_event_background_raise(void* args) {
	BOOL success = TRUE;
	GS_EVENT_HANDLER* handler = args;
	if (handler) {
		if (gapless_stream_event_handler) {
			gapless_stream_event_handler(handler->args);
		}
		success &= CloseHandle(handler->thread_handle);
		free(handler);
	}
	return success;
}

BOOL gapless_stream_event_raise(GS_EVENT_ARGS args) {
	GS_EVENT_HANDLER* handler = malloc(sizeof(GS_EVENT_HANDLER));
	if (!gapless_stream_event_handler) {
		return FALSE;
	}
	handler->args = args;
	handler->thread_handle = CreateThread(NULL, 0, gapless_stream_event_background_raise, handler, 0, NULL);
	if (!handler->thread_handle) {
		free(handler);
		return FALSE;
	}
	return TRUE;
}

BOOL gapless_stream_event_is_enabled() {
	if (!gapless_stream_event_handler) {
		return FALSE;
	}
	return TRUE;
}

BOOL gapless_stream_event_attach(GSEVENTPROC* handler) {
	if (gapless_stream_event_is_enabled() && !gapless_stream_event_detach()) {
		return FALSE;
	}
	gapless_stream_event_handler = handler;
	return TRUE;
}
BOOL gapless_stream_event_detach() {
	if (gapless_stream_event_is_enabled()) {
		//TODO: This triggers a breakpoint?
		//free(gapless_stream_event_handler); 
		gapless_stream_event_handler = NULL;
	}
	return TRUE;
}
