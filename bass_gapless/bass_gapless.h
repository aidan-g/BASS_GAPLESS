#ifndef BASSGAPLESS_H
#define BASSGAPLESS_H

#include "../bass/bass.h"

#ifndef BASSGAPLESSDEF
#define BASSGAPLESSDEF(f) WINAPI f
#endif

typedef enum {
	NONE = 0,
	KEEP_ALIVE = 1,
	BLOCKING_EVENTS = 2
} GS_ATTRIBUTE;

typedef enum {
	GS_NONE = 0,
	GS_START = 1,
	GS_CHANGE = 2,
	GS_END = 3
} GS_EVENT_TYPE;

typedef struct {
	GS_EVENT_TYPE event_type;
	DWORD channel_1;
	DWORD channel_2;
} GS_EVENT_ARGS;

typedef VOID(CALLBACK GSEVENTPROC)(GS_EVENT_ARGS args);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_Init)();

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_Free)();

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_SetConfig)(GS_ATTRIBUTE attrib, DWORD value);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_GetConfig)(GS_ATTRIBUTE attrib, DWORD* value);

__declspec(dllexport)
HSTREAM BASSGAPLESSDEF(BASS_GAPLESS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, void *user);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelEnqueue)(HSTREAM handle);

__declspec(dllexport)
DWORD* BASSGAPLESSDEF(BASS_GAPLESS_GetChannels)(DWORD *count);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_ChannelRemove)(DWORD handle);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_EnableEvents)(GSEVENTPROC* handler);

__declspec(dllexport)
BOOL BASSGAPLESSDEF(BASS_GAPLESS_DisableEvents)();

#endif