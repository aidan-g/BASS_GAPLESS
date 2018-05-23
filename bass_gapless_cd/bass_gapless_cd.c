#include "bass_gapless_cd.h"
#include "../bass_gapless/bass_gapless.h"
#include "../bass/basscd.h"

BOOL is_initialized = FALSE;

DWORD current_drive = NO_DRIVE;

DWORD current_flags = BASS_DEFAULT;

BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Init)() {
	if (is_initialized) {
		return FALSE;
	}
	is_initialized = TRUE;
	return TRUE;
}

BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Free)() {
	if (!is_initialized) {
		return FALSE;
	}
	is_initialized = FALSE;
	return TRUE;
}

BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Enable)(DWORD drive, DWORD flags) {
	DWORD success = TRUE;
	if (current_drive != NO_DRIVE) {
		return FALSE;
	}
	success &= BASS_GAPLESS_SetConfig(BLOCKING_EVENTS, TRUE);
	success &= BASS_GAPLESS_EnableEvents(&gapless_cd_event_handler);
	current_drive = drive;
	current_flags = flags;
	return success;
}

BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Disable)() {
	DWORD success = TRUE;
	if (current_drive == NO_DRIVE) {
		return FALSE;
	}
	success &= BASS_GAPLESS_SetConfig(BLOCKING_EVENTS, FALSE);
	success &= BASS_GAPLESS_DisableEvents();
	current_drive = NO_DRIVE;
	current_flags = BASS_DEFAULT;
	return success;
}

void CALLBACK gapless_cd_event_handler(GS_EVENT_ARGS args) {
	switch (args.event_type) {
	case GS_END:
		gapless_cd_prevent_stall(args.channel_1);
		break;
	}
}

BOOL gapless_cd_prevent_stall(DWORD handle) {
	DWORD tracks;
	DWORD recycle_stream;
	DWORD track = BASS_CD_StreamGetTrack(handle);
	if (track == NO_TRACK) {
		return FALSE;
	}
	tracks = BASS_CD_GetTracks(current_drive);
	if (track >= tracks) {
		return FALSE;
	}
	track++;
	if (BASS_GAPLESS_GetConfig(RECYCLE_STREAM, &recycle_stream) && recycle_stream) {
		if (!BASS_CD_StreamSetTrack(handle, track)) {
			return FALSE;
		}
	}
	else {
		if (!BASS_StreamFree(handle)) {
			return FALSE;
		}
		handle = BASS_CD_StreamCreate(current_drive, track, current_flags);
		if (handle == 0) {
			return FALSE;
		}
	}
	if (!BASS_GAPLESS_ChannelEnqueue(handle)) {
		return FALSE;
	}
	return TRUE;
}