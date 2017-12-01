#include <windows.h>

BOOL gapless_stream_registry_create();

BOOL gapless_stream_registry_enqueue(DWORD handle);

BOOL gapless_stream_registry_dequeue(DWORD* handle);

BOOL gapless_stream_registry_peek(DWORD* handle);

BOOL gapless_stream_registry_is_empty(BOOL* empty);

BOOL gapless_stream_registry_count(DWORD* length);

DWORD* gapless_stream_registry_get_all(DWORD* length);

BOOL gapless_stream_registry_remove(DWORD handle);

BOOL gapless_stream_registry_free();