/*
 * atmega_slave_conf.h
 *
 * Created on: 02.07.2011 02:11:02
 *     Author: cadavre
 */


#ifndef ATMEGA_SLAVE_CONF_H_
#define ATMEGA_SLAVE_CONF_H_

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
#define MISO		(1<<PB4)

#endif /* ATMEGA_SLAVE_CONF_H_ */