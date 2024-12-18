#include <string.h>
#include <time.h>
#include "data_transfer.h"
#include "routing.h"
#include "hexagon.h"
#include "ring_buffer.h"

#define PAYLOAD_SIZE sizeof(Frame)

void prep_package(Frame *frame, int* flip) {
	for(size_t i = 0; i < 127; i++){
		if(*flip){
			frame->route = ((uint64_t)3) << 62;
			frame->route >>= 2;
			frame->route |= ((uint64_t)2) << 62;

			if(i % 2){
				frame->rgb_value[i] = (RGB_Value){200, 50, 0};
			}else{
				frame->rgb_value[i] = (RGB_Value){0, 0, 0};
			}
			*flip = 0;
		}else{

			frame->route = ((uint64_t)3) << 62;
			frame->route >>= 2;
			frame->route |= ((uint64_t)2) << 62;
			frame->route >>= 2;
			frame->route |= ((uint64_t)2) << 62;

			if(i % 2){
				frame->rgb_value[i] = (RGB_Value){0, 0, 0};
			}else{
				frame->rgb_value[i] = (RGB_Value){0, 50, 200};
			}
			*flip = 1;
		}
	}
}

void* send_master(void *arg){
	Polling_args *args = (Polling_args *)arg;

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 5000;//time_per_cycle;	

	int flip = 0;
	
	while(1){
		Buffer* send_buffer = &(args->hexagon_panel->peer_out[1]->buffer_in[1]);

		pthread_mutex_lock(&(send_buffer->buffer_mutex));

		Frame frame;
		prep_package(&frame, &flip);

		
		uint8_t *bytes = (uint8_t *)&frame;
		
		while(ring_buffer_is_full(send_buffer->ring)){
			TraceLog(LOG_INFO, "A");
			pthread_cond_signal(&(send_buffer->bufferNotEmpty));
			pthread_cond_wait(&(send_buffer->bufferNotFull), &(send_buffer->buffer_mutex));
		}
		
		for(size_t i = 0; i < sizeof(Frame); i++){
			ring_buffer_push(send_buffer->ring, bytes[i]);
		}

		pthread_mutex_unlock(&(send_buffer->buffer_mutex));
		pthread_cond_signal(&(send_buffer->bufferNotEmpty));
		nanosleep(&ts, NULL);
	}
}

void send_payload(Buffer* dest_buffer, void *payload){
	pthread_mutex_lock(&(dest_buffer->buffer_mutex));

	size_t size = PAYLOAD_SIZE;
	for(uint8_t *byte = payload; size--; ++byte){
		while(ring_buffer_is_full(dest_buffer->ring)){
			TraceLog(LOG_INFO, "b");
			pthread_cond_wait(&(dest_buffer->bufferNotFull), &(dest_buffer->buffer_mutex));
		}
		ring_buffer_push(dest_buffer->ring, *byte);
	}
	pthread_cond_signal(&(dest_buffer->bufferNotEmpty));
	pthread_mutex_unlock(&(dest_buffer->buffer_mutex));
}

/*void* receiver_out(void *arg){
	Polling_args *args = (Polling_args *)arg;
	int out_index = (int)args->buffer_out_index;
	Buffer* respond_buffer = &(args->hexagon_panel->buffer_out[out_index]);
	
	while(1){
		pthread_mutex_lock(
			&(respond_buffer->buffer_mutex)
		);
		
		while(is_empty(respond_buffer)){
			TraceLog(LOG_INFO, "C");
			pthread_cond_wait(&(respond_buffer->bufferNotEmpty), &(respond_buffer->buffer_mutex));
		}
		uint8_t* pop_data = pop_front(respond_buffer);
		
		for(int i = 0; i < 3; i++){	
			if(args->hexagon_panel->peer_out[i] != NULL){
				//send_payload(&(args->hexagon_panel->peer_out[i]->buffer_out[i]), *pop_data);
			}
		}
		
		pthread_cond_signal(&(respond_buffer->bufferNotFull));
		pthread_mutex_unlock(&(respond_buffer->buffer_mutex));
	}
}*/

void* receiver_in(void *arg){
	Polling_args *args = (Polling_args *)arg;
	HexagonPanel *hp = args->hexagon_panel;

	int in_index = (int)args->buffer_in_index;
	Buffer* reciever_buffer = &(args->hexagon_panel->buffer_in[in_index]);
	Frame *frame = malloc(sizeof(Frame));
	
	uint8_t build_buffer[10000];
	int byte_size = 0;
	uint8_t *pop_data = (uint8_t*)malloc(sizeof(uint8_t));

	int i = 0;
	while(1){
		pthread_mutex_lock(
			&(reciever_buffer->buffer_mutex)
		);
		
		while(ring_buffer_is_empty(reciever_buffer->ring)){
			TraceLog(LOG_INFO, "D");
			pthread_cond_signal(&(reciever_buffer->bufferNotFull));
			pthread_cond_wait(&(reciever_buffer->bufferNotEmpty), &(reciever_buffer->buffer_mutex));
		}

		ring_buffer_pop(reciever_buffer->ring, pop_data);
		byte_size++;
		build_buffer[byte_size - 1] = *pop_data;
		if( !(byte_size % sizeof(Frame)) ){
			frame = (Frame*) build_buffer;
			switch(frame->route & (((uint64_t)3) << 62)){
				case 0:
					if(args->hexagon_panel->peer_out[0] != NULL){
						frame->route <<= 2;
						send_payload(&(args->hexagon_panel->peer_out[0]->buffer_in[0]), frame);
					}else{
						TraceLog(LOG_INFO, "Inferface 0 not found");
					}
				break;
				case (((uint64_t)1) << 62):
					if(args->hexagon_panel->peer_out[1] != NULL){
						frame->route <<= 2;
						send_payload(&(args->hexagon_panel->peer_out[1]->buffer_in[1]), frame);
					}else{
						TraceLog(LOG_INFO, "Interface 1 not found");
					}
				break;
				case (((uint64_t)2) << 62):
					if(args->hexagon_panel->peer_out[2] != NULL){
						frame->route <<= 2;
						send_payload(&(args->hexagon_panel->peer_out[2]->buffer_in[2]), frame);
					}else{
						TraceLog(LOG_INFO, "Interface 2 not found");
					}
				break;
				case (((uint64_t)3) << 62):
					//Process the package payload and display the content on the matrix
					TraceLog(
						LOG_INFO,
						"%d, PAYLOAD %d %d %d @ %d",
						i++,
						frame->rgb_value[5].r,
						frame->rgb_value[5].g,
						frame->rgb_value[5].b,
						frame->route >> 62
					);
					process_rgb_values(hp, frame);
				break;
			}

			byte_size = 0;
		}

		pthread_cond_signal(&(reciever_buffer->bufferNotFull));
		pthread_mutex_unlock(&(reciever_buffer->buffer_mutex));
	}
}
