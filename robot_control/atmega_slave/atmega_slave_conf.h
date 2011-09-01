/*
 * atmega_slave_conf.h
 *
 * Created on: 02.07.2011 02:11:02
 *     Author: cadavre
 */

#ifndef ATMEGA_SLAVE_CONF_H_
#define ATMEGA_SLAVE_CONF_H_

// porty SPI
#define MISO		(1<<PB4)

// porty serw
#define SERVO0_PIN				(1<<PD0)
#define SERVO1_PIN				(1<<PD1)
#define SERVO2_PIN				(1<<PD2)
#define SERVO3_PIN				(1<<PD3)
#define SERVOS					SERVO0_PIN|SERVO1_PIN|SERVO2_PIN|SERVO3_PIN

// serwa: iloœæ kroków na 1deg
#define SERVO_STEPS_PER_DEG		69
#define SERVO_MIN_SPEED			95

// serwa: wartoœci po³o¿eñ
#define SERVO_MIN				5200
#define SERVO_MAX				19000
#define SERVO_NULL				12100
#define SERVO_DEFAULT			SERVO_NULL
#define SERVO_MIN_DEG			0
#define SERVO_MAX_DEG			200

// krokowe: porty
#define MOTOR_PD				(1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7)
#define MOTOR_PC				(1<<PC2)|(1<<PC3)|(1<<PC4)|(1<<PC5)

// krokowe: cewki
#define M0_A1					(1<<PD4)
#define M0_B1					(1<<PD5)
#define M0_A2					(1<<PD6)
#define M0_B2					(1<<PD7)

#define M1_A1					(1<<PC2)
#define M1_B1					(1<<PC3)
#define M1_A2					(1<<PC4)
#define M1_B2					(1<<PC5)

// krokowe: kroki
#define M0_STEP1				PORTD |= M0_A1|M0_B1; PORTD &= ~(M0_A2|M0_B2)
#define M0_STEP2				PORTD |= M0_B1; PORTD &= ~(M0_A1|M0_A2|M0_B2)
#define M0_STEP3				PORTD |= M0_A2|M0_B1; PORTD &= ~(M0_A1|M0_B2)
#define M0_STEP4				PORTD |= M0_A2; PORTD &= ~(M0_A1|M0_B1|M0_B2)
#define M0_STEP5				PORTD |= M0_A2|M0_B2; PORTD &= ~(M0_A1|M0_B1)
#define M0_STEP6				PORTD |= M0_B2; PORTD &= ~(M0_A1|M0_A2|M0_B1)
#define M0_STEP7				PORTD |= M0_A1|M0_B2; PORTD &= ~(M0_A2|M0_B1)
#define M0_STEP8				PORTD |= M0_A1; PORTD &= ~(M0_A2|M0_B1|M0_B2)

#define M1_STEP1				PORTC |= M1_A1|M1_B1; PORTC &= ~(M1_A2|M1_B2)
#define M1_STEP2				PORTC |= M1_B1; PORTC &= ~(M1_A1|M1_A2|M1_B2)
#define M1_STEP3				PORTC |= M1_A2|M1_B1; PORTC &= ~(M1_A1|M1_B2)
#define M1_STEP4				PORTC |= M1_A2; PORTC &= ~(M1_A1|M1_B1|M1_B2)
#define M1_STEP5				PORTC |= M1_A2|M1_B2; PORTC &= ~(M1_A1|M1_B1)
#define M1_STEP6				PORTC |= M1_B2; PORTC &= ~(M1_A1|M1_A2|M1_B1)
#define M1_STEP7				PORTC |= M1_A1|M1_B2; PORTC &= ~(M1_A2|M1_B1)
#define M1_STEP8				PORTC |= M1_A1; PORTC &= ~(M1_A2|M1_B1|M1_B2)

// krokowe: stany prze³¹czeñ kroków
#define M0_SWITCH_FLAG			100
#define M1_SWITCH_FLAG			200

// oznaczenia przycisków
#define BTN_J1		0
#define BTN_J2		1
#define BTN_D3		2
#define BTN_J4		3
#define BTN_FUNCT	4

// stany przycisków
#define BTN_OFF		0x00		// nic nie wciœniête
#define BTN_ON		0x01		// funkcyjny wciœniêty
#define BTN_L		0x01		// wciœniêty Lewy
#define BTN_R		0x02		// wciœniêty Prawy

// flaga reakcji na wciœniêcie
#define BTN_GET_ON	4

#endif /* ATMEGA_SLAVE_CONF_H_ */