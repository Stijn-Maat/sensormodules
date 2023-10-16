/*
 * This file contains:
 *
 * The main program for the WSN
 *
 */
#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "dummy_data.h"

int main(void)
{
    init_stream(F_CPU);
	sei();
	
	init_adc();
	init_nrf();
	init_timer();	
	
	printf("WSN team 1 \n");
		
	my_id = get_my_id();
	printf("MY ID =0x%x\n",my_id);
	
	uint8_t rx_dr;
	
    while (1){
		
		button_press();
		
		rx_dr = nrf_rx_check();		// Check if nRF received something
		
		if (rx_dr){
			printf("packet received");
			get_packet();			
		}
			
		if ( TCE0.CNT >= 31250 ) {	// 64*31250/2000000 = 1 s
			send_snap();
			printf("Snapshot send\n");	
			TCE0.CNT = 0;
		}
    }
}

