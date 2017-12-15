#include "tsk.h"
#include "adc_proc.h"
#include "buttons.h"
#include "console.h"
#include "gui.h"
#include "util.h"
#include <stdint.h>

typedef struct {
	void(*init)(void);
	void(*loop)(void);
	uint32_t next_run;
	uint32_t period;
} task_t;

static void idle_init(void)
{
	// Yup
	pinMode(13, OUTPUT);
}

static void idle_loop(void)
{
}

task_t task_list[] =
{
	{ buttons_init,     buttons_loop,   0, 0    },
	{ adc_proc_init,	adc_proc_loop,	0, 0	},
	{ gui_init, 		gui_loop,		0, 100  },
	{ console_init,		console_loop,	0, 100	},
	{ idle_init,		idle_loop,		0, 1000 }
};

void tsk_init(void)
{
	uint8_t i;
	for (i = 0; i < cnt_of_array(task_list); i++)
	{
		//Serial.print("Initing task ");
		//Serial.println(1);
		task_list[i].init();
	}
}

void tsk_loop(void)
{
	uint8_t i;
	for (i = 0; i < cnt_of_array(task_list); i++)
	{
		if (task_list[i].next_run <= millis())
		{
			//Serial.print("Running task ");
			//Serial.println(i);
			task_list[i].next_run = millis() + task_list[i].period;
			task_list[i].loop();
		}
	}
}