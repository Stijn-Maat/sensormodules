#include "neighbor.h"

// Define of Maximum number of neighbors
Neighbor_t neighbors[MAX_NEIGHBORS];

// Score of the current number of neighbors
uint8_t numNeighbors = 0;

// Hop_counter
volatile uint8_t hop_count = 0; // dynamisch maken


// Function to add a new Neighbor
uint8_t new_neighbor(uint8_t broad_id, uint8_t i, uint8_t broad_nodes, uint8_t *buf) {
	uint8_t	hops;
	
	// set neigbhor ID and valid_time
	neighbors[i].id = broad_id;
	neighbors[i].valid_time = FORGET_BUURMAN_TIME;
	// statement for adding via_id to Via_Neighbor_list
	for(uint8_t j=0;j<broad_nodes;j++){
		hops = *(buf+1+j*2);
		if (hops>neighbors[i].hopCount){
			neighbors[i].hopCount=hops;
		}
		if(hops<MAX_HOPS){
			if(neighbors[i].hopIndex[hops]<MAX_VIA_NEIGHBORS){
				neighbors[i].via_neighbor_id[hops][neighbors[i].hopIndex[hops]]=*(buf+j*2);
				neighbors[i].hopIndex[hops]++;
			}
		}
	}
	return 1;
}


// Function to get a snapshot of neighbors
uint8_t get_snapshot(uint8_t *data, uint8_t *node_amount){
	uint8_t  snap_index = 0;
	uint8_t node_count = 0;
	
	// Add neighbors ID to snapshot
	for(uint8_t i = 0; i < numNeighbors; i++){
		*(data+snap_index) = neighbors[i].id;
		snap_index++;
		node_count++;
		*(data+snap_index) = 0;
		snap_index++;
		
		// Check snapshot sizelimit
		if(snap_index ==29){
			*node_amount = node_count;
			return 1;
		}
		
		// Kijken naar Defines dynamisch maken zodat er geen krap in komt
		//Populate the snapshot with via_neighbors
		for (uint8_t j= 0; j < neighbors[i].hopCount; j++){
			for (uint8_t k = 0; k < neighbors[i].hopIndex[j]; k++){
				
				if(neighbors[i].via_neighbor_id[j][k] != 0){
					*(data+snap_index) = neighbors[i].via_neighbor_id[j][k];
					snap_index++;
					*(data+snap_index) = j+1;
					snap_index++;
				}
				// Check snapshot sizelimit
				if(snap_index ==29){
					*node_amount = node_count;
					return 1;
				}
			}
		}
		
	}
	*node_amount = snap_index;
	return 1;
}

// Function to add Neighbor
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

// Function to calculate routing to a destination
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

// Function to remove a neighbor
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

// Function to count down and remove a neighbors with expired times
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