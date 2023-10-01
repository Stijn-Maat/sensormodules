#include "Neighbor.h"
#include <stdbool.h>

#define FORGET_BUURMAN_TIME 20
#define MAX_NEIGHBORS 255
#define MAX_HOPS 16

Neighbor_t neighbors[MAX_NEIGHBORS];
uint8_t numNeighbors = 0;

uint8_t* get_neighbors(uint8_t *id) {
	for (uint8_t i = 0; i < numNeighbors; i++) {
		if (neighbors[i].id == *id) {
			return &(neighbors[i].id);
		}
	}
	return NULL;
}


void add_neighbor(Neighbor_t newNeighbor) {
	if (numNeighbors < MAX_NEIGHBORS) {
		neighbors[numNeighbors++] = newNeighbor;
		} 
		else {
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
//ontvangen en opdelen van het packet in packetbuffer
void received_packet(uint8_t *packet, uint8_t length, uint8_t receivePipe) {

	Neighbor_t newNeighbor;
	newNeighbor.id = packet[0];
	newNeighbor.hops = 0;
	newNeighbor.next_neighbor_id = 0;
	newNeighbor.valid_time = FORGET_BUURMAN_TIME;
	// hiervoor get_neigbour toevoegen voor selecteren of buur al bestaat of niet
	add_neighbor(newNeighbor);
	return;
}

//goed idee moet alleen nog uitgewerkt worden

/*bool add_next_neighbor(uint8_t destination_id, uint8_t next_neigbor_id, Neighbor_t newNeighbor){
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
}*/


//printen burenlijst
void print_neighbors() {
	printf("Burenlijst:\n");
	for (uint8_t i = 0; i < numNeighbors; i++) {
		printf("ID: %02X, Hops: %d, Valid Time: %d, Next Neighbor ID: %02X\n",
		neighbors[i].id, neighbors[i].hops, neighbors[i].valid_time, neighbors[i].next_neighbor_id);
	}
}

// functie voor het aftellen van de valid_time wanneer id niet meer relevant is
void count_down() {
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


