#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24spiXM2.h"
#include "nrf24L01.h"
#include "serialF0.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Neighbor.h"

#define TEAM_NUMBER 0x1 << 5
#define MAX_COMMAND_LENGHT 50

//used functions
void init_nrf();
void send_id(uint8_t id);
void send_neighbor_id(uint8_t id);
void receive_id();
void dag_buurman ();
void init_timer();
void pingOfLife ();
void print_neighbors();
void count_down();

uint8_t pipe[5] = {0x32,0x73,0x65,0x78,0x81}; // "2sexy"
uint8_t IDpipe[5] = {0x73,0x65,0x78,0x79, 0XFF}; //"sexy + MY_ID"
uint8_t TXpipe[5] = {0x73,0x65,0x78,0x79,0x42};    // "sexy + 41"
uint8_t id;
volatile bool timerInterruptFlag = false;


int main(void)
{
	PORTF.DIRSET = PIN0_bm | PIN1_bm;

	// initializations
	init_stream(F_CPU);
	
	init_nrf();
	init_timer();
	
	PORTD.DIRCLR=PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm;
	
	id=TEAM_NUMBER|(PORTD.IN & PIN0_bm)|(PORTD.IN & PIN1_bm)|(PORTD.IN & PIN2_bm)|(PORTD.IN & PIN3_bm);

	sei();

	_delay_ms(20);
	PORTF.OUTTGL = PIN0_bm;
	
	while (1) {
		receive_id();
		
		print_neighbors();
		
		if (timerInterruptFlag) {
			count_down();
			timerInterruptFlag = false; // Reset the flag
		}
	}
}

void init_nrf(void)
{
	//PORTF.DIRSET = PIN0_bm;
	nrfspiInit();                              // Initialize SPI
	nrfBegin();                                // Initialize radio module
	nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);   // Power Control: -6 dBm
	nrfSetRetries(NRF_SETUP_ARD_1000US_gc, NRF_SETUP_ARC_NORETRANSMIT_gc);
	nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);// Data Rate: 250 Kbps
	nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);     // CRC Check
	nrfSetChannel(54);                         // Channel: 54
	nrfSetAutoAck(0);                          // Auto Acknowledge on
	nrfEnableDynamicPayloads();                // Enable Dynamic Payloads
	nrfClearInterruptBits();                   // Clear interrupt bits
	nrfFlushRx();                              // Flush fifo's
	nrfFlushTx();
	
	nrfOpenWritingPipe((uint8_t *) pipe);                  // Pipe for sending
	nrfOpenReadingPipe(0, (uint8_t *) pipe);               // Necessary for acknowledge
	nrfOpenReadingPipe(1, (uint8_t *) IDpipe);
	nrfStartListening();
}

void send_id(uint8_t id)
{
	//verzend id die wordt uitgelezen
	nrfStopListening();
	nrfWrite(&id, sizeof(id));
	nrfStartListening();
}


void receive_id()
{
	uint8_t packetBuffer[32] = {0};
	uint8_t tx_ds, max_rt, rx_dr;

	nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);

	if (rx_dr)
	{
		uint8_t length = nrfGetDynamicPayloadSize();
		nrfRead(packetBuffer, length);
		
		// Verwerk het ontvangen ID
		//printf("Ontvangen ID: %02X\n", packetBuffer[0]);
		received_packet(packetBuffer, length, *pipe); // hierbij moet de juiste pipe nog worden aangegeven
	}
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
	PORTF.OUTTGL = PIN1_bm;
	timerInterruptFlag = true; // activeer flag
}
void pingOfLife(void)
{
	send_id(id);
	//verstuur je buren ook mee 
}