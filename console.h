// console.h

#ifndef _CONSOLE_h
#define _CONSOLE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

void console_init(void);
void console_loop(void);

#endif

