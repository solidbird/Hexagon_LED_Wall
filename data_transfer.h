#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include "hexagon.h"

int init_buffer(Buffer *buffer);
int push(Buffer *buffer, char value);
int pop(Buffer *buffer);
int is_empty(Buffer *buffer);
int is_full(Buffer *buffer);

/*void* recieve_from_parent(HexagonPanel *hp, int buffer_index); 
void* recieve_from_child(HexagonPanel *hp, int buffer_index);
void* send_to_child(HexagonPanel *hp, int buffer_index, int data);
void* send_to_parent(HexagonPanel *hp, int buffer_index, int data);*/

void* send_master(void *arg);
void* sender(Buffer *dest_buffer, int data);
void* reciever(void *arg);

#endif
