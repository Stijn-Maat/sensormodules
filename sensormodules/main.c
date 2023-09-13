#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24/nrf24spiXM2.h"
#include "nrf24/nrf24L01.h"
#include "serialF0.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

#define TEAM_NUMMER 0x10

uint8_t pipe[5] = {0x48, 0x76, 0x41, 0x30, 0x31}; // pipe address "HvA01"
uint8_t IDpipe[5] = {0x32, 0x73, 0x65, 0x78, 0x79};


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
	id=TEAM_NUMMER|(PORTD.IN & PIN0_bm)|(PORTD.IN & PIN1_bm)|(PORTD.IN & PIN2_bm)|(PORTD.IN & PIN3_bm);

	sei();
	printf("%x\n",id);
	printf("Enter Message: ");

	char command[50];
	_delay_ms(20);
	PORTF.OUTTGL = PIN1_bm;
	
	while (1) {
		uint16_t userInput = uartF0_getc();
		if (userInput != UART_NO_DATA) {
			if (userInput == '\n' || userInput == '\r') {
				command[strlen(command)] = '\0';
				send(command);
				memset(command, 0, sizeof(command));
				} else {
				if (strlen(command) < sizeof(command) - 1) {
					command[strlen(command)] = (char)userInput;
				}
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
	nrfspiInit();                              // Initialize SPI
	nrfBegin();                                // Initialize radio module
	nrfSetRetries(NRF_SETUP_ARD_1000US_gc,     // Auto Retransmission Delay: 1000 us
	NRF_SETUP_ARC_8RETRANSMIT_gc);			   // Auto Retransmission Count: 8 retries
	nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);   // Power Control: -6 dBm
	nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);// Data Rate: 250 Kbps
	nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);     // CRC Check
	nrfSetChannel(54);                         // Channel: 54
	nrfSetAutoAck(1);                          // Auto Acknowledge on
	nrfEnableDynamicPayloads();                // Enable Dynamic Payloads
	nrfClearInterruptBits();                   // Clear interrupt bits
	nrfFlushRx();                              // Flush fifo's
	nrfFlushTx();
	
	nrfOpenWritingPipe(pipe);                  // Pipe for sending
	nrfOpenWritingPipe(IDpipe);
	nrfOpenReadingPipe(0, pipe);               // Necessary for acknowledge
	nrfOpenReadingPipe(1, IDpipe);
	nrfStartListening();
}

void send(char *command)
{
	nrfStopListening();
	uint8_t response = nrfWrite((uint8_t *)command, strlen(command));
	printf("\nVerzonden: %s\nAck ontvangen: %s\n", command, response > 0 ? "JA" : "NEE");
	nrfStartListening();
}

void receive(void)
{
	uint8_t packetBroad [32];
	uint8_t packetBroad_buffer [32];
	uint8_t packetPersonal [32];
	uint8_t packetPersonal_buffer [32];
	
	nrfOpenReadingPipe(0, pipe);

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
		
		PORTF.DIRSET = PIN0_bm;
		PORTF.OUTTGL = PIN0_bm;
	}
}