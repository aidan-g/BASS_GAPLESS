#include "../bass/bass.h"

#ifndef BASSGAPLESSDEF
#define BASSGAPLESSDEF(f) WINAPI f
#endif

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_Init)();

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_Free)();

__declspec(dllexport)
HSTREAM BASSGAPLESSDEF(BASS_GAPLESS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, void *user);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelEnqueue)(HSTREAM handle);

__declspec(dllexport)
DWORD* BASSGAPLESSDEF(BASS_GAPLESS_GetChannels)(DWORD *count);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelRemove)(DWORD handle);