#ifndef RING_BUFER_H
#define RING_BUFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "routing.h"

typedef Frame payload_t;

typedef struct {
    payload_t *buffer;    // Buffer array
    size_t head;    // Points to the next insertion position
    size_t tail;    // Points to the next element to remove
    size_t size;    // Total size of the buffer
    bool is_full;   // Flag to indicate buffer full status
} RingBuffer;

RingBuffer* ring_buffer_init(size_t size); 
void ring_buffer_free(RingBuffer *rb);
bool ring_buffer_push(RingBuffer *rb, payload_t value);
bool ring_buffer_pop(RingBuffer *rb, payload_t *value);
bool ring_buffer_is_empty(RingBuffer *rb);
bool ring_buffer_is_full(RingBuffer *rb);

#endif
