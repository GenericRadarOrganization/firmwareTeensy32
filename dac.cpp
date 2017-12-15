// 
// 
// 

#include "dac.h"
#include "adc_proc.h"
#include "adc.h"
#include "gui.h"
#include "config.h"

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_DMAEN \
	| PDB_SC_CONT | PDB_SC_PRESCALER(0) | PDB_SC_MULT(0))

#define VCO_EN 1

static uint16_t dac_buffer[DAC_BUFF_SIZE];

static uint8_t dac_enabled;

void dac_write_sweep(uint16_t min, uint16_t max)
{
	uint16_t i = 0;

	for (i = 0; i<DAC_BUFF_SIZE; i++)
	{
		dac_buffer[i] = min + i * (max - min) / DAC_BUFF_SIZE;
	}
}

void dac_init(void)
{
	dac_write_sweep(0, 0); // Constant value for doppler initially
	pinMode(VCO_EN, OUTPUT);
	digitalWrite(VCO_EN, 0);
	SIM_SCGC2 |= SIM_SCGC2_DAC0;

	DAC0_C0 = DAC_C0_DACEN | DAC_C0_DACTRGSEL | DAC_C0_DACRFS;
	DAC0_C1 = DAC_C1_DMAEN;

	SIM_SCGC7 |= SIM_SCGC7_DMA;
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
	SIM_SCGC6 |= SIM_SCGC6_PDB;

	// Modulus Register, 1/(48 MHz / 128 / 10) * 37500 = 1 s
	PDB0_MOD = PIT_PERIOD_DAC;
	// Interrupt delay
	PDB0_IDLY = 0;
	// PDB status and control
	PDB0_SC = PDB_CONFIG;
	// Software trigger (reset and restart counter)
	PDB0_SC |= PDB_SC_SWTRIG;
	// Load OK
	PDB0_SC |= PDB_SC_LDOK;

	// set up a DMA channel to store the ADC data
	SIM_SCGC7 |= SIM_SCGC7_DMA;
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
	DMA_CR = 0;
	DMA_TCD0_SADDR = dac_buffer;
	DMA_TCD0_SOFF = 2;
	DMA_TCD0_ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
	DMA_TCD0_NBYTES_MLNO = 2;
	DMA_TCD0_SLAST = -sizeof(dac_buffer);
	DMA_TCD0_DADDR = &DAC0_DAT0L;
	DMA_TCD0_DOFF = 0;
	DMA_TCD0_CITER_ELINKNO = sizeof(dac_buffer) / 2;
	DMA_TCD0_DLASTSGA = 0;
	DMA_TCD0_BITER_ELINKNO = sizeof(dac_buffer) / 2;
	DMA_TCD0_CSR = DMA_TCD_CSR_INTMAJOR;// DMA_TCD_CSR_INTMAJOR;
	DMAMUX0_CHCFG0 = DMAMUX_DISABLE;
	DMAMUX0_CHCFG0 = DMAMUX_SOURCE_PDB | DMAMUX_ENABLE;
	//update_responsibility = update_setup();

	NVIC_ENABLE_IRQ(IRQ_DMA_CH0);
	dac_enabled = 0;
	DMA_SERQ = 0;
}

void dac_start(void)
{
	digitalWrite(VCO_EN, 1);
	dac_enabled = 1;
	gui_invalidate();
}

void dac_stop(void)
{
	digitalWrite(VCO_EN, 0);
	dac_enabled = 0;
	gui_invalidate();
}

uint8_t dac_is_enabled(void)
{
	return dac_enabled;
}

void dma_ch0_isr()
{
	adc_trigger();
	adc_allow_run();
	DMA_CINT = 0;
}