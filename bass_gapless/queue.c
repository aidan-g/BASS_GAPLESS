#ifdef _DEBUG
#include <stdio.h>
#endif

#include "queue.h"

#define QUEUE_LOCK_TIMEOUT 1000

QUEUE* queue_create(DWORD capacity, BOOL synchronized) {
	QUEUE* queue = calloc(sizeof(QUEUE), 1);
	queue->capacity = capacity;
	if (synchronized) {
		queue->lock = CreateSemaphore(NULL, 1, 1, NULL);
		if (!queue->lock) {
#if _DEBUG
			printf("Failed to create semaphore.\n");
#endif
		}
	}
	return queue;
}

void queue_free(QUEUE* queue) {
	void* data;
	while (queue_dequeue(queue, &data)); //We're not freeing *data, is that OK?
	if (queue->lock) {
		if (!CloseHandle(queue->lock)) {
#if _DEBUG
			printf("Failed to release semaphore.\n");
#endif
		}
		else {
			queue->lock = NULL;
		}
	}
	free(queue);
	queue = NULL;
}

BOOL queue_enter(QUEUE* queue) {
	DWORD result;
	if (queue->lock) {
		result = WaitForSingleObject(queue->lock, QUEUE_LOCK_TIMEOUT);
		if (result == WAIT_OBJECT_0) {
			return TRUE;
		}
		else {
#if _DEBUG
			printf("Failed to enter semaphore.\n");
#endif
			return FALSE;
		}
	}
	return TRUE;
}

void queue_exit(QUEUE* queue) {
	if (queue->lock) {
		if (!ReleaseSemaphore(queue->lock, 1, NULL)) {
#if _DEBUG
			printf("Failed to exit semaphore.\n");
#endif
		}
	}
}

BOOL queue_enqueue(QUEUE* queue, void* data) {
	BOOL success = FALSE;
	QUEUE_NODE* node;
	if (!queue_enter(queue)) {
		success = FALSE;
	}
	else if (queue->length == queue->capacity) {
		success = FALSE;
	}
	else {
		node = calloc(sizeof(QUEUE_NODE), 1);
		node->data = data;
		node->previous = NULL;
		if (queue->length == 0) {
			queue->head = node;
			queue->tail = node;
		}
		else {
			queue->tail->previous = node;
			queue->tail = node;
		}
		queue->length++;
		success = TRUE;
	}
	queue_exit(queue);
	return success;
}

BOOL queue_dequeue(QUEUE* queue, void** data) {
	BOOL success = FALSE;
	QUEUE_NODE* node;
	if (!queue_enter(queue)) {
		success = FALSE;
	}
	else if (queue->length == 0) {
		success = FALSE;
	}
	else {
		node = queue->head;
		queue->head = node->previous;
		queue->length--;
		free(node);
		data[0] = node->data;
		success = TRUE;
	}
	queue_exit(queue);
	return success;
}

BOOL queue_peek(QUEUE* queue, void** data) {
	BOOL success = FALSE;
	if (!queue_enter(queue)) {
		success = FALSE;
	}
	else if (queue->length > 0) {
		data[0] = queue->head->data;
		success = TRUE;
	}
	queue_exit(queue);
	return success;
}

BOOL queue_is_empty(QUEUE* queue, BOOL* empty) {
	BOOL success = FALSE;
	if (!queue_enter(queue)) {
		success = FALSE;
	}
	else {
		*empty = queue->length == 0;
		success = TRUE;
	}
	queue_exit(queue);
	return success;
}

BOOL queue_get_all(QUEUE* queue, void** data, DWORD* length) {
	DWORD a;
	QUEUE_NODE* node;
	BOOL success = FALSE;
	if (!queue_enter(queue)) {
		success = FALSE;
	}
	else if (queue->length == 0) {
		success = FALSE;
	}
	else {
		node = queue->head;
		*length = queue->length;
		for (a = 0; a < *length; a++) {
			data[a] = node->data;
			node = node->previous;
		}
		success = TRUE;
	}
	queue_exit(queue);
	return success;
}

BOOL queue_remove(QUEUE* queue, void* data) {
	BOOL success = FALSE;
	QUEUE_NODE* head;
	QUEUE_NODE* tail;
	if (!queue_enter(queue)) {
		success = FALSE;
	}
	else if (queue->length == 0) {
		success = FALSE;
	}
	else {
		head = queue->head;
		tail = queue->tail;
		if (head->data == data) {
			queue->head = queue->head->previous;
			free(head);
			success = TRUE;
		}
		else {
			while (head && head->data != data) {
				tail = head;
				head = head->previous;
			}
			if (head) {
				tail->previous = head->previous;
				if (queue->tail == head) {
					queue->tail = tail;
				}
				free(head);
				success = TRUE;
			}
		}
		if (success) {
			queue->length--;
		}
	}
	queue_exit(queue);
	return success;
}