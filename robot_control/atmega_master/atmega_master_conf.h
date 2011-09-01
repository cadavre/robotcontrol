/*
 * atmega_master_conf.h
 *
 * Created on: 02.07.2011 00:01:37
 *     Author: cadavre
 */

#ifndef ATMEGA_MASTER_CONF_H_
#define ATMEGA_MASTER_CONF_H_

// oznaczenia przycisk�w
#define BTN_J1		0
#define BTN_J2		1
#define BTN_D3		2
#define BTN_J4		3
#define BTN_FUNCT	4

// stany przycisk�w
#define BTN_OFF		0x00		// nic nie wci�ni�te
#define BTN_ON		0x01		// funkcyjny wci�ni�ty
#define BTN_L		0x01		// wci�ni�ty Lewy
#define BTN_R		0x02		// wci�ni�ty Prawy

// porty SPI
#define SS_CTRL		(1<<PB6)
#define SS			(1<<PB2)
#define MOSI		(1<<PB3)
#define MISO		(1<<PB4)
#define SCK			(1<<PB5)

// od�wie�anie
//TODO: mniejszy czas od�wie�ania
#define LCD_REFRESH_TICK		40
#define USART_REFRESH_TICK		252

// porty LCD skonfigurowane w lcd/lcd44780.h

#endif /* ATMEGA_MASTER_CONF_H_ */