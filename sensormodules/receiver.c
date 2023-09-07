#define  F_CPU   2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24spiXM2.h"
#include "nrf24L01.h"
#include "serialF0.h"
#include <avr/interrupt.h>
#include <stdbool.h>

uint8_t  pipe[5] = {0x48, 0x76, 0x41, 0x30, 0x31};    //!<  pipe address is "HvA01"
uint8_t packet[32];                                  //!<  buffer for storing received data

void init_pwm(void);
void init_nrf(void);

int main(void)
{
	init_stream(F_CPU);
	init_pwm();
	init_nrf();
	PORTF.DIRSET = PIN0_bm;
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();

	while (1) {}   // do nothing
}

void init_nrf(void)
{
	nrfspiInit();
	nrfBegin();

	nrfSetRetries(NRF_SETUP_ARD_1000US_gc, NRF_SETUP_ARC_8RETRANSMIT_gc);
	nrfSetPALevel(NRF_RF_SETUP_PWR_6DBM_gc);
	nrfSetDataRate(NRF_RF_SETUP_RF_DR_250K_gc);
	nrfSetCRCLength(NRF_CONFIG_CRC_16_gc);
	nrfSetChannel(54);
	nrfSetAutoAck(1);
	nrfEnableDynamicPayloads();

	nrfClearInterruptBits();
	nrfFlushRx();
	nrfFlushTx();

	// Interrupt
	PORTF.INT0MASK |= PIN6_bm;
	PORTF.PIN6CTRL  = PORT_ISC_FALLING_gc;
	PORTF.INTCTRL   = (PORTF.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_LO_gc;

	nrfOpenReadingPipe(0, pipe);
	nrfStartListening();
}

void init_pwm(void)
{
	PORTC.OUTCLR = PIN0_bm;
	PORTC.DIRSET = PIN0_bm;

	TCC0.CTRLB   = TC0_CCAEN_bm | TC_WGMODE_SINGLESLOPE_gc;
	TCC0.CTRLA   = TC_CLKSEL_DIV1_gc;
	TCC0.PER     = 9999;
	TCC0.CCA     = 0;
}

ISR(PORTF_INT0_vect)
{
	uint8_t  tx_ds, max_rt, rx_dr;

	nrfWhatHappened(&tx_ds, &max_rt, &rx_dr);

	if ( rx_dr ) {
		PORTF.OUTTGL = PIN0_bm;
		nrfRead(packet, 8);
		for (int i=0; i<8; i++)
		{
			printf("%c \n",(char)packet[i]);
			packet[i]=0;
		}
		TCC0.CCABUFL  =  packet[0];       // low byte was sent first
		TCC0.CCABUFH  =  packet[1];
	}
}