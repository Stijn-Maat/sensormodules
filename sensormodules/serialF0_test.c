/*!
 *  \file    serialF0_test.c
 *  \author  Wim Dolman (<a href="mailto:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    16-04-2016
 *  \version 1.3
 *
 *  \brief   Example for serialF0 (serialF0.c and serialF0.h)
 *
 *  \details This example for HvA-Xmegaboard with streams
 *           using serialF0.c and serialF0.h.
 *
 *           This example uses scanf to receive a byte
 *           and to send this byte back in a formatted text string.
 *
 *           The baud rate is 115200.
 *
 *           This example is code 19.21 from 'De taal C en de Xmega' second edition,
 *           see <a href="http://www.dolman-wim.nl/xmega/book/index.php">Voorbeelden uit 'De taal C en de Xmega'</a>
 */
#define F_CPU     2000000UL              //!<  Clock frequency

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "serialF0.h"

/*! \brief main routine example for HvA-Xmegaboard
 *
 *  It initializes the standard streams for the serial interface(USART0 of port F) of 
 *  the HvA-xmegaboard.
 *
 *  It wait until it receives a character from the UART and sends the character 
 *  back in a formatted text string.
 *
 *  \return int
 */
int main(void)
{
  uint16_t c;

  init_stream(F_CPU);
  sei();

  while(1) {
    if ( (c = uartF0_getc()) == UART_NO_DATA ) {
      continue;
    }
    printf("Character: '%c' Hex: %#x\n", c, c);
  }
}
