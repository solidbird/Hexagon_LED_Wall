#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include "hexagon.h"
#include "routing.h"

void* send_master(void *arg);
void send_payload(Buffer *dest_buffer, void* payload);
void* receiver_out(void *arg);
void* receiver_in(void *arg);

#endif
