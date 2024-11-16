#include "data_transfer.h"
#include "hexagon.h"

int push(Buffer *buffer, char value){
	if(is_full(buffer)){ return 0; }
	buffer->data[++buffer->top] = value;
	return 1;
}

int init_buffer(Buffer *buffer){
	memset(buffer->data, 0, 1024 * sizeof(*(buffer->data)));
	buffer->top = -1;
}

int pop(Buffer *buffer){
	if(is_empty(buffer)) { return 0; }
	char pop_data = buffer->data[buffer->top];	
	buffer->data[buffer->top--] = '\0';

	return pop_data;
}

int is_empty(Buffer *buffer){
	return buffer->top == -1;
}

int is_full(Buffer *buffer){
	return buffer->top == 1023;
}
void* send_master(void *arg){
	Polling_args *args = (Polling_args *)arg;
	int index = (int)args->buffer_index;

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 100;//time_per_cycle;	

	int data = 0;
	while(1){
		if(args->hexagon_panel->peer_out[index] == NULL){ return NULL; }

		Buffer* send_buffer = &(args->hexagon_panel->peer_out[index]->buffer_in[index]);
	
		pthread_mutex_lock(&(send_buffer->buffer_mutex));
		if(is_full(send_buffer)){
			TraceLog(LOG_INFO, "BBBB");
			//pthread_cond_wait(&(send_buffer->bufferNotFull), &(send_buffer->buffer_mutex));
			pthread_mutex_unlock(&(send_buffer->buffer_mutex));
			continue;
		}
		push(send_buffer, data);
		data = (data + 1) % 127;

		TraceLog(LOG_INFO, "MASTER SEND");
		pthread_mutex_unlock(&(send_buffer->buffer_mutex));

		nanosleep(&ts, NULL);
	}
}

void* sender(Buffer* dest_buffer, int data){
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 70;//time_per_cycle;	

	pthread_mutex_lock(&(dest_buffer->buffer_mutex));

	if(is_full(dest_buffer)){
		TraceLog(LOG_INFO, "BBBB");
		//pthread_cond_wait(&(dest_buffer->bufferNotFull), &(dest_buffer->buffer_mutex));
		pthread_mutex_unlock(&(dest_buffer->buffer_mutex));
		return;
	}
	push(dest_buffer, data);

	pthread_cond_signal(&(dest_buffer->bufferNotEmpty));
	pthread_mutex_unlock(&(dest_buffer->buffer_mutex));
}

void* reciever(void *arg){
	Polling_args *args = (Polling_args *)arg;
	int index = (int)args->buffer_index;
	Buffer* reciever_buffer = NULL;
	Buffer* respond_buffer = NULL;

	if(args->hexagon_panel->peer_in[index] != NULL){
		reciever_buffer = &(args->hexagon_panel->buffer_in[index]);
	}else if(args->hexagon_panel->peer_out[index] != NULL){
		respond_buffer = &(args->hexagon_panel->buffer_out[index]);
	}

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 50;//time_per_cycle;
	
	while(1){
		pthread_mutex_lock(
			&(reciever_buffer->buffer_mutex)
		);
		
		if(is_empty(reciever_buffer)){
		//	pthread_cond_wait(&(reciever_buffer->bufferNotEmpty), &(reciever_buffer->buffer_mutex));
			pthread_mutex_unlock(&(reciever_buffer->buffer_mutex));
			continue;
		}
		int pop_data = pop(reciever_buffer);
		TraceLog(LOG_INFO, "RECV BUFFER: %d FOR ID/Buffer: %d/%d", pop_data, args->hexagon_panel->index, index);
		process_stuff(args->hexagon_panel, pop_data);
	
		for(int i = 0; i < 3; i++){	
			if(args->hexagon_panel->peer_out[i] != NULL){
				sender(&(args->hexagon_panel->peer_out[i]->buffer_in[i]), pop_data);
			}
			if(args->hexagon_panel->peer_in[i] != NULL){
				sender(&(args->hexagon_panel->peer_in[i]->buffer_out[i]), pop_data);
			}
		}	

		pthread_cond_signal(&(reciever_buffer->bufferNotEmpty));
		pthread_mutex_unlock(&(reciever_buffer->buffer_mutex));
		nanosleep(&ts, NULL);
	}
}
