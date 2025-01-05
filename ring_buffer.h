#ifndef RING_BUFER_H
#define RING_BUFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "routing.h"

typedef union {
	Frame frame;
	Discovery_package discovery_package;
} BufferData;

typedef enum {
	TYPE_FRAME,
	TYPE_DISCOVERY_PACKAGE
} DataType;

typedef struct {
	BufferData data;
	DataType type;
} RingBufferEntry;

typedef struct {
    RingBufferEntry *buffer;    // RingBufferEntry array
    size_t head;    			// Points to the next insertion position
    size_t tail;    			// Points to the next element to remove
    size_t size;    			// Total size of the buffer
    bool is_full;   			// Flag to indicate buffer full status
} RingBuffer;

RingBuffer* ring_buffer_init(size_t size); 
void ring_buffer_free(RingBuffer *rb);
bool ring_buffer_push(RingBuffer *rb, BufferData *value, DataType type);
bool ring_buffer_pop(RingBuffer *rb, BufferData *value, DataType *type);
bool ring_buffer_is_empty(RingBuffer *rb);
bool ring_buffer_is_full(RingBuffer *rb);

#endif
