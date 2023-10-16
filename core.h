/*
 * This file contains:
 *
 */

#ifndef CORE_H_
#define CORE_H_

#include <stdio.h>
#include <string.h>

#include "nrf24L01.h"
#include "nrf24spiXM2.h"
#include "serialF0.h"
#include "dummy_data.h"
#include "neighbor.h"

#define TEAM_NUMBER 1

volatile uint8_t my_id;

void init_nrf(void);
uint8_t get_my_id(void);
void init_timer(void);
void send_snap(void);
uint8_t nrf_rx_check(void);
void get_packet(void);
void read_broadcast(void);
void read_personal(void);
void button_press(void);
void send_personal(uint8_t *data);

#endif /* CORE_H_ */