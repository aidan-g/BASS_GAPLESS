#include "bass_gapless.h"
#include "gapless_stream.h"
#include "gapless_stream_registry.h"
#include "gapless_stream_event.h"

BOOL BASSGAPLESSDEF(BASS_GAPLESS_Init)() {
	if (!gapless_stream_registry_create()) {
		return FALSE;
	}
	return TRUE;
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_Free)() {
	if (!gapless_stream_registry_free()) {
		return FALSE;
	}
	if (!gapless_stream_event_end()) {
		return FALSE;
	}
	return TRUE;
}

HSTREAM BASSGAPLESSDEF(BASS_GAPLESS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, void *user) {
	return BASS_StreamCreate(freq, chans, flags, &gapless_stream_bass_proc, user);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelEnqueue)(HSTREAM handle) {
	return gapless_stream_registry_enqueue(handle);
}

DWORD* BASSGAPLESSDEF(BASS_GAPLESS_GetChannels)(DWORD *count) {
	return gapless_stream_registry_get_all(count);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelRemove)(DWORD handle) {
	return gapless_stream_registry_remove(handle);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_EnableEvents)(GSEVENTPROC* handler) {
	return gapless_stream_event_attach(handler);
}