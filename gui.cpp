// 
// 
//
#include <SPI.h>
#include <stdint.h>
#include <gfxfont.h>
#include "Adafruit_ST7735.h"
#include <Adafruit_GFX.h>
#include "buttons.h"
#include "dac.h"
#include "adc_proc.h"
#include "config.h"
#include "pot.h"
#include "gui.h"

#define resetText() tft.setTextColor(ST7735_WHITE);tft.setTextSize(1);tft.setCursor(0,0)
#define TFT_CS 6
#define TFT_RST 5
#define TFT_DC 7
#define TFT_MOSI 11
#define TFT_SCLK 14

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

typedef enum {
	GUI_SPLASH,
	GUI_DOPPLER,
	GUI_FMCW
} gui_state_t;

static gui_state_t state;
static boolean gui_invalid;

typedef struct {
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
} line_t;

static line_t cat[] = {
	{ 0,0,63,31 },
	{ 0,31,63,0 },
	{ 0,0,0,63 },
	{ 63,0,63,63 },
	{ 0,63,31,79 },
	{ 63,63,31,79 },
	{ 0,31,31,79 },
	{ 63,31,31,79 }
};

static line_t whiskers[] = {
	{ 15,47 - 8,63 - 24,47 - 4 },
	{ 15,47,63 - 22,47 },
	{ 15,47 + 8,63 - 20,47 + 3 },
	{ 111,47 - 8,63 + 24,47 - 4 },
	{ 111,47,63 + 22,47 },
	{ 111,47 + 8,63 + 20,47 + 3 }
};

static void drawFFT(uint16_t x0, uint16_t y0, uint16_t color)
{
	uint16_t* mag_out = get_mags();
	uint16_t i;
	uint16_t barsize;
	uint16_t normalcolor = color;
	for (i = 0; i < 64; i++)
	{
		barsize = (mag_out[i] - 0x7FFF) / 512;
		if (barsize > 64)
		{
			color = ST7735_MAGENTA;
		}
		else
		{
			color = normalcolor;
		}
		tft.drawLine(x0 + i, y0, x0 + i, y0 - barsize, color);
		tft.drawLine(x0 + i, y0 - barsize, x0 + i, y0 - 64, ST7735_BLUE);
	}
}
static void drawvector(line_t* arr, uint16_t size, uint16_t xo, uint16_t yo, uint16_t color)
{
	int i;
	for (i = 0; i<size; i++) {
		tft.drawLine(arr[i].x1 + xo, arr[i].y1 + yo, arr[i].x2 + xo, arr[i].y2 + yo, color);
	}
}

void gui_init(void)
{
	resetText();
	tft.initR(INITR_BLACKTAB);
	tft.fillScreen(ST7735_RED);
	tft.setCursor(0, 0);
	drawvector(cat,8,32,32,ST7735_WHITE);
	drawvector(whiskers, 6, 0, 32, ST7735_WHITE);
	tft.setCursor(38, 120);
	tft.setTextSize(3);
	tft.setTextColor(ST7735_WHITE);
	tft.print("GRO");
	tft.setCursor(5, 150);
	tft.setTextSize(1);
	tft.print("We can't pick a name");
	state = GUI_SPLASH;
	gui_invalid = true;
	delay(100);
	pot_init();
}

static void go_to_doppler(void)
{
	resetText();
	tft.fillScreen(ST7735_BLACK);
	tft.setTextColor(ST7735_GREEN);
	tft.setCursor(0, 0);
	tft.print("Doppler"); // Transitioning to doppler!
	tft.setTextSize(2);
	tft.setTextColor(ST7735_YELLOW);
	tft.setCursor(100, 0);
	tft.print("Up");
	tft.setTextColor(ST7735_CYAN);
	tft.setCursor(100, 48);
	tft.print("Dn");
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(100, 48*2);
	tft.print("Cl");
	tft.setTextColor(ST7735_MAGENTA);
	tft.setCursor(100, 48*3);
	tft.print("Md");
	dac_write_sweep(CONFIG_DOPPLER_DAC, CONFIG_DOPPLER_DAC);
	state = GUI_DOPPLER;
	gui_invalidate();
	adc_clear_cal();
	dac_start();
}

