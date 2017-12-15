// pot.h

#ifndef _POT_h
#define _POT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

void pot_init(void);
void pot_set(uint8_t val);
uint8_t pot_get(void);
void pot_inc(void);
void pot_dec(void);

#endif

