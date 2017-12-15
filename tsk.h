// tsk.h

#ifndef _TSK_h
#define _TSK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

void tsk_init(void);

void tsk_loop(void);

#endif

