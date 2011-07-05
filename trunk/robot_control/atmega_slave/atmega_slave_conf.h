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

// flaga reakcji na wci�ni�cie
#define BTN_GET_ON	4

// porty SPI
#define MISO		(1<<PB4)

// porty serw
#define SERVO0_PIN				(1<<PD0)
#define SERVO1_PIN				(1<<PD1)
#define SERVO2_PIN				(1<<PD2)
#define SERVO3_PIN				(1<<PD3)
#define SERVOS					SERVO0_PIN|SERVO1_PIN|SERVO2_PIN|SERVO3_PIN

// serwa: ilo�� krok�w na 1deg
#define SERVO_STEPS_PER_DEG		69

// serwa: warto�ci po�o�e�
#define SERVO_MIN				5200
#define SERVO_MAX				13100
#define SERVO_NULL				9150
#define SERVO_DEFAULT			9150
#define SERVO_MIN_DEG			0
#define SERVO_MAX_DEG			115

#endif /* ATMEGA_SLAVE_CONF_H_ */