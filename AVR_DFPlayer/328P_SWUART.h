/*
 * _328P_SWUART.h
 *
 * Created: 2018-04-02 오전 8:18:25
 *  Author: kiki
 * Original Source : https://community.atmel.com/projects/second-uart-software (written by danni)
 * Modified coding style
 * Adding comments for better understanding
 */ 


#ifndef __328P_SWUART_H_
#define __328P_SWUART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU	16000000L
#endif

#define SWU_DDR		DDRB
#define SWU_PORT	PORTB
#define SWU_PIN		PINB
#define SWU_RX		PB0
#define SWU_TX		PB1

#define BAUDRATE	9600
#define BIT_TIME	(uint16_t)(F_CPU / BAUDRATE + 0.5)		/* Clocks per one bit (rounded) */

extern volatile uint8_t sRxDone;

void SWU_Initialize(void);
void SWU_RxByte(uint8_t *data);
void SWU_TxByte(uint8_t data);
void SWU_RxBuffer(uint8_t *buffer, uint8_t length);
void SWU_TxBuffer(uint8_t *buffer, uint8_t length);

#endif /* __328P_SWUART_H_ */