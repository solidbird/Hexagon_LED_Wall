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
}

void* controller_main(void* controller_args){
	Controller_args *args = (Controller_args *) controller_args;
	HexagonPanel *hp = args->nodes;
	HexagonPanel *master = args->master;

	Frame *frame = (Frame*) malloc(sizeof(Frame));

	//TODO: timeout while loop has to run together with all nodes discovery phase
	master_discovery(master, hp, args->nodes_amount);
	//master_build_topology();
	//TODO: Build actual Frames out of the topology instead of doing this stuff
	int frame_size = 300;
	int frame_index = 0;
	Frame *master_frames = generate_frames(frame_size);
	while(1){
		//Send frame from Master to all nodes which are connected to
		//master_discovery(master);
		master_propegate_frame(master, master_frames, frame_size, &frame_index);
		node_controller(hp, args->nodes_amount, frame);
	}
}

void master_update_topology(HexagonPanel *master, Discovery_package *dp, int middle_offset_index, char *topology){
	int x = 0;
	int y = 0;
	uint64_t mask = ((uint64_t)1 << 63) | ((uint64_t)1 << 62);
	while((dp->route_edges & mask) != mask){
		//Translate X,Y coordinates in a linear memory [Y * TOPOLOGY_WIDTH + X]
		int topology_index = (y + middle_offset_index) * middle_offset_index * 2 + (x + middle_offset_index);
		topology[topology_index] = 'x';

		switch(dp->route_edges & (((uint64_t)3) << 62)){
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
		dp->route_edges <<= 2;
	}
}

void nodes_discovery(HexagonPanel *nodes, int nodes_amount, Discovery_package *dp){
	DataType type = TYPE_DISCOVERY_PACKAGE;
	for(int i = 0; i < nodes_amount; i++){
		for(int edge = 0; edge < 3; edge++){
			if(ring_buffer_is_empty(nodes[i].buffer_in[edge])) continue;
			ring_buffer_pop(nodes[i].buffer_in[edge], dp, &type);

			dp->route_edges >>= 2;
			dp->route_edges |= ((uint64_t)edge) << 62;

			ring_buffer_push(nodes[i].peer_in[edge]->buffer_out[edge], (BufferData*) dp, TYPE_DISCOVERY_PACKAGE);
			//TODO: Grab the thing and tag own edge to it 
		}
	}
}

int master_discovery(HexagonPanel *master, HexagonPanel *nodes, int nodes_amount){
	Discovery_package dp = {.route_edges = 0};
	Discovery_package *dp_nodes = (Discovery_package*) malloc(sizeof(Discovery_package));
	dp.route_edges = ~dp.route_edges;
	RingBuffer *send_buffer = master->peer_out[1]->buffer_in[1];
	ring_buffer_push(send_buffer, (BufferData*) &dp, TYPE_DISCOVERY_PACKAGE);
	Discovery_package* topology_info = malloc(sizeof(Discovery_package));
	DataType type = TYPE_DISCOVERY_PACKAGE;

	int middle_offset_index = 128;
	char *topology = (char*)malloc(sizeof(char) * (middle_offset_index * 2 * middle_offset_index * 2));

	for(int y = 0; y < middle_offset_index * 2; y++){
		for(int x = 0; x < middle_offset_index * 2; x++){
			topology[y * middle_offset_index * 2 + x] = '_';
		}
	}
	int timeout = 300;

	while(timeout){
		nodes_discovery(nodes, nodes_amount, dp_nodes);
		if(!ring_buffer_is_empty(master->buffer_out[1])){
			ring_buffer_pop(master->buffer_out[1], topology_info, &type);
			master_update_topology(master, topology_info, middle_offset_index, topology);
			timeout = 300;
		}else{
			timeout--;
		}
	}
}

void master_propegate_frame(HexagonPanel *master, Frame *master_frames, int frame_size, int *frame_index){
	RingBuffer *send_buffer = master->peer_out[1]->buffer_in[1];
	ring_buffer_push(send_buffer, (BufferData*) &master_frames[*frame_index], TYPE_FRAME);
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

void node_controller(HexagonPanel *nodes, int nodes_amount, Frame *frame){
	DataType type = TYPE_FRAME;
	//Iterate through all the nodes
	for(int i = 0; i < nodes_amount; i++){
		for(int x = 0; x < 3; x++){
			if(ring_buffer_is_empty(nodes[i].buffer_in[x])) continue;
			ring_buffer_pop(nodes[i].buffer_in[x], frame, &type);
	
			int edge = forward_process_frame(&nodes[i], frame);
			if(edge != 3){
				ring_buffer_push(nodes[i].peer_out[edge]->buffer_in[edge], (BufferData*) frame, TYPE_FRAME);
			}
			// Process or forward?
			// Process -> break out of this maybe?
			// Forward -> send_to_node(peer_out[n]->buffer_in[n])
		}
	}
}
