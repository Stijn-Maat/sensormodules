#include "dummy_data.h"

void init_adc()
{
	PORTA.DIRCLR     = PIN2_bm;                          // configure PA2 as input for ADCA
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc;            // PA2 to channel 0
	ADCA.CH0.CTRL    = ADC_CH_INPUTMODE_SINGLEENDED_gc;  // channel 0 single-ended
	ADCA.REFCTRL     = ADC_REFSEL_INTVCC_gc;             // internal VCC/1.6 reference
	ADCA.CTRLB       = ADC_RESOLUTION_12BIT_gc;          // 12 bits conversion, unsigned, no freerun
	ADCA.PRESCALER   = ADC_PRESCALER_DIV16_gc;           // 2MHz/16 = 125kHz
	ADCA.CTRLA       = ADC_ENABLE_bm;                    // enable adc
}

uint16_t read_adc()
{
	uint16_t adc_Value;
	
	ADCA.CH0.CTRL |= ADC_CH_START_bm;                    // start ADC conversion
	while ( !(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm) ) ;    // wait until it's ready
	adc_Value = ADCA.CH0.RES;
	ADCA.CH0.INTFLAGS |= ADC_CH_CHIF_bm;                 // reset interrupt flag

	return adc_Value;                                    // return measured value
}

float scale_ADC_Value(uint16_t adc_Value, uint16_t max_ADC_Value, float minRange, float maxRange) {
	
	// Calculate the scaled value using linear interpolation
	float scaledValue = minRange + ((maxRange - minRange) * adc_Value / max_ADC_Value);
	
	//printf("scaledValue: %.2f\n", scaledValue);

	return scaledValue;
}

//0x21 22 23 24
void get_dummy_data(uint8_t id, uint8_t *data) {

	uint16_t adc_Value;
	uint16_t max_ADC_Value;
		
	adc_Value = read_adc();
	//printf("Raw: %d\n", adc_Value);
	max_ADC_Value = 4095; // Maximum value for 12-bit ADC
		
	float scaledValue;
		
	switch(id){
		case 0x21:
		scaledValue = scale_ADC_Value(adc_Value, max_ADC_Value, 0.0, 50.0);
		//printf("Temperature: %.2f degrees Celsius\n", scaledValue);
		//send scaledValue + ID
		break;
		case 0x22:
		scaledValue = scale_ADC_Value(adc_Value, max_ADC_Value, 0.0, 5000.0);
		//printf("CO2: %.2f PPM\n", scaledValue);
		break;
		case 0x23:
		scaledValue = scale_ADC_Value(adc_Value, max_ADC_Value, 20.0, 90.0);
		//printf("Humidity: %.2f percent\n", scaledValue);
		break;
		case 0x24:
		scaledValue = scale_ADC_Value(adc_Value, max_ADC_Value, 0.0, 100.0);
		//printf("Nitrogen: %.2f PPM\n", scaledValue);
		break;
		default:
		//printf("Press button\n");
		break;
	}
	
		
	uint8_t wholePart = (uint8_t)scaledValue; // Extract whole numbers
	float fractionalPart = scaledValue - (float)wholePart; // Extract decimal part

	// Store the whole number part in the first byte
	data[0] = (uint8_t)wholePart;
	// Convert the decimal part to a byte 
	data[1] = (uint8_t)(fractionalPart * 100);

}



