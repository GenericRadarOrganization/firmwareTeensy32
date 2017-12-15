// 
// 
// 

#include "buttons.h"

#define HISTORY_LEN 16
uint8_t history[HISTORY_LEN];
uint8_t button_flags;

void buttons_init(void)
{
	uint8_t i;
	for (i = 0; i < 4; i++)
	{
		pinMode(16 + i, INPUT_PULLUP);
	}
}

void buttons_loop(void)
{
	int i;
	uint8_t temp = 0x0F;
	// i = 9, 8, 7, 6, 5, 4, 3, 2, 1
	for (i = HISTORY_LEN - 1; i>0; i--) {
		history[i] = history[i - 1]; // Shift the array right 1
	}
	history[0] = (uint8_t)(~(GPIOB_PDIR & 0x0000000F));
	for (i = 0; i<HISTORY_LEN-1; i++) {
		temp &= history[i];
	}
	temp &= ~history[HISTORY_LEN - 1]; // Require that the first rxd bit is 0 (force a rising edge!)
	button_flags |= temp;
}

uint8_t buttons_get_mask(void)
{
	return button_flags;
}

uint8_t buttons_clear_mask(uint8_t mask)
{
	button_flags &= ~(mask);
}