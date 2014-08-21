#include "stm32f10x.h"
#include "uart.h"
#include "microrl_func.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef struct {
	uint16_t timeOn;
	uint16_t timeOff;
	GPIO_TypeDef * gpio;
	uint16_t pin;
}ledTiming_t;

void vBlinker(void *pvParameters);
void microrl_run(void *pvParameters);

void vBlinker(void *pvParameters)
{
	GPIO_InitTypeDef gpio;

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = ((ledTiming_t*)pvParameters)->pin;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(((ledTiming_t*)pvParameters)->gpio, &gpio);

	while (1) {
		GPIO_WriteBit(((ledTiming_t*)pvParameters)->gpio, ((ledTiming_t*)pvParameters)->pin, Bit_RESET);
		vTaskDelay(((ledTiming_t*)pvParameters)->timeOff);
		GPIO_WriteBit(((ledTiming_t*)pvParameters)->gpio, ((ledTiming_t*)pvParameters)->pin, Bit_SET);
		vTaskDelay(((ledTiming_t*)pvParameters)->timeOn);
	}
}

void microrl_run(void *pvParameters)
{
	microrl_terminalInit();
	while(1)
	{
		microrl_terminalProcess();
	}
}

int main(void)
{
	UART_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	ledTiming_t* ptrLedTiming = pvPortMalloc(sizeof(ledTiming_t));
	ptrLedTiming->gpio = GPIOC;
	ptrLedTiming->pin = GPIO_Pin_8;
	ptrLedTiming->timeOff = 480;
	ptrLedTiming->timeOn = 20;

	xTaskCreate(	vBlinker,
					"Blinker",
					configMINIMAL_STACK_SIZE,
					(void*)ptrLedTiming,
					tskIDLE_PRIORITY + 1,
					NULL);

	ledTiming_t* ptrLedTiming2 = pvPortMalloc(sizeof(ledTiming_t));
	ptrLedTiming2->gpio = GPIOC;
	ptrLedTiming2->pin = GPIO_Pin_9;
	ptrLedTiming2->timeOff = 460;
	ptrLedTiming2->timeOn = 20;

	xTaskCreate(	vBlinker,"Blinker2",
					configMINIMAL_STACK_SIZE,
					(void*)ptrLedTiming2,
					tskIDLE_PRIORITY + 1,
					NULL);

	xTaskCreate(	microrl_run,"microrl",
					300,
					NULL,
					tskIDLE_PRIORITY + 1,
					NULL);

	vTaskStartScheduler();
	return 0;
}

void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}
