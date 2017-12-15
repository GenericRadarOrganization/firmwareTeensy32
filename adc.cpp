#include "adc.h"
#include "util.h"
#include "config.h"
#include <stdint.h>

#define ADC_PIN 22
#define ADC_DELAY_SAMPLES 150
// we add 10 dummy samples to avoid running into the slew caused by the falling
// edge of the vco tune output
static volatile uint16_t adc_buffer[ADC_BUFFER_SIZE+ ADC_DELAY_SAMPLES];

void adc_init(void)
{

	// Configure the ADC and run at least one software-triggered
	// conversion.  This completes the self calibration stuff and
	// leaves the ADC in a state that's mostly ready to use
	analogReadRes(12); // Use 16bit res
	analogReference(DEFAULT); // 0 to 3.3V
	analogReadAveraging(2);
	analogRead(ADC_PIN);

	// set the programmable delay block to trigger the ADC at 6000Hz
	SIM_SCGC6 |= SIM_SCGC6_PIT;
	//PDB0_MOD = PDB_PERIOD;
	//PDB0_SC = PDB_CONFIG | PDB_SC_LDOK;
	//PDB0_SC = PDB_CONFIG | PDB_SC_SWTRIG;
	//PDB0_CH0C1 = 0x0101;

	// Due to errata involving DMA/PDB triggers getting latched high, we will use the PIT (Same channel as DAC)
	PIT_MCR = 0;
	PIT_LDVAL1 = PIT_PERIOD_ADC;
	PIT_TCTRL1 = PIT_TCTRL_TEN;

	// enable the ADC for hardware trigger (from PIT) and DMA
	SIM_SOPT7 = SIM_SOPT7_ADC0ALTTRGEN | SIM_SOPT7_ADC0TRGSEL(5);
	ADC0_SC2 |= ADC_SC2_ADTRG | ADC_SC2_DMAEN;

	// set up a DMA channel to store the ADC data
	SIM_SCGC7 |= SIM_SCGC7_DMA;
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
	DMA_CR = 0;
	DMA_TCD1_SADDR = &ADC0_RA;
	DMA_TCD1_SOFF = 0;
	DMA_TCD1_ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
	DMA_TCD1_NBYTES_MLNO = 2;
	DMA_TCD1_SLAST = 0;
	DMA_TCD1_DADDR = adc_buffer;
	DMA_TCD1_DOFF = 2;
	DMA_TCD1_CITER_ELINKNO = sizeof(adc_buffer) / 2;
	DMA_TCD1_DLASTSGA = -sizeof(adc_buffer);
	DMA_TCD1_BITER_ELINKNO = sizeof(adc_buffer) / 2;
	DMA_TCD1_CSR = DMA_TCD_CSR_INTMAJOR | DMA_TCD_CSR_DREQ;
	DMAMUX0_CHCFG1 = DMAMUX_DISABLE;
	DMAMUX0_CHCFG1 = DMAMUX_SOURCE_ADC0 | DMAMUX_ENABLE;
	//update_responsibility = update_setup();
	DMA_SERQ = 1;
	NVIC_ENABLE_IRQ(IRQ_DMA_CH1);
}

void dma_ch1_isr() {
	//Serial.println("ASD");
	DMA_CINT = 1;
}

uint32_t adc_ready(void)
{
	return DMA_TCD1_CSR & DMA_TCD_CSR_DONE;
}

void adc_trigger(void)
{
	DMA_SERQ = 1;
}

volatile uint16_t* adc_get_ptr(void)
{
	return &adc_buffer[ADC_DELAY_SAMPLES];
}