
#ifndef DUMMY_DATA_H_
#define DUMMY_DATA_H_

#include <stdio.h>
#include <avr/io.h>


//int buttonPressCount = 0;

void create_dummy_data();
float scale_ADC_Value(uint16_t adc_Value, uint16_t max_ADC_Value, float minRange, float maxRange);
void init_adc();
uint16_t read_adc();
void get_dummy_data(uint8_t id, uint8_t *data);



#endif /* DUMMY_DATA_H_ */