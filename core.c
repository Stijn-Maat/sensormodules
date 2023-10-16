/*
 * This file contains:
 *
 */

#include "core.h"

#define NRF_CHANNEL 54  //ISO = 38
#define BROAD_ADDR '4','2','0','B',0x00
#define BASE_ADDR_P '4','2','0','P'

#define BASE_STATION 0x20

uint8_t broadcast_pipe[5] = {BROAD_ADDR}; 
	
uint8_t buttonPressed = 0;
	
/* Initialization for nRF24L01 */
void init_nrf(void){
	
	nrfspiInit();										// Initialize SPI
	nrfBegin();											// Initialize radio module
	nrfSetPALevel(NRF_RF_SETUP_PWR_18DBM_gc);			// Power Control (-18, -6 and 0 dBm)
	nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);			// Data Rate: 250 Kbps
	nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);				// CRC Check
	nrfSetChannel(NRF_CHANNEL);							// Channel: 54
	nrfSetAutoAck(0);									// Auto Acknowledge on
	nrfEnableDynamicPayloads();							// Enable Dynamic Payloads
	nrfClearInterruptBits();							// Clear interrupt bits
	nrfFlushRx();										// Flush fifo's
	nrfFlushTx();
	
	nrfOpenReadingPipe(0, broadcast_pipe);	// Open the Broadcast reading pipe 
	nrfStartListening();
	
								
}

/* Generate an ID based on GPIO */
uint8_t get_my_id(void){
	
	PORTD.DIRCLR   = PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm|PIN4_bm;
	PORTD.PIN0CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN1CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN2CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN3CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN4CTRL = PORT_OPC_PULLDOWN_gc;
	
	uint8_t id;
	id=(TEAM_NUMBER<<5)|(PORTD.IN & 0b11111);			// 001-00000 is default address
	
	uint8_t receive_pipe[5] = {BASE_ADDR_P,id};			
	nrfOpenReadingPipe(1, receive_pipe);				// Open a personal pipe
	
	return id;
}

/* 1 seccond timer for POL */
void init_timer(void){
	
	TCE0.CTRLA    = TC_CLKSEL_DIV64_gc; // prescaling 64
	TCE0.PER      = 0XFFFF;				// Max value			
}

/* Ping of Life: get "via" data ready for sending 
 * id = my_id, data = device id + hops, len = lenght of data
 */
void send_snap(void){    
	
	uint8_t buf[32];
	uint8_t data[30];
	uint8_t node_amount;
	
	get_snapshot(data, &node_amount);
	 
	uint8_t len = sizeof(data) / sizeof(data[0]);
	
	buf[0] = my_id;
	buf[1] = node_amount & 0x7F;	//MSB = 0, remainder = amount of nodes 
	
	memcpy(buf + 2, data, len);		//copy data[len] into buf[] starting from index 2
	
	nrfStopListening();
	nrfOpenWritingPipe(broadcast_pipe);
	nrfWrite(buf,len+2);
	
	//for (int i = 0; i < len+2; i++){
		//printf("%x ", buf[i]);
	//}
	//printf("\n");
	
	nrfStartListening();
	
}

/* check if nRF received something */
uint8_t nrf_rx_check(void){
	
	 uint8_t status = nrfWriteRegister(REG_STATUS, NRF_STATUS_RX_DR_bm);
	 
	 return status & NRF_STATUS_RX_DR_bm;
}

/* Read and procces an incomming packet */
void get_packet(void){
	
	uint8_t status = nrfGetStatus();
	uint8_t pipe   = (status & NRF_STATUS_RX_P_NO_gm) >> NRF_STATUS_RX_P_NO_gp;
	
	if (pipe == 0){
		read_broadcast();
	}
	else if (pipe == 1){
		read_personal();
	}
	else {
		printf("Unknown pipe\n");
	}
}

void read_broadcast(void){

	uint8_t buf[32];
	uint8_t broad_id;		// Source ID
	uint8_t broad_nodes;	// Amount of nodes
	uint8_t broad_data[30]; // Hops + Via ID's 
	
	uint8_t len = nrfGetDynamicPayloadSize();
	
	nrfRead(buf, len);						// Read the packet received and place in buf
	
	broad_id = buf[0];
	
	if ((buf[1] >> 7) == 0) {				// If MSB = 0 the message is a snapshot
		 broad_nodes = buf[1] & 0x7F;
		 memcpy(broad_data, buf+2, len-2);	// Add the remaining bytes into broad_data
		 
		 add_neighbor(broad_id, broad_nodes, broad_data);
		  
		 
	} else {
		printf("this is an update, we don't use those (yet)\n");
	}
}

void read_personal(void){
	
	uint8_t buf[32];
	uint8_t len = nrfGetDynamicPayloadSize();
	uint8_t via_node;
	
	nrfRead(buf, len); //Read the packet received and place in buf
	
	if (buf[0] != my_id){ // The message is not for this node :(
		
		via_node = calculated_routing(buf[0]);
		
		uint8_t send_pipe[5] = {BASE_ADDR_P,via_node};
			
		nrfStopListening();
		
		nrfOpenWritingPipe(send_pipe);	// Open a new writing pipe with the address calculated form the neighbor data
		nrfWrite(buf,len);				// Send the whole message that was received
		
		nrfStartListening();
	}
	else { // The message is for me! :)
		
		for (int i = 0; i < len; i++) {
			if (i == len-2) {
				printf("%d,", buf[i]); 
				} else {
				printf("%d ", buf[i]); 
			}
		}
		printf("\n");
	}
}

void button_press(void){
	
	uint8_t data[3];
	
	PORTB.PIN3CTRL = PORT_OPC_PULLUP_gc;
	
	if (! (PORTB.IN & PIN3_bm)){
		if (!buttonPressed) {
			buttonPressed = 1;
			if(buttonPressed){
				get_dummy_data(my_id, data);
				
				send_personal(data);
				
				
				//for (int i = 0; i < 2; i++){
					//printf("%d ", data[i]);
				//}
				//printf("\n");			
				}
		}
	}
	else {
		buttonPressed = 0;
	}
}

void send_personal(uint8_t *data){
	
	uint8_t buf[32];
	uint8_t via_node;
	uint8_t len = sizeof(data) / sizeof(data[0]);
	
	buf[0] = BASE_STATION;
	
	memcpy(buf+1, data, len);
	
	via_node = calculated_routing(BASE_STATION);
	
	if (via_node != 0){
		uint8_t send_pipe[5] = {BASE_ADDR_P,via_node};
	
		nrfStopListening();
	
		nrfOpenWritingPipe(send_pipe);	// Open a new writing pipe with the address calculated form the neighbor data
		nrfWrite(buf,len+1);			// Send the sensor data
	
		//for (int i = 0; i < len+1; i++){
			//printf("%d ", buf[i]);
		//}
		//printf("\n");
	
		nrfStartListening();
	}
	
}