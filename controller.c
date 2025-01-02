#include "controller.h"
#include "hexagon.h"
#include <time.h>

Frame* generate_frames(int frames_amount){
	Frame* frames = (Frame*) malloc(sizeof(Frame) * frames_amount);

	for(size_t x = 0; x < frames_amount; x++){
		for(size_t i = 0; i < 127; i++){
			frames[x].rgb_value[i] = (RGB_Value){rand() % 255, rand() % 255, rand() % 255};
			//if(i == x){
			//	frames[x].rgb_value[i] = (RGB_Value){200, 50, 0};
			//}else{
			//	frames[x].rgb_value[i] = (RGB_Value){0, 0, 0};
			//}
		}

		if(x < (int)(frames_amount/6)*1){
			frames[x].route = ((uint64_t)3) << 62;
			frames[x].route >>= 2;
			frames[x].route |= ((uint64_t)2) << 62;
			frames[x].route >>= 2;
			frames[x].route |= ((uint64_t)2) << 62;
		}else if(x < (int)(frames_amount/6)*2){
			frames[x].route = ((uint64_t)3) << 62;
			frames[x].route >>= 2;
			frames[x].route |= ((uint64_t)1) << 62;
			frames[x].route >>= 2;
			frames[x].route |= ((uint64_t)2) << 62;
		}else if(x < (int)(frames_amount/6)*3){
			frames[x].route = ((uint64_t)3) << 62;
			frames[x].route >>= 2;
			frames[x].route |= ((uint64_t)1) << 62;
		}else if(x < (int)(frames_amount/6)*4){
			frames[x].route = ((uint64_t)3) << 62;
			frames[x].route >>= 2;
			frames[x].route |= ((uint64_t)1) << 62;
		}else if(x < (int)(frames_amount/6)*5){
			frames[x].route = ((uint64_t)3) << 62;
			frames[x].route >>= 2;
			frames[x].route |= ((uint64_t)2) << 62;
		}else if(x < (int)(frames_amount)){
			frames[x].route = ((uint64_t)3) << 62;
		}
	}
	return frames;
}

int forward_process_frame(HexagonPanel* hp, Frame *frame){
	switch(frame->route & (((uint64_t)3) << 62)){
		case 0:
			if(hp->peer_out[0] != NULL){
				frame->route <<= 2;
				return 0;
			}else{
				TraceLog(LOG_INFO, "Inferface 0 not found");
			}
		break;
		case (((uint64_t)1) << 62):
			if(hp->peer_out[1] != NULL){
				frame->route <<= 2;
				return 1;
				//send_payload(&(args->hexagon_panel->peer_out[1]->buffer_in[1]), frame);
			}else{
				TraceLog(LOG_INFO, "Interface 1 not found");
			}
		break;
		case (((uint64_t)2) << 62):
			if(hp->peer_out[2] != NULL){
				frame->route <<= 2;
				return 2;
				//send_payload(&(args->hexagon_panel->peer_out[2]->buffer_in[2]), frame);
			}else{
				TraceLog(LOG_INFO, "Interface 2 not found");
			}
		break;
		case (((uint64_t)3) << 62):
			//Process the package payload and display the content on the matrix
			//TraceLog(LOG_INFO, "%d, PAYLOAD %d %d %d @ %d", i++, frame->rgb_value[5].r, frame->rgb_value[5].g, frame->rgb_value[5].b, frame->route >> 62);
			process_rgb_values(hp, frame);
			return 3;
		break;
	}
}

void* controller_main(void* controller_args){
	Controller_args *args = (Controller_args *) controller_args;
	HexagonPanel *hp = args->nodes;
	HexagonPanel *master = args->master;

	int frame_size = 300;
	Frame *master_frames = generate_frames(frame_size);
	Frame *frame = (Frame*) malloc(sizeof(Frame));

	int frame_index = 0;

	while(1){
		//Send frame from Master to all nodes which are connected to
		master_propegate_frame(args->master, master_frames, frame_size, &frame_index);
		//__asm__("int $3");
		//Iterate through all the nodes
		for(int i = 0; i < args->nodes_amount; i++){
			for(int x = 0; x < 3; x++){
				if(ring_buffer_is_empty(hp[i].buffer_in[x])) continue;
				ring_buffer_pop(hp[i].buffer_in[x], frame);
		
				int edge = forward_process_frame(&hp[i], frame);
				if(edge != 3){
					ring_buffer_push(hp[i].peer_out[edge]->buffer_in[edge], *frame);
				}
				// Process or forward?
				// Process -> break out of this maybe?
				// Forward -> send_to_node(peer_out[n]->buffer_in[n])
			}
		}
	}
}

void master_propegate_frame(HexagonPanel *master, Frame *master_frames, int frame_size, int *frame_index){
	//for(int i = 0; i < 3; i++){
		//if(master->peer_out[1] == NULL) continue;

		RingBuffer *send_buffer = master->peer_out[1]->buffer_in[1];

		//if(ring_buffer_is_full(send_buffer)) continue;
		
		ring_buffer_push(send_buffer, master_frames[*frame_index]);
	//}
	*frame_index = (*frame_index + 1) % frame_size;
}

void node_controller(HexagonPanel *node){}
