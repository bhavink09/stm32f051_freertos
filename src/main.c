#include "stm32f0xx_conf.h"
#include "usart_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define TIMER_TICK_HZ 1000u

volatile unsigned long timer_tick = 0;

SemaphoreHandle_t mutex = NULL;

void check_key()
{
	unsigned long tmp = GPIOA->IDR;
	if(tmp & 0x01)
	{
		GPIOC->BSRR = (1 << 9);
	}
	else
	{
		GPIOC->BRR = (1 << 9);
	}
}

void blinky_task()
{
	while(1)
	{
		// Set PORTC.8
		GPIOC->BSRR = (1 << 8);
		vTaskDelay(200);
		// Reset PORTC.8
		GPIOC->BRR = (1 << 8);
		vTaskDelay(500);
	}
}

void usart_task1()
{
	while(1)
	{
		if(xSemaphoreTake( mutex, ( TickType_t ) 0 ) == pdTRUE )
		{
			usart1_puts("Hello world ");
			xSemaphoreGive(mutex);
			vTaskDelay(1000);
		}
	}
}

void usart_task2()
{
	while(1)
	{
		if(xSemaphoreTake( mutex, ( TickType_t ) 0 ) == pdTRUE )
		{
			usart1_puts(" dlrow olleH ");
			xSemaphoreGive(mutex);
			vTaskDelay(1000);
		}
	}
}

int main(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 	// enable the clock to GPIOC
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 	// enable the clock to GPIOA

	// Put PORTC.8 in output mode
	GPIOC->MODER |= (1 << 16);

	// Put PORTC.9 in output mode
	GPIOC->MODER |= (1 << 18);

	// Put PORTA.0 in input mode
	GPIOA->MODER &= ~(3 << 0);
	usart1_init();

	mutex = xSemaphoreCreateMutex();
    xTaskCreate(usart_task2,
                (const char *)"blinky_task",
                configMINIMAL_STACK_SIZE,
                NULL,                 /* pvParameters */
                tskIDLE_PRIORITY + 1, /* uxPriority */
                NULL                  /* pvCreatedTask */);

    xTaskCreate(usart_task1,
                (const char *)"usart_task1",
                configMINIMAL_STACK_SIZE,
                NULL,                 /* pvParameters */
                tskIDLE_PRIORITY + 1, /* uxPriority */
                NULL                  /* pvCreatedTask */);

//    xTaskCreate(usart_task2,
//                (const char *)"usart_task2",
//                configMINIMAL_STACK_SIZE,
//                NULL,                 /* pvParameters */
//                tskIDLE_PRIORITY + 1, /* uxPriority */
//                NULL                  /* pvCreatedTask */);

    vTaskStartScheduler();
    while(1);
}
