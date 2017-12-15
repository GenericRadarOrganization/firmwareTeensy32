// adc_proc.h

#ifndef _ADC_PROC_h
#define _ADC_PROC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

void adc_proc_init(void);
void adc_proc_loop(void);

void adc_start(void);

void adc_stop(void);
void adc_allow_run(void);
void adc_cal(void);
void adc_clear_cal(void);
uint16_t get_peak(void);
uint16_t* get_mags(void);

#endif

