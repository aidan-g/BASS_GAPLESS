#include "../bass/bass.h"

#ifndef BASSGAPLESSASIODEF
#define BASSGAPLESSASIODEF(f) WINAPI f
#endif

__declspec(dllexport)
BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_Init)();

__declspec(dllexport)
BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_Free)();

__declspec(dllexport)
BOOL BASSGAPLESSASIODEF(BASS_GAPLESS_ASIO_ChannelEnable)(BOOL input, DWORD channel, void *user);

DWORD CALLBACK asio_gapless_stream_proc(BOOL input, DWORD channel, void *buffer, DWORD length, void *user);