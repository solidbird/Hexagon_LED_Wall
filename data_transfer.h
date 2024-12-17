#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include "hexagon.h"
#include "routing.h"

void init_buffer(Buffer *buffer);
int push_back(Buffer *buffer, uint8_t value);
uint8_t* pop_front(Buffer *buffer);
int is_empty(Buffer *buffer);
int is_full(Buffer *buffer);

void* send_master(void *arg);
void send_payload(Buffer *dest_buffer, void* payload);
void* receiver_out(void *arg);
void* receiver_in(void *arg);

#endif
