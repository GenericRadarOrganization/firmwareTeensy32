// dac.h

#ifndef _DAC_h
#define _DAC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

void dac_init(void);

void dac_write_sweep(uint16_t min, uint16_t max);

void dac_start(void);

void dac_stop(void);

uint8_t dac_is_enabled(void);

#endif

