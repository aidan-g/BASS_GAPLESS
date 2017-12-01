#include "buffer.h"

void* offset_buffer(void* buffer, DWORD position) {
	return (BYTE*)buffer + position;
}