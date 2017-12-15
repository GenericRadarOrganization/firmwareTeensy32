// gui.h

#ifndef _GUI_h
#define _GUI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

void gui_init(void);

void gui_loop(void);
void gui_invalidate(void);

#endif

