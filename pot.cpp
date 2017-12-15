// 
// 
// 

#include "pot.h"
#include "gui.h"
#include <SPI.h>
#define SSPIN 8

static SPISettings mySPISettings;
static uint8_t potval;

static void digitalPotWrite(uint8_t value) {
	SPI.beginTransaction(mySPISettings);
	// take the SS pin low to select the chip:
	digitalWrite(SSPIN, LOW);
	//  send in the address and value via SPI:
	//SPI.transfer(value);
	SPI.transfer(value);
	//KINETISK_SPI0.PUSHR = value | (0<<16) | SPI_PUSHR_CTAS(0);
	//while (((KINETISK_SPI0.SR) & (15 << 12)) > (3 << 12)); // wait if FIFO full
	// take the SS pin high to de-select the chip:
	digitalWrite(SSPIN, HIGH);
	SPI.endTransaction();
}

void pot_init(void)
{
	mySPISettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
	pinMode(SSPIN, OUTPUT);
	digitalWrite(SSPIN, HIGH);
	delay(100);
	pot_set(255);
}
void pot_set(uint8_t val)
{
	potval = val;
	digitalPotWrite(val);
}
void pot_inc(void)
{
	potval+=5;
	digitalPotWrite(potval);
	gui_invalidate();
}
void pot_dec(void)
{
	potval-=5;
	digitalPotWrite(potval);
	gui_invalidate();
}
uint8_t pot_get(void)
{
	return potval;
}
