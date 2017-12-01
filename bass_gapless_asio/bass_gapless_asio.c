#include "bass_gapless_asio.h"
#include "../bass_gapless/gapless_stream.h"
#include "../bass/bassasio.h"

BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_Init)() {
	//Nothing to do (yet).
	return TRUE;
}

BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_Free)() {
	//Nothing to do (yet).
	return TRUE;
}

BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_ChannelEnable)(BOOL input, DWORD channel, void *user) {
	return BASS_ASIO_ChannelEnable(input, channel, &asio_gapless_stream_proc, user);
}

DWORD CALLBACK asio_gapless_stream_proc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user) {
	return gapless_stream_proc(buffer, length);
}