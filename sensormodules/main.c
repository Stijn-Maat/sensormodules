#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "nrf24spiXM2.h"
#include "nrf24L01.h"
#include "serialF0.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEAM_NUMBER 0x10



//used functions
void init_nrf(void); // start the nRF 
void get_broadcast(uint8_t *id, uint8_t data[31]); //get message received on broadcast pipe and process
void get_personal(void); //get message received on personal pipe and process
uint8_t get_my_id(void); //reads io pins that form the node ID, opens pipe 1 with the node ID and returns the ID
void send(void); //sending message over variable pipe address	
uint8_t receive(void); //checks for something being received and returns the pipe that received the message


uint8_t broadcast_pipe[5] = {0x32,0x73,0x65,0x78,0x79}; // "2sexy"

//uint8_t send_pipe[5] = {0x73,0x65,0x78,0x79,send_id};    // "sexy + ?
	
int main(void){
	
	init_stream(F_CPU);
	sei();
	
	init_nrf();
	uint8_t my_id = get_my_id();

	while (1) {
		
		uint8_t pipe = receive();
		
		if (pipe == 0){ //broadcast
			
		}
		else { //personal
			
		} 
		
	
	}
}

void get_broadcast(uint8_t *id, uint8_t *data){
	//byte 0 = id, byte 1:31 = neighbor data
	
	uint8_t buf[32]; 
	uint8_t len = nrfGetDynamicPayloadSize();
	nrfRead(buf,len); //read the packet recieved and place in buf
	buf[len]='\0';
	
	*id = buf[0];
	
	memcpy(data, buf+1, len-1);
	
	
}

void get_personal(void){
	//byte 0 = adress, 
}

uint8_t get_my_id(void){
	
	//init address pins with a pulldown
	PORTD.DIRCLR=PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm;
	PORTD.PIN0CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN1CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN2CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN3CTRL = PORT_OPC_PULLDOWN_gc;
	
	uint8_t id;
	id=TEAM_NUMBER|(PORTD.IN & PIN0_bm)|(PORTD.IN & PIN1_bm)|(PORTD.IN & PIN2_bm)|(PORTD.IN & PIN3_bm);
	
	uint8_t recieve_pipe[5] = {0x73,0x65,0x78,0x79,id}; //"sexy + id"
	nrfOpenReadingPipe(1, recieve_pipe);
}

	

void init_nrf(void)
{
	PORTF.DIRSET = PIN0_bm;
	nrfspiInit();                              // Initialize SPI
	nrfBegin();                                // Initialize radio module
	nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);   // Power Control: -6 dBm
	nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);// Data Rate: 250 Kbps
	nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);     // CRC Check
	nrfSetChannel(54);                         // Channel: 54
	nrfSetAutoAck(0);                          // Auto Acknowledge on
	nrfEnableDynamicPayloads();                // Enable Dynamic Payloads
	nrfClearInterruptBits();                   // Clear interrupt bits
	nrfFlushRx();                              // Flush fifo's
	nrfFlushTx();
	
	nrfOpenReadingPipe(0, (uint8_t *) broadcast_pipe);               // Necessary for acknowledge
	
	nrfStartListening();
}



void send(void)
{
	
	//nrfStopListening(); 
	//nrfOpenWritingPipe();
	  //
	//nrfWrite((uint8_t *)data, strlen(data));
	//nrfStartListening();

}



uint8_t receive(void)
{
    
    uint8_t status;
    uint8_t pipe;
    uint8_t tx_ds, max_rt, rx_dr;

    nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);
    
    if (rx_dr)
    {
	    PORTF.OUTSET = PIN0_bm;
	    
	    status=nrfGetStatus(); 
	    //pipe = (status>>1)&0x7;
		pipe = (status & NRF_STATUS_RX_P_NO_gm) >> NRF_STATUS_RX_P_NO_gp;

    }
	
	return pipe;
	
    PORTF.OUTCLR = PIN0_bm;
}
