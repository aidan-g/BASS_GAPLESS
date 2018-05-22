#ifdef _DEBUG
#include <stdio.h>
#endif

#include "gapless_config.h"
#include "gapless_stream_event.h"

typedef struct
{
	HANDLE handle;
	GS_EVENT_ARGS args;
} GS_EVENT_HANDLER;

GSEVENTPROC* gapless_stream_event_handler = NULL;

DWORD WINAPI gapless_stream_event_raise_background(void* args) {
	BOOL success = TRUE;
	GS_EVENT_HANDLER* handler = args;
	if (handler) {
		if (gapless_stream_event_handler) {
			gapless_stream_event_handler(handler->args);
		}
		if (!CloseHandle(handler->handle)) {
#if _DEBUG
			printf("Failed to release thread.\n");
#endif
			success = FALSE;
		}
		else {
			handler->handle = NULL;
		}
		free(handler);
		handler = NULL;
	}
	return success;
}

BOOL gapless_stream_event_raise(GS_EVENT_ARGS args) {
	DWORD blocking_events = FALSE;
	GS_EVENT_HANDLER* handler;
	if (!gapless_stream_event_handler) {
		return FALSE;
	}
#if _DEBUG
	printf("Raising event: %d => %d => %d.\n", args.event_type, args.channel_1, args.channel_2);
#endif
	if (!gapless_config_get(BLOCKING_EVENTS, &blocking_events) || !blocking_events) {
		handler = malloc(sizeof(GS_EVENT_HANDLER));
		handler->args = args;
		handler->handle = CreateThread(NULL, 0, &gapless_stream_event_raise_background, handler, 0, NULL);
		if (!handler->handle) {
#if _DEBUG
			printf("Failed to create thread.\n");
#endif
			free(handler);
			handler = NULL;
			return FALSE;
		}
		return TRUE;
	}
	else {
		gapless_stream_event_handler(args);
		return TRUE;
	}
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
#if _DEBUG
	printf("Events enabled.\n");
#endif
	return TRUE;
}
BOOL gapless_stream_event_detach() {
	if (gapless_stream_event_is_enabled()) {
		gapless_stream_event_handler = NULL;
#if _DEBUG
		printf("Events disabled.\n");
#endif
	}
	return TRUE;
}
