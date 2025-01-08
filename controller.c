#include "controller.h"
#include "hexagon.h"
#include <time.h>


#define offset_topology(x, y) (MIDDLE_OFFSET_INDEX + y) * MIDDLE_OFFSET_INDEX * 2 + (MIDDLE_OFFSET_INDEX + x)

void generate_frames(Scene *scene, int frames_amount){

	for(size_t x = 0; x < frames_amount; x++){
		for(size_t i = 0; i < 127; i++){
			scene[x].rgb_value[i] = (RGB_Value){rand() % 255, rand() % 255, rand() % 255};
		}

		if(x%5 == 0){
			scene[x].destination = (Vector2){0,0};
		}else if(x%5 == 1){

			scene[x].destination = (Vector2){0,1};
		}else if(x%5 == 2){

			scene[x].destination = (Vector2){0,2};
		}else if(x%5 == 3){

			scene[x].destination = (Vector2){1,1};
		}else if(x%5 == 4){

			scene[x].destination = (Vector2){1,0};
		}
	}
}

void asc_sort_edges_cost(Edge_to_node_cost arr[]) {
   Edge_to_node_cost temp;

    // Compare and swap arr[0] and arr[1] if needed
    if (arr[0].node_cost > arr[1].node_cost) {
        temp = arr[0];
        arr[0] = arr[1];
        arr[1] = temp;
    }

    // Compare and swap arr[1] and arr[2] if needed
    if (arr[1].node_cost > arr[2].node_cost) {
        temp = arr[1];
        arr[1] = arr[2];
        arr[2] = temp;
    }

    // Compare and swap arr[0] and arr[1] again
    if (arr[0].node_cost > arr[1].node_cost) {
        temp = arr[0];
        arr[0] = arr[1];
        arr[1] = temp;
    }
}

void calculate_slidding_window_average(Topology_node *node){
	int sum = 0;
	for(int i = 0; i < 5; i++){
		sum += node->slidding_window[i];
	}
	node->node_cost = sum/5;
}

void update_topology_node(Topology_node *node, int x, int y){
	node[offset_topology(x,y)].slidding_window[node[offset_topology(x,y)].head] = 1;
	node[offset_topology(x,y)].head = (node[offset_topology(x,y)].head + 1) % 5;
	calculate_slidding_window_average(&node[offset_topology(x,y)]);
}

