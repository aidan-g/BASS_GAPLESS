#include "bass_gapless.h"

HSTREAM BASSGAPLESSDEF(gapless_stream_create)(DWORD freq, DWORD chans, DWORD flags, void *user);

DWORD CALLBACK gapless_stream_bass_proc(HSTREAM handle, void *buffer, DWORD length, void *user);

__declspec(dllexport)
DWORD gapless_stream_proc(void *buffer, DWORD length);