#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nrf24spiXM2.h"
#include "nrf24L01.h"
#include "serialF0.h"
#include "neighbor.h"

#define TEAM_NUMBER 1
#define BASE_PIPE

void init_nrf(void);															//start the nRF
void get_broadcast(uint8_t *data, uint8_t *len);					//get message received on broadcast pipe and process
void get_personal(uint8_t *address, uint8_t *data, uint8_t *len);//get message received on personal pipe and process
uint8_t get_my_id(void);														//reads io pins that form the node ID, opens pipe 1 with the node ID and returns the ID															//sending message over variable pipe address
uint8_t received(void);
void init_timer(void);
void pingOfLife(void);
//checks for something being received and returns the pipe that received the message
volatile uint8_t timerInterruptFlag = 0;

uint8_t broadcast_pipe[5] = {0x32,0x73,0x65,0x78,0x79}; // "2sexy"
uint8_t my_id;

int main(void){
	
	init_stream(F_CPU);
	sei();
	
	init_nrf();
	
	printf("nRF test\n");
	my_id = get_my_id();
	
	uint8_t pipe;
	
	//uint8_t broadcast_id;
	uint8_t broadcast_data[32];
	uint8_t broadcast_len;
	
	uint8_t received_addr;
	//uint8_t received_hops;
	uint8_t received_data[32];
	uint8_t received_len;
	
	

	while (1) {

		pipe = received();

		
		if (pipe == 0){
			get_broadcast(broadcast_data, &broadcast_len);
		}
		if (pipe == 1){
			get_personal(&received_addr, received_data, &received_len);
		}
		
		if (timerInterruptFlag) {
			count_down();
			timerInterruptFlag = 0; // Reset the flag
		}
		
	}
}

void get_broadcast(uint8_t *data, uint8_t *len){
	//byte 0 = id, byte 1:31 = neighbor data
	
	uint8_t buf[32];
	uint8_t lenght = nrfGetDynamicPayloadSize();
	nrfRead(buf,lenght); //read the packet received and place in buf
	
	uint8_t broad_id = buf[0];
	memcpy(data, buf+1, lenght-1);
	*len = lenght-1;
	
	add_neighbor(broad_id,buf);
	
	
}

void get_personal(uint8_t *address, uint8_t *data, uint8_t *len){
	//byte 0 = adress,
	
	uint8_t buf[32];
	uint8_t lenght = nrfGetDynamicPayloadSize();
	nrfRead(buf,lenght); //read the packet received and place in buf
	
	*address = buf[0];
	memcpy(data, buf + 1, lenght - 1);
	*len = lenght-1;
	
}

uint8_t get_my_id(void){
	
	//init address pins with a pulldown
	PORTD.DIRCLR   = PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm|PIN4_bm;
	PORTD.PIN0CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN1CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN2CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN3CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN4CTRL = PORT_OPC_PULLDOWN_gc;
	
	uint8_t id;
	id=(TEAM_NUMBER<<5)|(PORTD.IN & 0b11111);			//001-00000 is default address
	
	uint8_t recieve_pipe[5] = {0x73,0x65,0x78,0x79,id}; //"sexy + id"
	nrfOpenReadingPipe(1, recieve_pipe);
	
	return id;
}



void init_nrf(void){
	nrfspiInit();                              // Initialize SPI
	nrfBegin();                                // Initialize radio module
	nrfSetPALevel(NRF_RF_SETUP_PWR_18DBM_gc);  // Power Control
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






uint8_t received(void){
	
	uint8_t status;
	uint8_t pipe = 9;
	uint8_t tx_ds, max_rt, rx_dr;
	
	nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);
	
	if (rx_dr){
		//printf("something received!\n");
		status = nrfGetStatus();
		pipe = (status & NRF_STATUS_RX_P_NO_gm) >> NRF_STATUS_RX_P_NO_gp;
		
	}
	else {
		pipe = 9; // just to make sure it can't be a known pipe
		//printf("nothing received\n");
	}
	
	return pipe;
	
}

void init_timer(void)
{
	TCC0.CTRLB = TC0_CCAEN_bm | TC_WGMODE_NORMAL_gc;
	TCC0.CTRLA = TC_CLKSEL_DIV8_gc;
	TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;
	TCC0.PER = 62499;
}

ISR(TCC0_OVF_vect)
{
	pingOfLife();
	timerInterruptFlag = 1; // activeer flag
}

void pingOfLife(void)
{
	
	uint8_t buf[32];
	uint8_t len;
	buf[0] = my_id;
	
	nrfStopListening();
	
	nrfWrite(buf,len);
	
	nrfStartListening();
}
