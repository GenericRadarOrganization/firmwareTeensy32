// 
// 
// 

#include "console.h"
#include "adc_proc.h"
#include "dac.h"

static void console_cmd(char** tokens, uint8_t n_tokens);

void console_init(void)
{

}

void console_loop(void)
{
	char inp[64];
	char* token;
	char* tokens[8];
	uint8_t i = 0;

	if (Serial.available())
	{
		Serial.readBytesUntil('\n', inp, 64);
		
		// Grab first token
		token = strtok(inp, " ");

		// And the rest!
		while (token != NULL && i < 8)
		{
			tokens[i] = token;
			i++;
			token = strtok(NULL, " ");
		}
		// Process command
		console_cmd(tokens, i);
	}
}

static void console_cmd(char** tokens, uint8_t n_tokens)
{
	uint32_t temp1;
	uint32_t temp2;

	if (strcmp(tokens[0], "dac") == 0)
	{
		if (n_tokens == 2 && strcmp(tokens[1], "start") == 0)
		{
			Serial.println("Starting DAC");
			dac_start();
			return;
		}
		else if (n_tokens == 2 && strcmp(tokens[1], "stop") == 0)
		{
			Serial.println("Stopping DAC");
			dac_stop();
			return;
		}
		else if (n_tokens == 4 && strcmp(tokens[1], "set") == 0)
		{
			//dac_stop();
			sscanf(tokens[2], "%lu", &temp1);
			sscanf(tokens[3], "%lu", &temp2);
			Serial.print("Setting DAC sweep from ");
			Serial.print(temp1);
			Serial.print(" to ");
			Serial.println(temp2);
			dac_write_sweep(temp1, temp2);
			//dac_start();
			return;
		}
	}
	if (n_tokens == 2 && strcmp(tokens[0], "adc") == 0)
	{
		if (strcmp(tokens[1], "start") == 0)
		{
			Serial.println("Starting ADC Sampling");
			adc_start();
			return;
		}
		else if (strcmp(tokens[1], "stop") == 0)
		{
			Serial.println("Stopping ADC Sampling");
			adc_stop();
			return;
		}
	}
}