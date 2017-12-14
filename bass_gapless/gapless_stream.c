#ifdef _DEBUG
#include <stdio.h>
#endif

#include "gapless_config.h"
#include "gapless_stream.h"
#include "gapless_stream_registry.h"
#include "buffer.h"
#include "../bass/bass.h"

HSTREAM BASSGAPLESSDEF(gapless_stream_create)(DWORD freq, DWORD chans, DWORD flags, void *user) {
	HSTREAM result = BASS_StreamCreate(freq, chans, flags, &gapless_stream_bass_proc, user);
#if _DEBUG
	if (result)
	{
		printf("Created gapless channel: %d\n", result);
	}
	else {
		printf("Failed to create gapless channel.\n");
	}
#endif
	return result;
}

DWORD CALLBACK gapless_stream_bass_proc(HSTREAM handle, void *buffer, DWORD length, void *user) {
	return gapless_stream_proc(buffer, length);
}

DWORD gapless_stream_proc(void *buffer, DWORD length) {
	DWORD position = 0;
	DWORD remaining = length;
	DWORD handle;
	BOOL keep_alive;
	gapless_config_get(KEEP_ALIVE, &keep_alive);
	while (gapless_stream_registry_peek(&handle)) {
		if (BASS_ChannelIsActive(handle) == BASS_ACTIVE_PLAYING) {
			DWORD read = BASS_ChannelGetData(handle, offset_buffer(buffer, position), remaining);
			if (!read) {
				//Looks like there isn't enough space left for any more frames.
				//This seems to happen when playing DSD.
				break;
			}
			if (read == BASS_STREAMPROC_END) {
				//End of stream.
				goto dequeue;
			}
			if (read == BASS_ERROR_UNKNOWN) {
				//Some unknown error.
				goto dequeue;
			}
			position += read;
			remaining -= read;
			if (!remaining) {
				break;
			}
		}
		else {
		dequeue:
			gapless_stream_registry_dequeue(&handle);
		}
	}
	if (!position && !keep_alive) {
		position = BASS_STREAMPROC_END;
	}
	else if (position < length) {
#if _DEBUG
		printf("Buffer underrun while writing buffer.\n");
#endif
	}
	return position;
}