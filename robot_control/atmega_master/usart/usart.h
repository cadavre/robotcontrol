/*
 * usart.h
 *
 * Created on: 07.07.2011 13:05:00
 *     Author: cadavre
 */

#ifndef USART_H_
#define USART_H_

#define UART_BAUD					9600
#define __UBRR						F_CPU/16/UART_BAUD-1

#define UART_RX_BUF_SIZE			32
#define UART_RX_BUF_MASK			(UART_RX_BUF_SIZE-1)

#define UART_TX_BUF_SIZE			16
#define UART_TX_BUF_MASK			(UART_TX_BUF_SIZE-1)

void USART_init( uint16_t baud );

char USART_get(void);
void USART_char( char data );
void USART_string(char *s);
void USART_int(int value);

#endif /* USART_H_ */
