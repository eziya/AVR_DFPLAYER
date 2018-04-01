/*
 * _328P_USART.c
 *
 * Created: 2018-02-06 오후 10:27:09
 *  Author: kiki
 */ 

#ifndef F_CPU
#define F_CPU	16000000
#endif

#include "328P_USART.h"
#include <util/delay.h>

volatile usart * const usart0 = (void*)USART0_REG;
volatile uint8_t usart0_rx_flag = 0;

ISR(USART_RX_vect)
{
	/* Disable Rx Interrupt */
	usart0->ucsr_b &= ~_BV(RXCIE0);
	usart0_rx_flag = 1;
}

void USART_Init()
{
	/* Enable U2X0 */
	usart0->ucsr_a |= _BV(U2X0);
	
	/* 115200 Baud rate, UBRR = (fosc / (8 * BAUD)) - 1 */
	/* 16MHz / (8 * 9600) - 1 = 16 */
	uint16_t baudrate = F_CPU / 8 / 9600 - 1;
	usart0->ubrr_h = (baudrate >> 8) & 0xFF;
	usart0->ubrr_l = baudrate & 0xFF;
	
	/* 8-N-1 */		
	usart0->ucsr_c |= _BV(UCSZ01) | _BV(UCSZ00);
	
	/* Enable Transmit + Receive */
	usart0->ucsr_b |= _BV(RXEN0) | _BV(TXEN0);	
}

bool USART_RxByte(uint8_t *data)
{	
	uint16_t loop = USART_TIMEOUT;
	do 
	{
		if(usart0->ucsr_a & _BV(RXC0))
		{
			*data = usart0->udr;			
			return true;
		}
		_delay_ms(1);		
	} while (--loop);
	
	return false;
}

void USART_RxByte_IT()
{
	/* Enable Rx Interrupt */	
	usart0->ucsr_b |= _BV(RXCIE0);
}

void USART_TxByte(uint8_t data)
{
	while((usart0->ucsr_a & _BV(UDRE0)) == 0);			
	usart0->udr = data;	
}

bool USART_RxBuffer(uint8_t *buffer, uint16_t len)
{
	for(int i=0; i < len; i++)
	{
		if(!USART_RxByte(&buffer[i]))
		{
			return false;
		}
		
		/* DFPlayer dedicated code */		
		if(i == 2 && buffer[2] != 0x06) return false;
	}	
	
	return true;
}

void USART_TxBuffer(uint8_t *buffer, uint16_t len)
{
	for(int i=0; i < len; i++)
	{
		USART_TxByte(buffer[i]);
	}
}

bool USART_Available()
{
	if(usart0->ucsr_a & _BV(RXC0))
	{
		return true;
	}
	
	return false;
}