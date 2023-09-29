#include "Neighbor.h"
#include <stdbool.h>

#define FORGET_BUURMAN_TIME 20
#define MAX_NEIGHBORS 255
#define MAX_HOPS 16

Neighbor_t neighbors[MAX_NEIGHBORS];
uint8_t numNeighbors = 0;

bool is_id_in_neighbors(uint8_t id) {
	for (uint8_t i = 0; i < numNeighbors; i++) { // Iterate only over valid neighbors
		if (neighbors[i].id == id) {
			return true;
		}
	}
	return false;
}

void add_neighbor(Neighbor_t newNeighbor) {
	if (numNeighbors < MAX_NEIGHBORS) {
		neighbors[numNeighbors++] = newNeighbor;
		} else {
		printf("Max neighbors reached.\n");
	}
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

void received_packet(uint8_t *packet, uint8_t length, uint8_t receivePipe) {

	Neighbor_t newNeighbor;
	newNeighbor.id = packet[0];
	newNeighbor.hops = 0;
	newNeighbor.next_neighbor_id = 0;
	newNeighbor.valid_time = FORGET_BUURMAN_TIME;
	add_neighbor(newNeighbor);
	return;
}

bool add_next_neighbor(uint8_t destination_id, uint8_t next_neigbor_id, Neighbor_t newNeighbor){
	uint8_t send = 1;
	next_neigbor_id = newNeighbor.id;
	
	//Give the ID from the destination
	printf("I want to send a message to: ");
	fgets((char*) destination_id, sizeof(destination_id), stdin);

	printf("You typed: %c", destination_id);
	
	//When destination received, request to the neighbor to send their neighborlist
	if (destination_id == UART_NO_DATA){
		nrfStopListening();
		nrfWrite(send,sizeof(send));
		nrfStartListening();
	}
	
	//Send neighborlist when 1 is received
	if (packetBuffer == 1){
		nrfStopListening();
		for (uint8_t i = 0; i < numNeighbors; i++){
			nrfWrite(&neighbors[i].id, sizeof(neighbors[i].id));
		}
		nrfStartListening();
	}
	
	//Saves next neighbor ID
	if (packetBuffer != neighbors[0].id){
		for (uint8_t i = 0; i < numNeighbors; i++) { // Iterate only over valid neighbors
			if (neighbors[i].next_neighbor_id == next_neigbor_id) {
				return true;
			}
		}
	}
	return false;
}

//iemand wil naar sensor ?? iets sturen
//aan de directe buren wordt gevraagd of die sensor ?? kent (de burenlijst van de directe buur wordt opgehaald)
//kent de directe buur hem niet, dan wordt aan zijn buur gevraagd of die hem kent
//ja
//nee
//etc.

void cal_hops(uint8_t source_id, uint8_t neighbor_id, uint8_t destination_id) {
	// Implement your logic for calculating hops here
	// You need to consider the current neighbor list to calculate hops
	
	// Create a queue for BFS traversal
	uint8_t queue[MAX_NEIGHBORS];
	bool visited[MAX_NEIGHBORS];
	
	// Initialize the queue and visited array
	for (uint8_t i = 0; i < numNeighbors; i++) {
		queue[i] = 0;
		visited[i] = false;
	}

	// Start BFS algorithm
	uint8_t front = 0;
	uint8_t rear = 0;
	uint8_t current_id = source_id = MY_ID;
	uint8_t hops = 0;

	while (current_id != destination_id && front <= rear) {
		current_id = queue[front++];
		visited[current_id] = true;

		// Explore neighbors of the current node
		for (uint8_t i = 0; i < numNeighbors; i++) {
			if (!visited[i] && neighbors[i].id == current_id) {
				// Add neighbors to the queue
				queue[++rear] = neighbors[i].next_neighbor_id;
			}
		}

		// Increment the number of hops
		hops++;
	}

	if (current_id != destination_id) {
		printf("No path found from %02X to %02X\n", source_id, destination_id);
	}
}

void print_neighbors() {
	printf("Burenlijst:\n");
	for (uint8_t i = 0; i < numNeighbors; i++) {
		printf("ID: %02X, Hops: %d, Valid Time: %d, Next Neighbor ID: %02X\n",
		neighbors[i].id, neighbors[i].hops, neighbors[i].valid_time, neighbors[i].next_neighbor_id);
	}
}
