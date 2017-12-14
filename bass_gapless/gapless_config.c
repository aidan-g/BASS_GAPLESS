#ifdef _DEBUG
#include <stdio.h>
#endif

#include "gapless_config.h"

#define MAX_CONFIGS 10

static DWORD config[MAX_CONFIGS];

BOOL gapless_config_set(GS_ATTRIBUTE attrib, DWORD value) {
	config[attrib] = value;
#if _DEBUG
	printf("Setting config: %d = %d\n", attrib, value);
#endif
	return TRUE;
}

BOOL gapless_config_get(GS_ATTRIBUTE attrib, DWORD* value) {
	if (*value = config[attrib]) {
#if _DEBUG
		printf("Getting config: %d = %d\n", attrib, value);
#endif
		return TRUE;
	}
	return FALSE;
}