#include "gapless_stream_event.h"

GS_EVENT_ARGS gapless_stream_event_args_none;
BOOL gapless_stream_event_shutdown = FALSE;
volatile BOOL gapless_stream_event_active = FALSE;
HANDLE gapless_stream_event_thread_handle = NULL;
HANDLE gapless_stream_event_lock_handle = NULL;
GS_EVENT_ARGS gapless_stream_event_args;
GSEVENTPROC* gapless_stream_event_handler = NULL;

BOOL gapless_stream_event_enter() {
	DWORD result;
	if (!gapless_stream_event_lock_handle) {
		return TRUE;
	}
	result = WaitForSingleObject(gapless_stream_event_lock_handle, INFINITE);
	if (result != WAIT_OBJECT_0) {
		return FALSE;
	}
	return TRUE;
}

void gapless_stream_event_exit() {
	if (!gapless_stream_event_lock_handle) {
		return;
	}
	ReleaseSemaphore(gapless_stream_event_lock_handle, 1, NULL);
}

DWORD WINAPI gapless_stream_event_update(void* args) {
	if (gapless_stream_event_shutdown || !gapless_stream_event_lock_handle) {
		return FALSE;
	}
	gapless_stream_event_active = TRUE;
	while (!gapless_stream_event_shutdown) {
		if (!gapless_stream_event_enter()) {
			continue;
		}
		if (gapless_stream_event_handler && gapless_stream_event_args.event_type != GS_NONE) {
			gapless_stream_event_handler(gapless_stream_event_args);
			gapless_stream_event_args.event_type = GS_NONE;
		}
	}
	return TRUE;
}

BOOL gapless_stream_event_begin() {
	if (gapless_stream_event_active) {
		return TRUE;
	}
	gapless_stream_event_shutdown = FALSE;
	gapless_stream_event_lock_handle = CreateSemaphore(NULL, 1, 1, NULL);
	if (!gapless_stream_event_lock_handle) {
		return FALSE;
	}
	if (!gapless_stream_event_enter()) {
		return FALSE;
	}
	gapless_stream_event_thread_handle = CreateThread(NULL, 0, gapless_stream_event_update, NULL, 0, NULL);
	if (!gapless_stream_event_thread_handle) {
		return FALSE;
	}
	return TRUE;
}

BOOL  gapless_stream_event_end() {
	gapless_stream_event_shutdown = TRUE;
	gapless_stream_event_exit();
	if (!gapless_stream_event_thread_handle) {
		return TRUE;
	}
	else {
		WaitForSingleObject(gapless_stream_event_thread_handle, INFINITE);
		CloseHandle(gapless_stream_event_thread_handle);
		CloseHandle(gapless_stream_event_lock_handle);
		return TRUE;
	}
}

BOOL gapless_stream_event_is_enabled() {
	return gapless_stream_event_active;
}

BOOL gapless_stream_event_raise(GS_EVENT_ARGS args) {
	if (!gapless_stream_event_handler) {
		return FALSE;
	}
	if (args.event_type == GS_NONE) {
		return FALSE;
	}
	gapless_stream_event_args = args;
	gapless_stream_event_exit();
	return TRUE;
}

BOOL gapless_stream_event_attach(GSEVENTPROC* handler) {
	gapless_stream_event_handler = handler;
	if (!gapless_stream_event_begin()) {
		return FALSE;
	}
	return TRUE;
}