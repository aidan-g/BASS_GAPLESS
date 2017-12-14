#ifdef _DEBUG
#include <stdio.h>
#endif

#include "gapless_stream_registry.h"
#include "gapless_stream_event.h"
#include "queue.h"

#define EMPTY_GAPLESS_STREAM 0

#define MAX_GAPLESS_STREAMS 10

QUEUE* queue = NULL;

DWORD current_stream = EMPTY_GAPLESS_STREAM;

BOOL gapless_stream_registry_create() {
	if (queue) {
		return FALSE;
	}
	queue = queue_create(MAX_GAPLESS_STREAMS, TRUE);
	if (!queue) {
#if _DEBUG
		printf("Failed to create queue.\n");
#endif
		return FALSE;
	}
	return TRUE;
}

BOOL gapless_stream_registry_enqueue(DWORD handle) {
	if (!queue) {
		return FALSE;
	}
#if _DEBUG
	printf("Enqueuing channel: %d\n", handle);
#endif
	return queue_enqueue(queue, (void*)handle);
}

BOOL gapless_stream_registry_dequeue(DWORD* handle) {
	if (!queue) {
		return FALSE;
	}
#if _DEBUG
	printf("Dequeuing channel: %d\n", handle);
#endif
	return queue_dequeue(queue, (void**)handle);
}

void gapless_steam_registry_raise(BOOL success, DWORD handle) {
	GS_EVENT_ARGS event_args;
	if (!gapless_stream_event_is_enabled())
	{
		return;
	}
	if (success) {
		if (handle != current_stream) {
			if (current_stream) {
				event_args.event_type = GS_CHANGE;
				event_args.channel_1 = current_stream;
				event_args.channel_2 = handle;
			}
			else {
				event_args.event_type = GS_START;
				event_args.channel_1 = handle;
				event_args.channel_2 = EMPTY_GAPLESS_STREAM;
			}
			gapless_stream_event_raise(event_args);
			current_stream = handle;
		}
	}
	else if (current_stream) {
		event_args.event_type = GS_END;
		event_args.channel_1 = current_stream;
		event_args.channel_2 = EMPTY_GAPLESS_STREAM;
		gapless_stream_event_raise(event_args);
		current_stream = EMPTY_GAPLESS_STREAM;
	}
}

BOOL gapless_stream_registry_peek(DWORD* handle) {
	BOOL success;
	if (!queue) {
		return FALSE;
	}
	success = queue_peek(queue, (void**)handle);
	gapless_steam_registry_raise(success, *handle);
	return success;
}

BOOL gapless_stream_registry_remove(DWORD handle) {
	if (!queue) {
		return FALSE;
	}
#if _DEBUG
	printf("Removing channel: %d\n", handle);
#endif
	return queue_remove(queue, (void*)handle);
}

BOOL gapless_stream_registry_is_empty(BOOL* empty) {
	if (!queue) {
		return FALSE;
	}
	return queue_is_empty(queue, empty);
}

BOOL gapless_stream_registry_count(DWORD* length) {
	if (!queue) {
		return FALSE;
	}
	*length = queue->length;
	return TRUE;
}

DWORD* gapless_stream_registry_get_all(DWORD* length) {
	static DWORD buffer[MAX_GAPLESS_STREAMS];
	if (!queue) {
		return NULL;
	}
	if (!queue_get_all(queue, (void**)buffer, length)) {
		*length = 0;
	}
	return buffer;
}

BOOL gapless_stream_registry_free() {
	if (!queue) {
		return FALSE;
	}
	queue_free(queue);
	queue = NULL;
	return TRUE;
}