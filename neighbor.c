#include "Neighbor.h"
#include <stdio.h>
#include "core.h"

// Define of Maximum number of neighbors and Via Neighbors
static Neighbor_t neighbors[MAX_NEIGHBORS];
static Via_t vias[MAX_VIA_NEIGHBORS];

// Score of the current number of neighbors & vias
static uint8_t numNeighbors = 0;
static uint8_t numVias = 0;

// Static functions used only in c file
static void update_via(uint8_t id, uint8_t hops, uint8_t host);
static void remove_neighbor(uint8_t id);
static void remove_host_vias(uint8_t host);
static uint8_t update_neighbor_vias(uint8_t broad_id, uint8_t broad_nodes, uint8_t *buf);


uint8_t update_neighbor_vias(uint8_t broad_id, uint8_t broad_nodes, uint8_t *buf) {
	
	// Updating via list
	for(uint8_t i = 0; i < broad_nodes; i+=2){
		printf("%d\n", i);
		update_via(buf[i], buf[i+1] + 1, broad_id);
	}
	return 0;
}

void update_via(uint8_t id, uint8_t hops, uint8_t host){
	
	for(uint8_t i = 0; i < numVias; i++){
		if(vias[i].id == id){
			if(vias[i].hops >= hops){
				vias[i].hops = hops;
				vias[i].host = host;
			}
			return;
		}
	}
	
	vias[numVias].id = id;
	vias[numVias].hops = hops;
	vias[numVias].host = host;
	printf("id: %x, hops: %d, host: %d\n",id, hops, host);
	numVias++;
}

// Function to get a snapshot of neighbors
uint8_t get_snapshot(uint8_t *data) {
	uint8_t dataPingIndex = 2;
	
	data[0] = my_id;
	data[1] = SNAPSHOT;
	
	for(uint8_t i = 0; i < numNeighbors; i++, dataPingIndex+=2){
		data[dataPingIndex] = neighbors[i].id;
		data[dataPingIndex+1] = 0;
	}
	for(uint8_t via = 0; via < numVias; via++){
		for(uint8_t i = 0; i < numNeighbors; i++){
			if (neighbors[i].id == vias[via].id){
				break;
			}
			data[dataPingIndex] = vias[via].id;
			data[dataPingIndex+1] = vias[via].hops;
		}	
	}
	printf("Snapshot send");
	return 1;
}

// Function to store Neighbor
void add_neighbor(uint8_t broad_id, uint8_t broad_nodes, uint8_t *buf) {
	
	remove_host_vias(broad_id);
	
	if(numNeighbors >= MAX_NEIGHBORS){
		printf("Max neighbors reached.\n");
		return;	
	}
	
	uint8_t place = 0;
	
	// Loop voor bestaande directe buren
	for (; place < numNeighbors; place++) {
		// Als het broadcast-ID bestaat, vernieuw dan de via-lijst en geldige tijd
		if (broad_id == neighbors[place].id) {
			break;
		}
	}
	
	neighbors[place].id = broad_id;
	neighbors[place].valid_time = FORGET_BUURMAN_TIME;
	numNeighbors++; // Verhoog het aantal buren na het toevoegen van een nieuwe buurman

	// Als het broadcast-ID niet bestaat, voeg dan een nieuwe buurman toe
	update_neighbor_vias(broad_id, broad_nodes, buf);
	printf("Added new neighbor with ID: %d\n", broad_id);
}

// Function to calculate routing to a destination
uint8_t calculated_routing(uint8_t destination) {
      
	for(uint8_t i = 0; i< numNeighbors; i++){
		if(neighbors[i].id == destination){
			printf("%d\n", destination);
			return neighbors[i].id;
		}
	}
	
	for(uint8_t i = 0; i < numVias; i++){
		if (vias[i].id == destination){
			printf("%d\n", destination);
			return vias[i].id;
		}
	}
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
            printf("Neighbor removed: ID=%d\n", id);
            break;
        }
    }
}

void remove_host_vias(uint8_t host){
	
	for (uint8_t i = 0; i < numVias; i++){
		if(vias[i].host == host){
			for(uint8_t j = i; j < numVias-1; j++){
				vias[j] = vias[j+1];
			}
			numVias--;
		}
	}
}

void count_down() {
	for (uint8_t i = 0; i < numNeighbors; i++) {
		neighbors[i].valid_time--;
		if (neighbors[i].valid_time == 0) {
			printf("Neighbor expired: ID=%d\n", neighbors[i].id);
			remove_neighbor(neighbors[i].id);
			i--; // Decrement i as the array size has changed
			numNeighbors--;
		}
	}
}

