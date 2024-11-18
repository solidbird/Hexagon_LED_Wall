#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include "hexagon.h"
#include "routing.h"

int init_buffer(Buffer *buffer);
int push(Buffer *buffer, uint8_t value);
int pop(Buffer *buffer);
int is_empty(Buffer *buffer);
int is_full(Buffer *buffer);

void* send_master(void *arg);
void* send_payload(HexagonPanel *src_hp, Buffer *dest_buffer, int index, void* payload);
void* receiver_out(void *arg);
void* receiver_in(void *arg);

#endif
