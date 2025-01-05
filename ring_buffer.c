#include "ring_buffer.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize the ring buffer
RingBuffer* ring_buffer_init(size_t size) {
    RingBuffer *rb = (RingBuffer*)malloc(sizeof(RingBuffer));
    if (!rb) {
        return NULL;
    }
    rb->buffer = (RingBufferEntry*)malloc(size * sizeof(RingBufferEntry));
    if (!rb->buffer) {
        free(rb);
        return NULL;
    }
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->is_full = false;
    return rb;
}

// Free the ring buffer
void ring_buffer_free(RingBuffer *rb) {
    if (rb) {
        free(rb->buffer);
        free(rb);
    }
}

// Add an element to the ring buffer
bool ring_buffer_push(RingBuffer *rb, BufferData value, DataType type) {
    if (!rb){ 
		return false;
	}

    rb->buffer[rb->head].data = value;
    rb->buffer[rb->head].type = type;
    rb->head = (rb->head + 1) % rb->size;

    if (rb->is_full) {
        rb->tail = (rb->tail + 1) % rb->size; // Overwrite oldest value
    }

    rb->is_full = (rb->head == rb->tail);
	
    return true;
}

// Remove an element from the ring buffer
bool ring_buffer_pop(RingBuffer *rb, BufferData *value, DataType *type) {
	if (!rb || (rb->head == rb->tail && !rb->is_full)) {
        return false; // Buffer is empty
    }

    *value = rb->buffer[rb->tail].data;
    *type = rb->buffer[rb->tail].type;
    rb->tail = (rb->tail + 1) % rb->size;
    rb->is_full = false;
	
    return true;
}

// Check if the buffer is empty
bool ring_buffer_is_empty(RingBuffer *rb) {
    return (rb && rb->head == rb->tail && !rb->is_full);
}

// Check if the buffer is full
bool ring_buffer_is_full(RingBuffer *rb) {
    return rb && rb->is_full;
}
