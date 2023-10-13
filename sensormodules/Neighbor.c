#include "neighbor.h"

Neighbor_t neighbors[MAX_NEIGHBORS];
uint8_t numNeighbors = 0;

uint8_t new_neighbor(uint8_t broad_id, uint8_t i, uint8_t broad_nodes, uint8_t *buf) {
	uint8_t	hops;
	neighbors[i].id = broad_id;
	neighbors[i].valid_time = FORGET_BUURMAN_TIME;
	// statement for adding via_id to Via_Neighbor_list
	for(uint8_t j=0;j<broad_nodes;j++){
		hops = *(buf+1+j*2)
		if(hops<MAX_HOPS){
			if(neighbors[i].hopIndex[hops]<MAX_VIA_NEIGHBORS){
				neighbors[i].via_neighbor_id[hops][neighbors.hopIndex[hops]]=*(buf+j*2);
				neighbors[i].hopIndex[hops]++;
			}
		}
	}
	return 1;
}



uint8_t get_snapshot(uint8_t *data, uint8_t *node_amount, uint8_t *hops_amount){
	uint8_t  snap_index = 0;

	for(uint8_t i = 0; i < numNeighbors; i++){
		*(data+snap_index) = neighbors[i].id;
		snap_index++;
		
		uint8_t hops_count = 0;
		for (uint8_t j= 0; j < MAX_HOPS; j++){
			for (uint8_t k = 0; k < MAX_VIA_NEIGHBORS; k++){
				
				if(neighbors[i].via_neighbor_id[j][k] != 0){
					*(data+snap_index) = neighbors[i].via_neighbor_id[j][k];
					snap_index++;
					hops_count++;
				}
			}
			*(hops_amount + i) = hops_count;
		}
			*node_amount = snap_index;
	}
	return 1;
}

void add_neighbor(uint8_t broad_id,uint8_t broad_nodes, uint8_t *buf) {
	if (numNeighbors < MAX_NEIGHBORS) {
		if (numNeighbors>0){
			//loop for existing direct neighbors
			for (uint8_t i=0;i<numNeighbors;i++){
				//if broad id exists refresh via list and valid time
				if (broad_id == neighbors[i].id){
					if (new_neighbor(broad_id,i,broad_nodes,buf)){
						return;
					}
					
				}
			}
			new_neighbor(broad_id,numNeighbors,broad_nodes,buf);
			return;	
		}
		new_neighbor(broad_id,numNeighbors,broad_nodes,buf);
		return;
	}
	else {
		printf("Max neighbors reached.\n");
	}
	
}

uint8_t calculated_routing(uint8_t destination){
	uint8_t direct_id;
	//loop through lists of neighbors
	for (uint8_t i=0;i<numNeighbors;i++){
		// if direct id == destination, return direct neighbor id
		if (neighbors[i].id==destination){
			direct_id = neighbors[i].id;
			return direct_id;
		}else{
			// else loop via list of direct neighbor
			for (uint8_t j=0;j<MAX_HOPS;j++){
				for (uint8_t k=0;k<MAX_VIA_NEIGHBORS;k++){
					// if via id == destination, return direct neighbor id
					if (neighbors[i].via_neighbor_id[j][k]==destination){
						direct_id = neighbors[i].id;
						return direct_id;
					}
				}
			}
		}
	}
	// when not in list return 0
	return 0;
}

void remove_neighbor(uint8_t id) {
	for (uint8_t i = 0; i < numNeighbors; i++) {
		if (neighbors[i].id == id) {
			// Shift elements to remove the neighbor
			for (uint8_t j = i; j < numNeighbors - 1; j++) {
				neighbors[j] = neighbors[j + 1];
			}
			numNeighbors--;
			break;
		}
	}
}

void count_down(void){
	for (uint8_t i = FORGET_BUURMAN_TIME - 1; i >= 0; i--) {
		if (neighbors[i].valid_time > 0) {
			neighbors[i].valid_time--;
		}
		if (neighbors[i].valid_time == 0) {
			remove_neighbor(neighbors[i].id);
		}
		if (neighbors[i].valid_time == FORGET_BUURMAN_TIME + 1) {
			remove_neighbor(neighbors[i].id);
		}
	}
}
