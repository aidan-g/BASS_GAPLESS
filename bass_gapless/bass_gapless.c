#ifdef _DEBUG
#include <stdio.h>
#endif

#include "bass_gapless.h"
#include "gapless_config.h"
#include "gapless_stream.h"
#include "gapless_stream_registry.h"
#include "gapless_stream_event.h"

BOOL is_initialized = FALSE;

//I have no idea how to prevent linking against this routine in msvcrt.
//It doesn't exist on Windows XP.
//Hopefully it doesn't do anything important.
int _except_handler4_common() {
	return 0;
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_Init)() {
	if (is_initialized) {
		return FALSE;
	}
	if (!gapless_stream_registry_create()) {
		return FALSE;
	}
	is_initialized = TRUE;
#if _DEBUG
	printf("BASS GAPLESS initialized.\n");
#endif
	return TRUE;
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_Free)() {
	BOOL success = TRUE;
	if (!is_initialized) {
		success = FALSE;
	}
	else {
		success &= gapless_stream_registry_free();
		success &= gapless_stream_event_detach();
	}
	if (success) {
		is_initialized = FALSE;
#if _DEBUG
		printf("BASS GAPLESS released.\n");
#endif
	}
	else {
#if _DEBUG
		printf("Failed to release BASS GAPLESS.\n");
#endif
	}
	return success;
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_SetConfig)(GS_ATTRIBUTE attrib, DWORD value) {
	return gapless_config_set(attrib, value);
}

BOOL BASSGAPLESSDEF(BASS_GAPLESS_GetConfig)(GS_ATTRIBUTE attrib, DWORD* value) {
	return gapless_config_get(attrib, value);
}

HSTREAM BASSGAPLESSDEF(BASS_GAPLESS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, void *user) {
	if (!is_initialized) {
		return 0;
	}
	return gapless_stream_create(freq, chans, flags, user);
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