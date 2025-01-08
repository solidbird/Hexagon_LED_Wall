#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "routing.h"
#include "hexagon.h"
#include <pthread.h>

#define MIDDLE_OFFSET_INDEX 64

typedef enum {
	discovery_phase,
	run_phase
} Master_state;

typedef struct Controller_args{
	HexagonPanel *master;
	HexagonPanel *nodes;
	int nodes_amount;
} Controller_args;

typedef struct Topology_node{
	uint8_t node_available;
	int slidding_window[5];
	int head;
	double node_cost;
} Topology_node;

typedef struct Edge_to_node_cost{
	uint8_t edge;
	double node_cost;
} Edge_to_node_cost;

void* controller_main(void* controller_args);

void generate_frames(Scene *scene, int frames_amount);
void master_propegate_frame(HexagonPanel *master, Frame *master_frames, int frame_size, int *frame_index);
void node_controller(HexagonPanel *node, int nodes_amount, Frame *frame);
void master_controller(HexagonPanel *master, Master_state *state);

#endif
