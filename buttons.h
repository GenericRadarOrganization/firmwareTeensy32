// buttons.h

#ifndef _BUTTONS_h
#define _BUTTONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

void buttons_init(void);
void buttons_loop(void);

uint8_t buttons_get_mask(void);
uint8_t buttons_clear_mask(uint8_t mask);

#endif