void package_frames(Frame *frames, int frames_amount, Topology_node *topology, Scene *scene){
	/*
		- Take each scene frame and calculate the poisition of each pixel with the color
		to be on the hexagon panels (lay piuctute or videos on top of the hexagon panels)

		- Have a bunch of Frame packages ready
			- we can run the alogrithm without even sending packages out yet to prepare the routes for each
			package. (Might take a lot of time otherwise just go and calculate the algorithm everyonce in a while)

		- return frames
	*/
	
	// 0. Take costs into account prefer always the node that is that has the lowest costs
	// 1. Go to Y as fast as possible (if possible)
	// 2. Then take edge 1 priority (so go to X as fast as possible)	
	for(int i = 0; i < frames_amount; i++){
		frames[i].route |= ((uint64_t)3) << 62;
		int x = 0;
		int y = 0;
		for(int i = 0; i < MIDDLE_OFFSET_INDEX*2 * MIDDLE_OFFSET_INDEX*2; i++){
			if(!topology[i].node_available) continue;
			topology[i].head = (topology[i].head + 1) % 5;
			topology[i].slidding_window[topology[i].head] = 0;
		}
		if(scene[i].destination.x == 0 && scene[i].destination.y == 0){
			for(int x = 0; x < 127; x++){
				frames[i].rgb_value[x] = (RGB_Value){scene[i].rgb_value[x].r, scene[i].rgb_value[x].g, scene[i].rgb_value[x].b};
			}
			continue;
		}

		while(scene[i].destination.x != x || scene[i].destination.y != y){

			int peak_index[] = {offset_topology(x+1,y-1), offset_topology(x+1,y), offset_topology(x,y+1)};
			calculate_slidding_window_average(&topology[peak_index[0]]);
			calculate_slidding_window_average(&topology[peak_index[1]]);
			calculate_slidding_window_average(&topology[peak_index[2]]);
			Edge_to_node_cost sorted_edges[] = {
				(Edge_to_node_cost){0,topology[peak_index[0]].node_cost},
				(Edge_to_node_cost){1,topology[peak_index[1]].node_cost},
				(Edge_to_node_cost){2,topology[peak_index[2]].node_cost}
			};

			int possible_edges[] = {0,0,0};

			asc_sort_edges_cost(sorted_edges);

			for(int i = 0; i < 2; i++){
				if(!topology[peak_index[i]].node_available){
					possible_edges[i] = -1;
				}
			}

			if(sorted_edges[0].node_cost != sorted_edges[1].node_cost && sorted_edges[1].node_cost != sorted_edges[2].node_cost && sorted_edges[0].node_cost != sorted_edges[2].node_cost){
				int edge_to_take = -1;
				for(int i = 0; i < 3; i++){
					if(possible_edges[sorted_edges[i].edge] != -1){
						edge_to_take = sorted_edges[i].edge;
						break;
					}
				}
				if(edge_to_take != -1){
					//TAKE THAT EDGE AND CONTINUE WITH MAIN WHILE LOOP
					frames[i].route >>= 2;
					switch(edge_to_take){
						case 0:
							x++;
							y--;
							frames[i].route |= ((uint64_t)0) << 62;
						break;
						case 1:
							x++;
							frames[i].route |= ((uint64_t)1) << 62;
						break;
						case 2:
							y++;
							frames[i].route |= ((uint64_t)2) << 62;
						break;
					}
					continue;
					update_topology_node(topology, x, y);
					
					for(int x = 0; x < 127; x++){
						frames[i].rgb_value[x] = (RGB_Value){scene[i].rgb_value[x].r, scene[i].rgb_value[x].g, scene[i].rgb_value[x].b};
					}
				}
			}else{
				if(y > scene[i].destination.y){
					if(possible_edges[sorted_edges[0].edge] != -1){
						//TAKE THAT EDGE AND CONTINUE WITH MAIN WHILE LOOP
						x++;
						y--;
						update_topology_node(topology, x, y);
						frames[i].route >>= 2;
						frames[i].route |= ((uint64_t)0) << 62;
						for(int x = 0; x < 127; x++){
							frames[i].rgb_value[x] = (RGB_Value){scene[i].rgb_value[x].r, scene[i].rgb_value[x].g, scene[i].rgb_value[x].b};
						}
						continue;
					}
				}else if(y < scene[i].destination.y){
					if(possible_edges[sorted_edges[2].edge] != -1){
						//TAKE THAT EDGE AND CONTINUE WITH MAIN WHILE LOOP
						y++;
						update_topology_node(topology, x, y);
						frames[i].route >>= 2;
						frames[i].route |= ((uint64_t)2) << 62;
						for(int x = 0; x < 127; x++){
							frames[i].rgb_value[x] = (RGB_Value){scene[i].rgb_value[x].r, scene[i].rgb_value[x].g, scene[i].rgb_value[x].b};
						}
						continue;
					}
				}

				if(possible_edges[sorted_edges[1].edge] != -1){
					//TAKE THAT EDGE AND CONTINUE WITH MAIN WHILE LOOP
					x++;
					update_topology_node(topology, x, y);
					frames[i].route >>= 2;
					frames[i].route |= ((uint64_t)1) << 62;
					for(int x = 0; x < 127; x++){
						frames[i].rgb_value[x] = (RGB_Value){scene[i].rgb_value[x].r, scene[i].rgb_value[x].g, scene[i].rgb_value[x].b};
					}
					continue;
				}
			}

			//TODO: Check if even a node is at that peak location if not than just dont look at it
/*			if(topology[peak_edge0_index].node_cost <= topology[peak_edge1_index].node_cost){
				if(topology[peak_edge0_index].node_cost < topology[peak_edge2_index].node_cost){
					//Go with edge 0
				}else{
					//Go with edge 2
				}
			}else{
				if(topology[peak_edge1_index].node_cost < topology[peak_edge2_index].node_cost){
					//Go with edge 1
				}else{
					//Go with edge 2
				}
			}
			
			//TODO: We want to only do this for one iteration and check the above rule as well before going on with the next iteration
			while(y != sceen->destination.y){
				if(!topology[peak_edge2_index].node_available){
					while(x != sceen->destination){
						if(!topology[peak_edge1_index].node_available){

						}
					}
				}
				if(y < sceen->destination.y){
					y++;
				}else if(y > sceen->destination.y){ //TODO: Check for node_available at edge 0;
					x++;
					y--;
				}
			}*/

		}
	}
}

