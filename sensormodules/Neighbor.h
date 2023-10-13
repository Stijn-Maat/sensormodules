#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

#include <stdio.h>

#define FORGET_BUURMAN_TIME 20
#define MAX_NEIGHBORS 255
#define MAX_VIA_NEIGHBORS 20
#define MAX_HOPS 16

typedef struct {
	uint8_t id;
	uint8_t hopIndex[MAX_HOPS];
	uint8_t via_neighbor_id[MAX_HOPS][MAX_VIA_NEIGHBORS];
	uint8_t valid_time;
} Neighbor_t;


void add_neighbor(Neighbor_t newNeighbor);
void remove_neighbor(uint8_t id);
void count_down(void);
uint8_t calculated_routing(uint8_t destination);
#endif /* NEIGHBOR_H_ */
