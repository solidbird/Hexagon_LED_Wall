#include "controller.h"
#include "hexagon.h"
#include <time.h>

Frame* generate_frames(int frames_amount){
	Frame* frames = (Frame*) malloc(sizeof(Frame) * frames_amount);

	for(size_t x = 0; x < frames_amount; x++){
		for(size_t i = 0; i < 127; i++){
			frames[x].rgb_value[i] = (RGB_Value){rand() % 255, rand() % 255, rand() % 255};
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
			process_rgb_values(hp, frame);
			return 3;
		break;
	}
}

void master_build_topology(){
	Discovery_package dp[8];
	char **topology = NULL;
	const int offset_index = 128; 

	for(int i = 0; i < 8; i++){
		dp[i].route_edges = 0;
	}
	topology = malloc(sizeof(char*) * (offset_index * 2));
	for(size_t i = 0; i < offset_index * 2; i++){
		topology[i] = malloc(sizeof(char) * (offset_index * 2));
	}

	for(int y = 0; y < offset_index * 2; y++){
		for(int x = 0; x < offset_index * 2; x++){
			topology[y][x] = '_';
		}
	}

	dp[0].route_edges |= (((uint64_t)1) << 62);
	dp[0].route_edges |= (((uint64_t)3) << 60);

	dp[1].route_edges |= (((uint64_t)2) << 62);
	dp[1].route_edges |= (((uint64_t)1) << 60);
	dp[1].route_edges |= (((uint64_t)3) << 58);

	dp[2].route_edges |= (((uint64_t)2) << 62);
	dp[2].route_edges |= (((uint64_t)2) << 60);
	dp[2].route_edges |= (((uint64_t)1) << 58);
	dp[2].route_edges |= (((uint64_t)3) << 56);
	
	dp[3].route_edges |= (((uint64_t)1) << 62);
	dp[3].route_edges |= (((uint64_t)2) << 60);
	dp[3].route_edges |= (((uint64_t)1) << 58);
	dp[3].route_edges |= (((uint64_t)3) << 56);
	
	dp[4].route_edges |= (((uint64_t)1) << 62);
	dp[4].route_edges |= (((uint64_t)2) << 60);
	dp[4].route_edges |= (((uint64_t)1) << 58);
	dp[4].route_edges |= (((uint64_t)3) << 56);

	dp[5].route_edges |= (((uint64_t)2) << 62);
	dp[5].route_edges |= (((uint64_t)1) << 60);
	dp[5].route_edges |= (((uint64_t)1) << 58);
	dp[5].route_edges |= (((uint64_t)3) << 56);

	dp[6].route_edges |= (((uint64_t)0) << 62);
	dp[6].route_edges |= (((uint64_t)2) << 60);
	dp[6].route_edges |= (((uint64_t)1) << 58);
	dp[6].route_edges |= (((uint64_t)3) << 56);
	
	dp[7].route_edges |= (((uint64_t)1) << 62);
	dp[7].route_edges |= (((uint64_t)1) << 60);
	dp[7].route_edges |= (((uint64_t)3) << 58);

	//__asm__("int $3");

	for(size_t i = 0; i < 8; i++){
		int x = 0;
		int y = 0;

		while(dp[i].route_edges != (((uint64_t)3) << 62)){

			topology[y + offset_index][x + offset_index] = 'x';

			switch(dp[i].route_edges & (((uint64_t)3) << 62)){
				// look if topology is already set at case if not then set
				case 0:
					y++; x--;
				break;
				case (((uint64_t)1) << 62):
					x++;
				break;
				case (((uint64_t)2) << 62):
					y++;
				break;
			}
			dp[i].route_edges <<= 2;
		}
	}
	// timeout after X sec. if no packages arrived to switch to next state
	for(int y = 0; y < 4; y++){
		for(int x = 0; x < 4; x++){
			printf("%c", topology[y + offset_index][x + offset_index]);
		}
		printf("\n");
	}
	//__asm__("int $3");
}

void* controller_main(void* controller_args){
	Controller_args *args = (Controller_args *) controller_args;
	HexagonPanel *hp = args->nodes;
	HexagonPanel *master = args->master;

	int frame_size = 300;
	Frame *master_frames = generate_frames(frame_size);
	Frame *frame = (Frame*) malloc(sizeof(Frame));

	int frame_index = 0;
	Discovery_package *dp;

	master_build_topology();

	while(1){
		//Send frame from Master to all nodes which are connected to
		master_propegate_frame(args->master, master_frames, frame_size, &frame_index);
		
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
	RingBuffer *send_buffer = master->peer_out[1]->buffer_in[1];
	ring_buffer_push(send_buffer, master_frames[*frame_index]);
	*frame_index = (*frame_index + 1) % frame_size;
}

void master_controller(HexagonPanel *master, Master_state *state){
	switch(*state){
		case discovery_phase:
			// Here we build the topology by going through the packages received by the discovery_phase
		break;
		case run_phase:
			// This is the actual process of propegating the packages laster according to the topology map
			// and dijkstra 
		break;
	}
}

void node_controller(HexagonPanel *node){}
