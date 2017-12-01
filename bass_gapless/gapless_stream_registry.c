#include "gapless_stream_registry.h"
#include "gapless_stream_event.h"
#include "queue.h"

#define EMPTY_GAPLESS_STREAM 0

#define MAX_GAPLESS_STREAMS 10

QUEUE* queue;

DWORD current_stream = EMPTY_GAPLESS_STREAM;

BOOL gapless_stream_registry_create() {
	queue = queue_create(MAX_GAPLESS_STREAMS, TRUE);
	return TRUE;
}

BOOL gapless_stream_registry_enqueue(DWORD handle) {
	return queue_enqueue(queue, (void*)handle);
}

BOOL gapless_stream_registry_dequeue(DWORD* handle) {
	return queue_dequeue(queue, (void**)handle);
}

BOOL gapless_stream_registry_peek(DWORD* handle) {
	BOOL success = queue_peek(queue, (void**)handle);
	if (success) {
		if (*handle != current_stream) {
			GS_EVENT_ARGS event_args;
			event_args.event_type = GS_CHANGE;
			event_args.channel_1 = current_stream;
			event_args.channel_2 = *handle;
			gapless_stream_event_raise(event_args);
		}
	}
	return success;
}

BOOL gapless_stream_registry_remove(DWORD handle) {
	return queue_remove(queue, (void*)handle);
}

BOOL gapless_stream_registry_is_empty(BOOL* empty) {
	return queue_is_empty(queue, empty);
}

BOOL gapless_stream_registry_count(DWORD* length) {
	*length = queue->length;
	return TRUE;
}

DWORD* gapless_stream_registry_get_all(DWORD* length) {
	static DWORD buffer[MAX_GAPLESS_STREAMS];
	if (!queue_get_all(queue, (void**)buffer, length)) {
		*length = 0;
	}
	return buffer;
}

BOOL gapless_stream_registry_free() {
	queue_free(queue);
	return TRUE;
}