#include "gapless_stream.h"
#include "gapless_stream_registry.h"
#include "buffer.h"
#include "../bass/bass.h"

HSTREAM BASSGAPLESSDEF(gapless_stream_create)(DWORD freq, DWORD chans, DWORD flags, void *user) {
	return BASS_StreamCreate(freq, chans, flags, &gapless_stream_bass_proc, user);
}

DWORD CALLBACK gapless_stream_bass_proc(HSTREAM handle, void *buffer, DWORD length, void *user) {
	return gapless_stream_proc(buffer, length);
}

DWORD gapless_stream_proc(void *buffer, DWORD length) {
	DWORD position = 0;
	DWORD remaining = length;
	DWORD handle;
	while (gapless_stream_registry_peek(&handle)) {
		if (BASS_ChannelIsActive(handle) == BASS_ACTIVE_PLAYING) {
			position += BASS_ChannelGetData(handle, offset_buffer(buffer, position), remaining);
			remaining = length - position;
			if (!remaining) {
				break;
			}
		}
		else {
			gapless_stream_registry_dequeue(&handle);
		}
	}
	if (!position) {
		position = BASS_STREAMPROC_END;
	}
	return position;
}