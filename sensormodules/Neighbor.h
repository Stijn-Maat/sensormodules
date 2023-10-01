#ifndef NEIGHBORH
#define NEIGHBORH

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "serialF0.h"

#define MY_ID 0x47 // A = 0x41, B = 0x42, C = 0x43

typedef struct {
	uint8_t id;
	uint8_t hops;
	uint8_t next_neighbor_id;
	uint8_t valid_time;
} Neighbor_t;


uint8_t* get_neighbors(uint8_t *id);
void remove_neighbor(uint8_t id);
void received_packet(uint8_t *packet, uint8_t length, uint8_t receivePipe);
void add_neighbor(Neighbor_t newNeighbor);
void cal_hops(uint8_t source_id, uint8_t neighbor_id, uint8_t destination_id);
bool add_next_neighbor(uint8_t destination_id, uint8_t next_neighbor_id, Neighbor_t newNeighbor);

#endif /* NEIGHBOR */