static void go_to_fmcw(void)
{
	resetText();
	tft.fillScreen(ST7735_BLACK);
	tft.setTextColor(ST7735_RED);
	tft.setCursor(0, 0);
	tft.print("FMCW");
	tft.setTextSize(2);
	tft.setTextColor(ST7735_YELLOW);
	tft.setCursor(100, 0);
	tft.print("Up");
	tft.setTextColor(ST7735_CYAN);
	tft.setCursor(100, 48);
	tft.print("Dn");
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(100, 48 * 2);
	tft.print("Cl");
	tft.setTextColor(ST7735_MAGENTA);
	tft.setCursor(100, 48 * 3);
	tft.print("Md");
	// Configure DAC for FMCW sweep
	dac_write_sweep(CONFIG_FMCW_DAC_MIN, CONFIG_FMCW_DAC_MAX);
	state = GUI_FMCW;
	gui_invalidate();
	adc_clear_cal();
	dac_start();
}



void gui_loop(void)
{
	switch (state) {
		case GUI_SPLASH:
			// Wait for any button press
			if (buttons_get_mask() & 0x0F) {
				buttons_clear_mask(0x0F);
				go_to_doppler();
			}
			break;
		case GUI_DOPPLER:
			// Wait for Cal Button
			if (buttons_get_mask() & 0x04) {
				buttons_clear_mask(0x04);
				adc_cal();
			}
			// Wait for button 4 to be pressed
			if (buttons_get_mask() & 0x08) {
				buttons_clear_mask(0x08);
				go_to_fmcw();
			}
			if (buttons_get_mask() & 0x02) {
				buttons_clear_mask(0x2);
				pot_dec();
			}
			if (buttons_get_mask() & 0x01) {
				buttons_clear_mask(0x01);
				pot_inc();
			}
			if (gui_invalid) { // If something has changed and the GUI is invalid
				tft.setTextColor(ST7735_WHITE);
				tft.setTextSize(1);
				tft.setCursor(0, 20);
				tft.fillRect(0, 20, 110, 7, ST7735_BLACK);
				tft.print("Pot val: ");
				tft.print(pot_get());

				tft.fillRect(0, 60, 80, 14, ST7735_BLACK);
				tft.setCursor(0, 60);
				tft.setTextSize(2);
				tft.setTextColor(ST7735_CYAN);
				tft.print(get_peak()*0.41379310344f);
				gui_invalid = false;
			}
			drawFFT(0, 159, ST7735_YELLOW);
			break;
		case GUI_FMCW:
			// Wait for button 4 to be pressed
			if (buttons_get_mask() & 0x08) {
				buttons_clear_mask(0x08);
				go_to_doppler();
			}
			if (buttons_get_mask() & 0x04) {
				buttons_clear_mask(0x04);
				adc_cal();
			}
			if (buttons_get_mask() & 0x02) {
				buttons_clear_mask(0x2);
				pot_dec();
			}
			if (buttons_get_mask() & 0x01) {
				buttons_clear_mask(0x01);
				pot_inc();
			}
			if (gui_invalid) { // If something has changed and the GUI is invalid
				tft.setTextColor(ST7735_WHITE);
				tft.setTextSize(1);
				tft.setCursor(0, 20);
				tft.fillRect(0, 20, 110, 7, ST7735_BLACK);
				tft.print("Pot val: ");
				tft.print(pot_get());

				tft.fillRect(0, 60, 80, 14, ST7735_BLACK);
				tft.setCursor(0, 60);
				tft.setTextSize(2);
				tft.setTextColor(ST7735_CYAN);
				tft.print(get_peak()*0.41379310344f);
				gui_invalid = false;
			}
			drawFFT(0, 159, ST7735_YELLOW);
			break;
	}
}

// This gives us a mechanism to force the gui to redraw
// For example when changing a setting via console
void gui_invalidate(void)
{
	gui_invalid = true;
}