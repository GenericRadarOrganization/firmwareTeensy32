#ifndef CONFIG_H
#define CONFIG_H

#define ADC_F_SAMPLE 4096
#define DAC_F_SAMPLE 2560 // To achieve 200ms sweep period

#define DAC_BUFF_SIZE 512
#define ADC_BUFFER_SIZE 512

#define CONFIG_FMCW_DAC_MIN (790-500)
#define CONFIG_FMCW_DAC_MAX (790+500) // For 200MHz*3 sweep
#define CONFIG_DOPPLER_DAC 790

#define PIT_PERIOD_DAC (F_CPU / DAC_F_SAMPLE)
#define PIT_PERIOD_ADC (F_CPU / ADC_F_SAMPLE)

#endif