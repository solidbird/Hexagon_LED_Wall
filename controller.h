#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "routing.h"
#include "hexagon.h"
#include <pthread.h>

typedef enum {
	discovery_phase,
	run_phase
} Master_state;

typedef struct Controller_args{
	HexagonPanel *master;
	HexagonPanel *nodes;
	int nodes_amount;
} Controller_args;

void* controller_main(void* controller_args);
Frame* generate_frames(int frames_amount);
void master_propegate_frame(HexagonPanel *master, Frame *master_frames, int frame_size, int *frame_index);
void node_controller(HexagonPanel *node, int nodes_amount, Frame *frame);
void master_controller(HexagonPanel *master, Master_state *state);

#endif
