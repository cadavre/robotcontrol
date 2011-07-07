/*
 * usart.c
 *
 * Created on: 07.07.2011 13:05:00
 *     Author: cadavre
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "usart.h"

volatile char UART_RxBuf[UART_RX_BUF_SIZE];
volatile uint8_t UART_RxHead;
volatile uint8_t UART_RxTail;

volatile char UART_TxBuf[UART_TX_BUF_SIZE];
volatile uint8_t UART_TxHead;
volatile uint8_t UART_TxTail;

void USART_init(uint16_t baud) {
	UBRRH = (uint8_t)(baud>>8);
	UBRRL = (uint8_t)baud;
	UCSRB |= (1<<RXEN)|(1<<TXEN);
	UCSRC |= (1<<URSEL)|(3<<UCSZ0);			// 8bit, 1bit stop
	UCSRB |= (1<<RXCIE);					// IRQ
}

void USART_char(char data) {
	uint8_t tmp_head;
    tmp_head  = (UART_TxHead + 1) & UART_TX_BUF_MASK;
    while ( tmp_head == UART_TxTail ){}
    UART_TxBuf[tmp_head] = data;
    UART_TxHead = tmp_head;
    UCSRB |= (1<<UDRIE);
}

void USART_string(char *s) {
	register char c;
	while ((c = *s++)) USART_char(c);
}

void USART_int(int value) {
	char string[17];
	itoa(value, string, 10);
	USART_string(string);
}

char USART_get(void) {
    if ( UART_RxHead == UART_RxTail ) return 0;
    UART_RxTail = (UART_RxTail + 1) & UART_RX_BUF_MASK;
    return UART_RxBuf[UART_RxTail];
}

ISR(USART_RXC_vect) {
    uint8_t tmp_head;
    char data;
    data = UDR;
    tmp_head = ( UART_RxHead + 1) & UART_RX_BUF_MASK;
    if ( tmp_head == UART_RxTail ) {

    } else {
		UART_RxHead = tmp_head;
		UART_RxBuf[tmp_head] = data;
    }
}

ISR(USART_UDRE_vect)  {
    if ( UART_TxHead != UART_TxTail ) {
    	UART_TxTail = (UART_TxTail + 1) & UART_TX_BUF_MASK;
    	UDR = UART_TxBuf[UART_TxTail];
    } else {
		UCSRB &= ~(1<<UDRIE);
    }
}