int forward_process_frame(HexagonPanel* hp, Frame *frame){
	switch(frame->route & (((uint64_t)3) << 62)){
		case 0:
			if(hp->peer_out[0] != NULL){
				frame->route <<= 2;
				//TraceLog(LOG_INFO, "FORWARD 0");
				return 0;
			}else{
				TraceLog(LOG_INFO, "Inferface 0 not found");
			}
		break;
		case (((uint64_t)1) << 62):
			if(hp->peer_out[1] != NULL){
				frame->route <<= 2;
				//TraceLog(LOG_INFO, "FORWARD 1");
				return 1;
			}else{
				TraceLog(LOG_INFO, "Interface 1 not found");
			}
		break;
		case (((uint64_t)2) << 62):
			if(hp->peer_out[2] != NULL){
				frame->route <<= 2;
				//TraceLog(LOG_INFO, "FORWARD 2");
				return 2;
			}else{
				TraceLog(LOG_INFO, "Interface 2 not found");
			}
		break;
		case (((uint64_t)3) << 62):
			//Process the package payload and display the content on the matrix
			//TraceLog(LOG_INFO, "PROCESS");
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
	Scene *scene = (Scene*) malloc(sizeof(Scene) * frame_size);

	Frame *frame = (Frame*) malloc(sizeof(Frame));
	Frame *master_frames = (Frame*) malloc(sizeof(Frame) * frame_size);
	Topology_node *topology = (Topology_node*)malloc(sizeof(Topology_node) * (MIDDLE_OFFSET_INDEX * 2 * MIDDLE_OFFSET_INDEX * 2));
	
	for(int y = -MIDDLE_OFFSET_INDEX; y < MIDDLE_OFFSET_INDEX; y++){
		for(int x = -MIDDLE_OFFSET_INDEX; x < MIDDLE_OFFSET_INDEX; x++){
			int index = offset_topology(x, y);
			topology[index].node_available = 0;
			if(x == 0 && y == 0){
				topology[index].node_cost = -1;
			}else{
				topology[index].node_cost = 0;
			}
			topology[index].head = 0;
			for(int i = 0; i < 5; i++){
				topology[index].slidding_window[i] = 0;
			}
		}
	}
	//TOxDO: timeout while loop has to run together with all nodes discovery phase
	master_discovery(master, hp, args->nodes_amount, topology);
	//TODO: Build actual Frames out of the topology instead of doing this stuff
	int frame_index = 0;
	generate_frames(scene, frame_size);
	package_frames(master_frames, frame_size, topology, scene);
	while(1){
		//Send frame from Master to all nodes which are connected to
		master_propegate_frame(master, master_frames, frame_size, &frame_index);
		node_controller(hp, args->nodes_amount, frame);
		
		/*TraceLog(LOG_INFO, "(0,0): %f", topology[offset_topology(0,0)].node_cost);
		TraceLog(LOG_INFO, "(0,1): %f", topology[offset_topology(0,1)].node_cost);
		TraceLog(LOG_INFO, "(0,2): %f", topology[offset_topology(0,2)].node_cost);
		TraceLog(LOG_INFO, "(1,1): %f", topology[offset_topology(1,1)].node_cost);
		TraceLog(LOG_INFO, "(1,0): %f", topology[offset_topology(1,0)].node_cost);*/
	}
}

void master_update_topology(HexagonPanel *master, Discovery_package *dp, Topology_node *topology){
	int x = 0;
	int y = 0;
	int skip_package = 0;
	// 10 01 00 01 11 11 11 (11)
	// dp->route_edges <<= (64-i)

	// i = 0..31: 
	// 		mask = (uint64_t)1 << 2*(i*2+1) | (uint64_t)1 << (i*2)
	//		if (dp->route_edges & mask) == mask: continue;
	//		dp->route_edges >> ((i*2))
	//		case 0:
	//		case 1:
	//		case 2:
	//uint64_t mask = ((uint64_t)1 << 63) | ((uint64_t)1 << 62);
	for(int i = 0; i < 32; i++){
		uint64_t mask = ((uint64_t)1 << (i*2+1) | ((uint64_t)1 << (i*2)));
		if((dp->route_edges & mask) == mask){
			skip_package = 1;
			continue;
		}

		switch((dp->route_edges & mask) >> (i*2)) {
			// look if topology is already set at case if not then set
			case 0:
				y--; x++;
			break;
			case 1:
				x++;
			break;
			case 2:
				y++;
			break;
		}

		if(skip_package){
			skip_package = 0;
			x = 0;
			y = 0;
		}

		int topology_index = (y + MIDDLE_OFFSET_INDEX) * MIDDLE_OFFSET_INDEX * 2 + (x + MIDDLE_OFFSET_INDEX);
		topology[topology_index].node_available = 1;
	}
}

void nodes_discovery(HexagonPanel *nodes, int nodes_amount, Discovery_package *dp){
	/*
		- init array of 3 values [0], [1], [2] representing the edges and set the values initially boolean (0)
		- if !checked[0] || !checked[1] || !checked[2]
			- tag your edge and checked[edge] = 1 and send package backwards to that input edge (or prefered input edge 1) and propegate to all edges to output
		- else
			- just forward the package accros one of the checked edges in the priority of taking edge 1 always

		TODO: How can we eventually switch from this mode to actual run mode?
				(suggestion just also small timeout for each node since the initial packages are very close)
				or (Differentiate packages "Frame", "Discovery_package")
		
		TOxDO: Grab the thing and tag own edge to it	
	*/
	DataType type = TYPE_DISCOVERY_PACKAGE;
	for(int i = 0; i < nodes_amount; i++){
		for(int edge = 0; edge < 3; edge++){
			if(!ring_buffer_is_empty(nodes[i].buffer_in[edge])){
				ring_buffer_pop(nodes[i].buffer_in[edge], dp, &type);

				if(!nodes[i].neighbor_parent_available[0] || !nodes[i].neighbor_parent_available[1] || !nodes[i].neighbor_parent_available[2]){
					nodes[i].neighbor_parent_available[edge] = 1;

					dp->route_edges >>= 2;
					dp->route_edges |= ((uint64_t)edge) << 62;

					if(nodes[i].neighbor_parent_available[1]){
						ring_buffer_push(nodes[i].peer_in[1]->buffer_out[1], (BufferData*) dp, TYPE_DISCOVERY_PACKAGE);		
					}else if(nodes[i].neighbor_parent_available[0]){
						ring_buffer_push(nodes[i].peer_in[0]->buffer_out[0], (BufferData*) dp, TYPE_DISCOVERY_PACKAGE);		
					}else if(nodes[i].neighbor_parent_available[2]){
						ring_buffer_push(nodes[i].peer_in[2]->buffer_out[2], (BufferData*) dp, TYPE_DISCOVERY_PACKAGE);		
					}				
	

					if(nodes[i].peer_out[0] != NULL){
						ring_buffer_push(nodes[i].peer_out[0]->buffer_in[0], (BufferData*) dp, TYPE_DISCOVERY_PACKAGE);		
					}
					if(nodes[i].peer_out[1] != NULL){
						ring_buffer_push(nodes[i].peer_out[1]->buffer_in[1], (BufferData*) dp, TYPE_DISCOVERY_PACKAGE);		
					}
					if(nodes[i].peer_out[2] != NULL){
						ring_buffer_push(nodes[i].peer_out[2]->buffer_in[2], (BufferData*) dp, TYPE_DISCOVERY_PACKAGE);		
					}
				}
			}
			if(!ring_buffer_is_empty(nodes[i].buffer_out[edge])){
				ring_buffer_pop(nodes[i].buffer_out[edge], dp, &type);
				if(nodes[i].neighbor_parent_available[1]){
					ring_buffer_push(nodes[i].peer_in[1]->buffer_out[1], dp, TYPE_DISCOVERY_PACKAGE);
				}else if(nodes[i].neighbor_parent_available[0]){
					ring_buffer_push(nodes[i].peer_in[0]->buffer_out[0], dp, TYPE_DISCOVERY_PACKAGE);
				}else if(nodes[i].neighbor_parent_available[2]){
					ring_buffer_push(nodes[i].peer_in[2]->buffer_out[2], dp, TYPE_DISCOVERY_PACKAGE);
				}
			}
		}
	}
}

int master_discovery(HexagonPanel *master, HexagonPanel *nodes, int nodes_amount, Topology_node* topology){
	Discovery_package dp = {.route_edges = 0};
	Discovery_package *dp_nodes = (Discovery_package*) malloc(sizeof(Discovery_package));
	dp.route_edges = ~dp.route_edges;
	RingBuffer *send_buffer = master->peer_out[1]->buffer_in[1];
	ring_buffer_push(send_buffer, (BufferData*) &dp, TYPE_DISCOVERY_PACKAGE);
	Discovery_package* topology_info = malloc(sizeof(Discovery_package));
	DataType type = TYPE_DISCOVERY_PACKAGE;

	
	int timeout = 300;

	while(timeout){
		nodes_discovery(nodes, nodes_amount, dp_nodes);
		if(!ring_buffer_is_empty(master->buffer_out[1])){
			ring_buffer_pop(master->buffer_out[1], topology_info, &type);
			master_update_topology(master, topology_info, topology);
			timeout = 300;
		}else{
			timeout--;
		}
	}
	printf("\n\n---------------\n   TOPOLOGY:   \n---------------\n\n");	
	for(int y = -5; y < 10; y++){
		for(int x = -5; x < 10; x++){
			if(topology[(y + MIDDLE_OFFSET_INDEX) * MIDDLE_OFFSET_INDEX * 2 + (x + MIDDLE_OFFSET_INDEX)].node_available){
				printf("(%d,%d)",x ,y);
			}else{
				printf("     ");
			}
		}
		printf("\n");
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
