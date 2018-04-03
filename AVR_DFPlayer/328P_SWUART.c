/*
* _328P_SWUART.c
*
* Created: 2018-04-02 오전 8:18:37
*  Author: kiki
 * Original Source : https://community.atmel.com/projects/second-uart-software (written by danni)
 * Modified coding style
 * Adding comments for better understanding
*/

#include "328P_SWUART.h"

volatile uint8_t sRxDone;				/* Ready to read Rx byte */

static volatile uint8_t sTxCount;		
static volatile uint8_t sTxData;
static volatile uint8_t sRxData;
static volatile uint8_t sRxMask;
static volatile uint8_t sRxTemp;

/*****************************************************************************/
/* TIMER1 Input Capture Interrupt Handler									 */
/* When Rx pin detects start bit, interrupt handler is called				 */
/* When interrupt handler is called, TCNT1 value is written to ICR1 register */
/*****************************************************************************/
ISR(TIMER1_CAPT_vect)
{	
	/* Set OCR1B to read Rx */
	OCR1B = ICR1 + (uint16_t)(BIT_TIME * 1.5);		/* Output Compare B interrupt will occurs 1.5 BIT_TIME counter(time) later */
	sRxTemp = 0;									/* Clear temporary variable */
	sRxMask = 1;									/* Set bit mask to read first bit */	
	TIFR1 = _BV(OCF1B);								/* Clear Output Compare B interrupt flag */
			
	if(!(SWU_PIN & _BV(SWU_RX)))					/* If RX pin is low, It means start bit */
	{
		TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);			/* Disable input capture(Detecting start bit), Enable Output Compare A(for Tx) / B(for Rx) interrupt */
	}
}

/*****************************************************************************/
/* TIMER1 Output Compare B Interrupt Handler								 */
/* This handler is enabled to read data when system detects start bit		 */
/* sRxMask is true while system reads 8 bits from Rx pin					 */
/*****************************************************************************/
ISR(TIMER1_COMPB_vect)
{
	if(sRxMask)
	{		
		if(SWU_PIN & _BV(SWU_RX)) sRxTemp |= sRxMask;		/* if Rx input is high, write 1 to bit */				
		sRxMask <<= 1;										/* Shift 1 bit */
		OCR1B += BIT_TIME;									/* Set OCR1B to read next bit */
	}
	else
	{
		sRxDone = 1;										/* Reading 8 bits is done */
		sRxData = sRxTemp;									/* Store Rx data */
		TIFR1 = _BV(ICF1);									/* Clear input capture interrupt flag to read new byte */
		TIMSK1 = _BV(ICIE1) | _BV(OCIE1A);					/* Enable input capture + COMPA interrupt */
	}
}

/*****************************************************************************/
/* TIMER1 Output Compare A Interrupt Handler								 */
/* This handler is enabled to send data										 */
/* Be careful, sTxData is inverted data						 				 */
/*****************************************************************************/
ISR(TIMER1_COMPA_vect)
{
	uint8_t OutputMode;
	
	OCR1A += BIT_TIME;								/* Set OCR1A to send next bit */
	
	if(sTxCount)									/* If there are bits to send */
	{
		sTxCount--;									/* Decrease counter */
		OutputMode = _BV(COM1A1);					/* Set output as low */
		
		if(sTxCount != 9)							/* If it's data bit */
		{	
			if(!(sTxData & 0x01)) OutputMode = _BV(COM1A1) | _BV(COM1A0);		/* If bit is low, set output high (sTxData is inverted) */
			sTxData >>= 1;							
		}
		
		TCCR1A = OutputMode;
	}
}

/*****************************************************************************/
/* Software UART Initialization											 */
/* Enable Output Compare A interrupt to send data							 */
/* Enable Input Capture interrupt to receive data			 				 */
/*****************************************************************************/
void SWU_Initialize()
{
	OCR1A = TCNT1 + 1;						/* Output Compare A interrupt is called immediately after initialization */
	TCCR1A = _BV(COM1A1) | _BV(COM1A0);		/* Set output mode, set OC1A high(Idle), Timer1 mode 0 */
	TCCR1B = _BV(ICNC1) | _BV(CS10);		/* Input capture noise canceler, falling edge detection, no prescaling */
	TIFR1 = _BV(ICF1);						/* Clear capture flag */
	TIMSK1 = _BV(ICIE1) | _BV(OCIE1A);		/* Enable input capture interrupt(Detect start bit) + Output comapre A match interrupt(Tx) */

	sTxCount = 0;							/* No Tx Data */
	sRxDone = 0;							/* No Rx Data */
	SWU_DDR |= _BV(SWU_TX);					/* Set TX pin as output */
	
	sei();									/* Enable global interrupt */
}

/*****************************************************************************/
/* Read Byte																 */
/*****************************************************************************/
void SWU_RxByte(uint8_t * data)
{
	while( !sRxDone );
	
	sRxDone = 0;
	*data = sRxData;
}

/*****************************************************************************/
/* Write Byte																 */
/*****************************************************************************/
void SWU_TxByte(uint8_t data)
{
	while(sTxCount);
	sTxData = ~data;			/* !!! Write inverted data for stop bit generation */
	sTxCount = 10;				/* Start(1) + data(8) + Stop(1) */
}

/*****************************************************************************/
/* Read Bytes																 */
/*****************************************************************************/
void SWU_RxBuffer(uint8_t *buffer, uint8_t length)
{
	for(uint8_t i = 0; i < length; i++)
	{
		SWU_RxByte(buffer);
		buffer++;
	}
}

/*****************************************************************************/
/* Write Bytes																 */
/*****************************************************************************/
void SWU_TxBuffer(uint8_t *buffer, uint8_t length)
{
	for(uint8_t i = 0; i < length; i++)
	{
		SWU_TxByte(*buffer);
		buffer++;
	}
}