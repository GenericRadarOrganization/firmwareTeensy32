// Thomas Gorham Fall 2017 - ECE 764
#include "Adafruit_ST7735.h"
#include "pot.h"
#include "buttons.h"
#include "gui.h"
#include "console.h"
#include "dac.h"
#include "adc_proc.h"
#include "adc.h"
#include "tsk.h"

void setup()
{
	Serial.begin(9600);
	//Serial.println("Test");

	adc_init();
	dac_init();
	tsk_init();
}

void loop()
{
	tsk_loop();
}
