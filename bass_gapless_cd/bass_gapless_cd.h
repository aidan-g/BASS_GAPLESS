#ifndef BASSGAPLESSCD_H
#define BASSGAPLESSCD_H

#include "../bass/bass.h"
#include "../bass_gapless/bass_gapless.h"

#ifndef BASSGAPLESSCDDEF
#define BASSGAPLESSCDDEF(f) WINAPI f
#endif

#define BASS_DEFAULT 0
#define NO_DRIVE -1
#define NO_TRACK -1

__declspec(dllexport)
BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Init)();

__declspec(dllexport)
BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Free)();

__declspec(dllexport)
BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Enable)(DWORD drive, DWORD flags);

__declspec(dllexport)
BOOL BASSGAPLESSCDDEF(BASS_GAPLESS_CD_Disable)();

__declspec(dllexport)
void CALLBACK gapless_cd_event_handler(GS_EVENT_ARGS args);

__declspec(dllexport)
BOOL gapless_cd_prevent_stall(DWORD handle);

#endif