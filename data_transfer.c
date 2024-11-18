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
	//int index = (int)args->buffer_in_index;

	struct timespec ts;
	ts.tv_sec = 1;
	ts.tv_nsec = 5000000;//time_per_cycle;	

	int data = 0;
	while(1){
		Buffer* send_buffer = &(args->hexagon_panel->peer_out[1]->buffer_in[1]);

		pthread_mutex_lock(&(send_buffer->buffer_mutex));
		while(is_full(send_buffer)){
			pthread_cond_wait(&(send_buffer->bufferNotFull), &(send_buffer->buffer_mutex));
		}
		push(send_buffer, data);
		data = (data + 1) % 127;

		pthread_mutex_unlock(&(send_buffer->buffer_mutex));
		pthread_cond_signal(&(send_buffer->bufferNotEmpty));
		nanosleep(&ts, NULL);
	}
}

void* sender(HexagonPanel* src_hp, Buffer* dest_buffer, int index, int data){
	//if(src_hp->index == 0 && index == 1)
	//	TraceLog(LOG_INFO, "SEND %d: FROM %d TO INDEX %d", data, src_hp->index, index);
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 5000000;//time_per_cycle;	

	pthread_mutex_lock(&(dest_buffer->buffer_mutex));

	while(is_full(dest_buffer)){
		pthread_cond_wait(&(dest_buffer->bufferNotFull), &(dest_buffer->buffer_mutex));
	}
	push(dest_buffer, data);

	pthread_cond_signal(&(dest_buffer->bufferNotEmpty));
	pthread_mutex_unlock(&(dest_buffer->buffer_mutex));
}

void* receiver_out(void *arg){
	Polling_args *args = (Polling_args *)arg;
	int out_index = (int)args->buffer_out_index;
	Buffer* respond_buffer = &(args->hexagon_panel->buffer_out[out_index]);
	
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 500000;//time_per_cycle;
	int last_data = -1;
	
	while(1){
		//TraceLog(LOG_INFO, "LET'S GOOO");
		pthread_mutex_lock(
			&(respond_buffer->buffer_mutex)
		);
		
		while(is_empty(respond_buffer)){
			pthread_cond_wait(&(respond_buffer->bufferNotEmpty), &(respond_buffer->buffer_mutex));
		}
		int pop_data = pop(respond_buffer);
		//process_stuff(args->hexagon_panel, pop_data);
		for(int i = 0; i < 3; i++){	
			if(args->hexagon_panel->peer_out[i] != NULL){
				TraceLog(LOG_INFO, "RECV BACK %d/%d --> %d/%d = %d", args->hexagon_panel->index, i, args->hexagon_panel->peer_out[i]->index, i, pop_data);
				//sender(args->hexagon_panel, args->hexagon_panel->peer_out[i], i, pop_data);
			}
		}
		
		int last_data = pop_data;
		pthread_cond_signal(&(respond_buffer->bufferNotFull));
		pthread_mutex_unlock(&(respond_buffer->buffer_mutex));
		nanosleep(&ts, NULL);
	}
}

void* receiver_in(void *arg){
	Polling_args *args = (Polling_args *)arg;
	int in_index = (int)args->buffer_in_index;
	Buffer* reciever_buffer = &(args->hexagon_panel->buffer_in[in_index]);
	
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 500000;//time_per_cycle;
	int last_data = -1;
	
	while(1){
		pthread_mutex_lock(
			&(reciever_buffer->buffer_mutex)
		);
		
		while(is_empty(reciever_buffer)){
			pthread_cond_wait(&(reciever_buffer->bufferNotEmpty), &(reciever_buffer->buffer_mutex));
		}
		int pop_data = pop(reciever_buffer);
		//if(last_data >= pop_data)
		//	TraceLog(LOG_INFO, "DOUBLE RECV %d: %d/%d <-- %d/%d", pop_data, args->hexagon_panel->index, in_index, args->hexagon_panel->peer_in[in_index]->index, in_index);
		if(args->hexagon_panel->index == 0)
			TraceLog(LOG_INFO, "RECV %d: %d/%d <-- %d/%d", pop_data, args->hexagon_panel->index, in_index, args->hexagon_panel->peer_in[in_index]->index, in_index);
		process_stuff(args->hexagon_panel, pop_data);
	
		for(int i = 0; i < 3; i++){	
			if(args->hexagon_panel->peer_out[i] != NULL){
				sender(args->hexagon_panel, &(args->hexagon_panel->peer_out[i]->buffer_in[i]), i, pop_data);
			}
		}
		for(int i = 0; i < 3; i++){	
			if(args->hexagon_panel->peer_in[i] != NULL && args->hexagon_panel->peer_in[i]->index != -1){
				//TraceLog(LOG_INFO, "SEND BACKWARDS %d/%d --> %d/%d: %d",
				//args->hexagon_panel->index, i,
				//args->hexagon_panel->peer_in[i]->index, i,
				//pop_data
				//);
				sender(args->hexagon_panel, &(args->hexagon_panel->peer_in[i]->buffer_out[i]), i, pop_data);
			}
		}
		
		int last_data = pop_data;
		pthread_cond_signal(&(reciever_buffer->bufferNotFull));
		pthread_mutex_unlock(&(reciever_buffer->buffer_mutex));
		nanosleep(&ts, NULL);
	}
}
