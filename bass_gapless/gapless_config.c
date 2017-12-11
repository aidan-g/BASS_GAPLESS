#include "gapless_config.h"

#define MAX_CONFIGS 10

static DWORD config[MAX_CONFIGS];

BOOL gapless_config_set(GS_ATTRIBUTE attrib, DWORD value) {
	config[attrib] = value;
	return TRUE;
}

BOOL gapless_config_get(GS_ATTRIBUTE attrib, DWORD* value) {
	if (*value = config[attrib]) {
		return TRUE;
	}
	return FALSE;
}