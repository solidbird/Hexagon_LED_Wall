#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include "hexagon.h"

int init_buffer(Buffer *buffer);
int push(Buffer *buffer, char value);
int pop(Buffer *buffer);
int is_empty(Buffer *buffer);
int is_full(Buffer *buffer);

void* send_master(void *arg);
void* sender(HexagonPanel *src_hp, Buffer *dest_buffer, int index, int data);
void* receiver_out(void *arg);
void* receiver_in(void *arg);

#endif
