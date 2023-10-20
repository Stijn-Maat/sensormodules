#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

#include <stdio.h>

#define FORGET_BUURMAN_TIME 20
#define MAX_NEIGHBORS 253
#define MAX_VIA_NEIGHBORS 253
#define MAX_HOPS (vias/vias[0] + 2)
#define SNAPSHOT 0 
#define UPDATE 1

typedef struct {
	uint8_t id;
	uint8_t host;
	uint8_t hops;
} Via_t;

typedef struct{
	uint8_t id;
	uint8_t valid_time;
} Neighbor_t;



void add_neighbor(uint8_t broad_id,uint8_t broad_nodes, uint8_t *buf);
uint8_t get_snapshot(uint8_t *data);
uint8_t calculated_routing(uint8_t destination);
void count_down();

#endif /* NEIGHBOR_H_ */
