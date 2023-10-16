#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

#include <stdio.h>

#define FORGET_BUURMAN_TIME 20
#define MAX_NEIGHBORS 30
#define MAX_VIA_NEIGHBORS 20
#define MAX_HOPS 16

typedef struct {
	uint8_t id;
	uint8_t hopCount;
	uint8_t hopIndex[MAX_HOPS];
	uint8_t via_neighbor_id[MAX_HOPS][MAX_VIA_NEIGHBORS];
	uint8_t valid_time;
} Neighbor_t;


void add_neighbor(uint8_t broad_id,uint8_t broad_nodes, uint8_t *buf);
uint8_t new_neighbor(uint8_t broad_id, uint8_t i, uint8_t broad_nodes, uint8_t *buf);
uint8_t get_snapshot(uint8_t *data, uint8_t *node_amount);
void remove_neighbor(uint8_t id);
void count_down(void);
uint8_t calculated_routing(uint8_t destination);
#endif /* NEIGHBOR_H_ */