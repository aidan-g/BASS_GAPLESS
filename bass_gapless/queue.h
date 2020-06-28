#include <windows.h>

typedef struct QUEUE_NODE QUEUE_NODE;

typedef struct QUEUE_NODE {
	void* data;
	QUEUE_NODE* previous;
};

typedef struct {
	QUEUE_NODE* head;
	QUEUE_NODE* tail;
	DWORD length;
	DWORD capacity;
	HANDLE lock;
} QUEUE;

QUEUE* queue_create(DWORD capacity, BOOL synchronized);

void queue_free(QUEUE* queue);

BOOL queue_contains(QUEUE* queue, void* data);

BOOL queue_enqueue(QUEUE* queue, void* data, BOOL unique);

BOOL queue_dequeue(QUEUE* queue, void** data);

BOOL queue_push(QUEUE* queue, void* data, BOOL unique);

BOOL queue_peek(QUEUE* queue, void** data);

BOOL queue_is_empty(QUEUE* queue, BOOL* empty);

BOOL queue_get_all(QUEUE* queue, void** data, size_t size, DWORD* length);

BOOL queue_remove(QUEUE* queue, void* data);