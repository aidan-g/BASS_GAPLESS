#ifdef _DEBUG
#include <stdio.h>
#endif

#include "bass_gapless_asio.h"
#include "../bass_gapless/gapless_stream.h"
#include "../bass/bassasio.h"

BOOL is_initialized = FALSE;

//I have no idea how to prevent linking against this routine in msvcrt.
//It doesn't exist on Windows XP.
//Hopefully it doesn't do anything important.
int _except_handler4_common() {
	return 0;
}

BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_Init)() {
	if (is_initialized) {
		return FALSE;
	}
	is_initialized = TRUE;
#if _DEBUG
	printf("BASS GAPLESS ASIO initialized.\n");
#endif
	return TRUE;
}

BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_Free)() {
	if (!is_initialized) {
		return FALSE;
	}
	is_initialized = FALSE;
#if _DEBUG
	printf("BASS GAPLESS ASIO released.\n");
#endif
	return TRUE;
}

BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_ChannelEnable)(BOOL input, DWORD channel, void *user) {
	BOOL success = BASS_ASIO_ChannelEnable(input, channel, &asio_gapless_stream_proc, user);
	if (!success) {
#if _DEBUG
		printf("BASS GAPLESS ASIO enabled.\n");
#endif
	}
	return success;
}

DWORD CALLBACK asio_gapless_stream_proc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user) {
	DWORD result = gapless_stream_proc(buffer, length);
	switch (result)
	{
	case BASS_STREAMPROC_END:
	case BASS_ERROR_UNKNOWN:
		result = 0;
		break;
	default:
#if _DEBUG
		printf("Write %d bytes to ASIO buffer\n", result);
#endif
		break;
	}
	if (result < length) {
#if _DEBUG
		printf("Buffer underrun while writing to ASIO buffer.\n");
#endif
	}
	return result;
}