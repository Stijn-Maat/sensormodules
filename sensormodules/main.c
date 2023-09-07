#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24spiXM2.h"
#include "nrf24L01.h"
#include "serialF0.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

uint8_t pipe[5] = {0x48, 0x76, 0x41, 0x30, 0x31}; // pipe address "HvA01"
void init_nrf(void);
void send(char *command);

char buffer[32];
uint16_t length;
uint16_t c;

int main(void)
{
	init_stream(F_CPU);
	init_nrf();

	printf("Enter Message: ");

	PORTF.DIRSET = PIN1_bm;
	PORTF.OUTSET = PIN1_bm;
	_delay_ms(20);
	PORTF.OUTTGL = PIN1_bm;
	sei();

	while (1)
	{
		c = uartF0_getc();
		if (c == '\r')
		{
			char command[] = "YourDataHere"; // Replace with the data you want to send
			send(command);
		}
	}
}

void init_nrf(void)
{
	nrfspiInit();                              // Initialize SPI
	nrfBegin();                                // Initialize radio module
	nrfSetRetries(NRF_SETUP_ARD_1000US_gc,     // Auto Retransmission Delay: 1000 us
	NRF_SETUP_ARC_8RETRANSMIT_gc); // Auto Retransmission Count: 8 retries
	nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);  // Power Control: -6 dBm
	nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc); // Data Rate: 250 Kbps
	nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);     // CRC Check
	nrfSetChannel(54);                         // Channel: 54
	nrfSetAutoAck(1);                          // Auto Acknowledge on
	nrfEnableDynamicPayloads();                // Enable Dynamic Payloads
	nrfClearInterruptBits();                   // Clear interrupt bits
	nrfFlushRx();                              // Flush fifo's
	nrfFlushTx();
	nrfOpenWritingPipe(pipe);                  // Pipe for sending
	nrfOpenReadingPipe(0, pipe);               // Necessary for acknowledge
}

void send(char *command)
{
	nrfStopListening();
	// Datasheet 130 us between
	_delay_us(130);
	uint8_t response = nrfWrite((uint8_t *)command, strlen(command));
	printf("\nVerzonden: %s\nAck ontvangen: %s\n", command, response > 0 ? "JA" : "NEE");
	nrfStartListening();
}
