/*
 * usart_driver.c
 *
 *  Created on: Mar 22, 2015
 *      Author: Bhavin
 */
#include "stm32f0xx_conf.h"
#include "stm32f0xx_usart.h"

#include "usart_driver.h"

#include <stdint.h>
#include <stdio.h>

/* **************************************************************************
 * Private global variables. They are not expected to be accessed directly
 * outside the file. Use respective get and set methods to read/modify
 * them from outside.
 */

/****************************************************************************/

/* **************************************************************************
 * Private functions, not to be used from outside
 */

/**********************************************************
 * USART1 interrupt request handler: on reception of a
 * character 't', toggle LED and transmit a character 'T'
 *********************************************************/
void USART1_IRQHandler(void)
{
	char rcvd;
    /* RXNE handler */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* If received 't', toggle LED and transmit 'T' */
    	rcvd = (char)USART_ReceiveData(USART1);
        USART_SendData(USART1, rcvd);
        /* Wait until Tx data register is empty
        */
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        {
        }
    }
}
/****************************************************************************/

/* **************************************************************************
 * Public functions
 */
void usart1_init(void)
{
    /* USART configuration structure for USART1 */
    USART_InitTypeDef usart1_init_struct;
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef gpioa_init_struct;

    /* Enable clock for USART1, AFIO and GPIOA */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO |
    //                       RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

    /* GPIOA PIN9 alternative function Tx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF;
    gpioa_init_struct.GPIO_OType = GPIO_OType_PP;
    gpioa_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpioa_init_struct);
    /* GPIOA PIN9 alternative function Rx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF;
    gpioa_init_struct.GPIO_OType = GPIO_OType_OD;
    gpioa_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpioa_init_struct);

    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    usart1_init_struct.USART_BaudRate = 9600;
    usart1_init_struct.USART_WordLength = USART_WordLength_8b;
    usart1_init_struct.USART_StopBits = USART_StopBits_1;
    usart1_init_struct.USART_Parity = USART_Parity_No ;
    usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &usart1_init_struct);

    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
}

void usart1_rcv_interrupt_en()
{
	/* Enable RXNE interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART1_IRQn);
}

void usart1_putch(char data)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, data);
}

void usart1_puts(char* str)
{
	while(*str)
	{
		usart1_putch(*str);
		++str;
	}
}

void usart1_puti(int32_t data, uint8_t digits)
{
	char buffer[11];
	itoa(buffer, data, digits);
	usart1_puts(buffer);
}
/****************************************************************************/
