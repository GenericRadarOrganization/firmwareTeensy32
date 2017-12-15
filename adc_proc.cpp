// 
// 
// 

#include "adc_proc.h"
#include "adc.h"
#include "config.h"
#include "gui.h"
#include "arm_math.h"

#define MAX_PEAKS 4

static arm_rfft_instance_q15 S;
static arm_cfft_radix4_instance_q15 S_CFFT;

static q15_t fft_out[ADC_BUFFER_SIZE*2]; // Two spots for real and complex
static uint16_t mag_out[ADC_BUFFER_SIZE]; // Combines all the samples back
static uint16_t mti_buf[ADC_BUFFER_SIZE];
static uint8_t adc_enable;
static q15_t window[ADC_BUFFER_SIZE];
static uint8_t do_cal;
volatile static uint8_t allow_run;
static uint8_t finished_proc;

typedef struct {
	uint16_t idx;
	uint16_t val;
} peak_t;
peak_t peaks[MAX_PEAKS];

// Builds a hamming window
static void build_window(q15_t* window, uint16_t window_len)
{
	uint16_t i;
	for (i = 0; i < window_len; i++)
	{
		mag_out[i] = 0x3AE1; // Temporarily use the mag_out buffer for a vector multiplication
		window[i] = arm_cos_q15(i*((1 << 15) / (window_len - 1)));
	}
	arm_mult_q15((q15_t*)mag_out, window, window, window_len);
	for (i = 0; i < window_len; i++)
	{
		window[i] = 0x451F - window[i] - 1; // Added the -1 because of sign issues
		//window[i] = (0x7FFF-arm_cos_q15(i*((1 << 15) / (window_len-1))))/2;
	}
}

static uint8_t find_peaks(uint16_t* mags, peak_t* peaks, uint8_t max_peaks, uint16_t avg, uint16_t threshold)
{
	uint16_t i, j;
	uint16_t maxval;
	uint16_t maxidx;

	for (i = 0; i < max_peaks; i++)
	{
		maxval = 0;
		maxidx = 0;
		for (j = ADC_BUFFER_SIZE / 2 + 1; j>0; j--)
		{
			if (mags[j] > maxval)
			{
				maxval = mags[j];
				maxidx = j;
			}
		}
		if ((maxval-0x7FFF) > threshold)
		{
			if (maxidx != peaks[i].idx) {
				gui_invalidate();
			}
			peaks[i].idx = maxidx;
			peaks[i].val = maxval;
			mags[maxidx] = 0; // Strike out found peak to avoid refinding it
		}
		else {
			break;
		}
	}
	return i;
}


// This function initializes state variables for the ADC processing
void adc_proc_init(void)
{
	uint8_t success = 0;
	arm_rfft_init_q15(&S, &S_CFFT, ADC_BUFFER_SIZE, 0, 1);
	build_window(window, ADC_BUFFER_SIZE);
	adc_enable = 1;
	allow_run = 0;
	finished_proc = 0;
	memset(mti_buf, 0, sizeof(mti_buf));
	do_cal = 0;
}

// This function handles ADC processing state, calculations, and population of results
void adc_proc_loop(void)
{
	uint32_t i;
	uint32_t micros_save;
	volatile uint16_t* adc_buff;
	q15_t temp_buf[ADC_BUFFER_SIZE/2];
	q15_t* fft_in;
	q15_t avg;

	if (adc_ready()&&allow_run)
	{
		micros_save = micros();
		adc_buff = adc_get_ptr();
		fft_in = (q15_t*)adc_buff;

		for (i = 0; i < ADC_BUFFER_SIZE; i++)
		{
			//fft_in[i] = arm_sin_q15(((i*0x7FFF*200/4000)%0x7FFF))/256;
			//fft_in[i] = arm_sin_q15((i * 0x7FFF / 10 + 100) % 0x7FFF)/2;
			adc_buff[i] -= 0x7FFF;
		}

		arm_mult_q15(window, fft_in, fft_in, ADC_BUFFER_SIZE);
		arm_rfft_q15(&S, fft_in, fft_out);

		arm_shift_q15(fft_out, 8, fft_out, ADC_BUFFER_SIZE*2); // In place

		// The output of this function is unsigned
		arm_cmplx_mag_q15(fft_out, (q15_t*)mag_out, ADC_BUFFER_SIZE);

		if (do_cal) {
			for (i = 0; i < ADC_BUFFER_SIZE; i++)
			{
				mti_buf[i] = mag_out[i];
			}
			do_cal = 0;
		}
		for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
			mag_out[i] -= mti_buf[i];
			mag_out[i] += 0x7FFF;
		}

		for (i = 0; i < (ADC_BUFFER_SIZE); i++)
		{
			Serial.print(mag_out[i]);
			//Serial.print(window[i]);
			Serial.print(" ");
		}
		Serial.println("\n");
		arm_mean_q15((q15_t*)mag_out, ADC_BUFFER_SIZE, &avg);
		//Serial.print("Average val: ");
		Serial.println((uint16_t)avg);

		find_peaks(mag_out, peaks, 1, avg, 500);
		//Serial.print("Main peak idx: ");
		//Serial.print(peaks[0].idx);
		//Serial.print(" val: ");
		//Serial.println(peaks[0].val);
		//Serial.print("Freq bin: ");
		//Serial.println(peaks[0].idx * ADC_F_SAMPLE / ADC_BUFFER_SIZE);
		//Serial.println(micros() - micros_save);
		allow_run = 0;
	}
}

void adc_start(void)
{
	adc_enable = 1;
}

void adc_stop(void)
{
	adc_enable = 0;
}

void adc_allow_run(void)
{
	allow_run = 1;
}

void adc_cal(void)
{
	Serial.println("Cal!");
	do_cal = 1;
}

void adc_clear_cal(void)
{
	memset(mti_buf, 0, sizeof(mti_buf));
}

uint16_t get_peak(void)
{
	return peaks[0].idx;
}

uint16_t* get_mags(void)
{
	return &mag_out[0];
}