#include "bass_gapless.h"
#include "gapless_stream.h"
#include "gapless_stream_registry.h"
#include "gapless_stream_event.h"

BOOL is_initialized = FALSE;

BOOL BASSGAPLESSDEF(BASS_GAPLESS_Init)() {
	if (is_initialized) {
		return FALSE;
	}
	if (!gapless_stream_registry_create()) {
		return FALSE;
	}
	is_initialized = TRUE;
	return TRUE;
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_Free)() {
	if (!is_initialized) {
		return FALSE;
	}
	if (!gapless_stream_registry_free()) {
		return FALSE;
	}
	if (!gapless_stream_event_detach()) {
		return FALSE;
	}
	is_initialized = FALSE;
	return TRUE;
}

HSTREAM BASSGAPLESSDEF(BASS_GAPLESS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, void *user) {
	if (!is_initialized) {
		return 0;
	}
	return BASS_StreamCreate(freq, chans, flags, &gapless_stream_bass_proc, user);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelEnqueue)(HSTREAM handle) {
	if (!is_initialized) {
		return FALSE;
	}
	return gapless_stream_registry_enqueue(handle);
}

DWORD* BASSGAPLESSDEF(BASS_GAPLESS_GetChannels)(DWORD *count) {
	if (!is_initialized) {
		return NULL;
	}
	return gapless_stream_registry_get_all(count);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelRemove)(DWORD handle) {
	if (!is_initialized) {
		return FALSE;
	}
	return gapless_stream_registry_remove(handle);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_EnableEvents)(GSEVENTPROC* handler) {
	if (!is_initialized) {
		return FALSE;
	}
	return gapless_stream_event_attach(handler);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_DisableEvents)() {
	if (!is_initialized) {
		return FALSE;
	}
	return gapless_stream_event_detach();
}