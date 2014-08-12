#ifndef MICRORL_FUNC_H_
#define MICRORL_FUNC_H_

#include "microrl.h"
#include "uart.h"

#define microrlNUM_OF_FUNC	10

#define microrl_getChar			UART_GetCharBlocking
#define microrl_sendString		UART_SendString

typedef struct {
	const char * name;
	int (*func)(int, const char* const*);
} exFunc_t;

void microrl_run(void *pvParameters);
void microrl_registerExecuteFunc(int (*func)(int, const char* const*), const char* name);

#endif
