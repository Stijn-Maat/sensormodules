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

#define TEAM_NUMBER 0x10
#define MAX_COMMAND_LENGHT 50

#define MY_ID 0x41 //A = 0x41, B = 0x42, C = 0x43 

char pipe[5] = {'2','s','e','x','y'};
char IDpipe[5] = {'s','e','x','y',MY_ID};


//used functions
void init_nrf(void);
void send(char *command);
void receive(void);

int main(void)
{
	PORTF.DIRSET = PIN1_bm;
	PORTF.OUTSET = PIN1_bm;

	// initializations
	init_stream(F_CPU);
	init_nrf();

	PORTD.DIRCLR=PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm;
	uint8_t id;
	id=TEAM_NUMBER|(PORTD.IN & PIN0_bm)|(PORTD.IN & PIN1_bm)|(PORTD.IN & PIN2_bm)|(PORTD.IN & PIN3_bm);

	sei();
	printf("%x\n",id);
	printf("Enter Message: ");

	char command[MAX_COMMAND_LENGHT];
	_delay_ms(20);
	PORTF.OUTTGL = PIN1_bm;
	
	while (1) {
		uint16_t userInput = uartF0_getc();
		if (userInput != UART_NO_DATA) {
			if (userInput == '\n' || userInput == '\r') {
				send(command);
				memset(command, 0, MAX_COMMAND_LENGHT);
			}
			else if (strlen(command) < MAX_COMMAND_LENGHT - 1) {
				command[strlen(command)] = (char)userInput;
			}
		}
		
		else if (userInput == UART_NO_DATA){
			receive();
		}
	}
	return 0;
}

void init_nrf(void)
{
	PORTF.DIRSET = PIN0_bm;
	nrfspiInit();                              // Initialize SPI
	nrfBegin();                                // Initialize radio module
	nrfSetRetries(NRF_SETUP_ARD_1000US_gc,     // Auto Retransmission Delay: 1000 us
	NRF_SETUP_ARC_8RETRANSMIT_gc);			   // Auto Retransmission Count: 8 retries
	nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);   // Power Control: -6 dBm
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

void send(char *command)
{
	char ID = MY_ID;
	
	size_t commandlength = strlen(command);
	
	char *data = (char *)malloc(commandlength + 2);
	
	data[0] = ID;
	
	strcpy(data + 1, command);
	
	nrfStopListening();  
	nrfWrite((uint8_t *)data, sizeof(data));
	
	printf("\nVerzonden:%s\n", data);
	nrfStartListening(); 
	
	free(data);
	
}

void receive(void)
{
	uint8_t packetBroad [32] = { 0 };
	uint8_t packetBroad_buffer [32] = { 0 };
	uint8_t packetPersonal [32] = { 0 };
	uint8_t packetPersonal_buffer [32] = { 0 };
	
	uint8_t tx_ds, max_rt, rx_dr;

	nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);
	
	if (rx_dr)
	{
		uint8_t length = nrfGetDynamicPayloadSize();
		
		nrfReadRegisterMulti(REG_RX_ADDR_P0, packetBroad, 32);
		nrfReadRegisterMulti(REG_RX_ADDR_P1, packetPersonal, 32);
		
		if (packetBroad != 0){
			nrfRead(packetBroad_buffer, 32);
			packetBroad_buffer[length] = '\0';
			
			printf("\nOntvangen bericht: %s\n", packetBroad_buffer);
			
		}
		else if (packetPersonal != 0){
			nrfRead(packetPersonal_buffer, 32);
			packetPersonal_buffer[length] = '\0';
			
			printf("\nOntvangen bericht: %s\n", packetPersonal_buffer);
			
		}
		PORTF.OUTSET = PIN0_bm;
	}
	PORTF.OUTCLR = PIN0_bm;
}
