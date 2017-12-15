// adc.h

#ifndef _ADC_h
#define _ADC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

#include <stdint.h>

void adc_init(void);

uint32_t adc_ready(void);

void adc_trigger(void);

volatile uint16_t* adc_get_ptr(void);

#endif

