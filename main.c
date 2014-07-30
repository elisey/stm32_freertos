#include "stm32f10x.h"
#include "uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void vBlinker(void *pvParameters) {
	while (1) {
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
		vTaskDelay(480);
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
		vTaskDelay(20);
	}
}

void vBlinker2(void *pvParameters) {
	while (1) {
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
		vTaskDelay(460);
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
		vTaskDelay(20);
	}
}

void vUartSender(void* pvParametrs)
{
	UART_Init();

	while(1)
	{
		UART_SendString("This is vUartSender task\n");
		vTaskDelay(1000);
	}
}

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gpio;

	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio);

	xTaskCreate(	vBlinker,"Blinker",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY + 1,
					NULL);

	xTaskCreate(	vBlinker2,"Blinker2",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY + 1,
					NULL);

	xTaskCreate(	vUartSender,"UartSender",
					configMINIMAL_STACK_SIZE,
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
