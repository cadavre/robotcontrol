/*
 * atmega_slave.c
 *
 * Created on: 26.06.2011
 *     Author: Seweryn 'cadavre' Zeman
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include "atmega_slave_conf.h"

/*
 * Variables for servo controls
 */
volatile uint16_t servo_pos_raw[4] = {SERVO_DEFAULT,SERVO_DEFAULT,SERVO_DEFAULT,SERVO_DEFAULT};
volatile uint8_t servo_speed = SERVO_STEPS_PER_DEG;
volatile uint8_t current_servo = 0;

/*
 * Variables for SPI
 */
volatile uint8_t drive_state[6] = {90,90,90,90,0,SERVO_STEPS_PER_DEG};
volatile uint8_t btn_state[6] = {BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF};

volatile uint8_t j = 0;
volatile uint8_t btn_get_flag = 0;

/*
 * Prototypes
 */
void btn_stepper_handle(uint8_t btn);
void btn_servo_handle(uint8_t btn);
void btn_funct_handle(uint8_t btn);
void toggle_servo_signal(uint8_t servo);
void servo_move(uint8_t servo, uint8_t dir);
void servo_to_deg(uint8_t servo, uint8_t deg);
void servo_to_raw(uint8_t servo, uint16_t raw);

/*
 * Initialize ports for servo signal outputs
 */
void servo_ports_init(void) {
	DDRD |= SERVOS;
	PORTD &= ~(SERVOS);
}

/*
 * Initialize Timer2 for 5ms intervals
 * - 5ms x 4 servos = 20ms = control signal interval
 */
void Timer2_init(void) {
	TCCR2 |= (1<<WGM21);			// CTC
	TCCR2 |= (1<<CS21)|(1<<CS22);	// prescaler 256
	OCR2 = 0x93;					// ~4.7ms
	TIMSK |= (1<<OCIE2);			// IRQ
}

/*
 * Initialize Timer1 for software PWM for servos
 * - 2,5ms with 20000 ticks
 */
void Timer1_init(void) {
	TCCR1B |= (1<<WGM12);			// CTC
	TIMSK |= (1<<OCIE1A);			// IRQ
}

/*
 * SPI transfer interface initialization
 */
void SPI_init(void) {
	DDRB |= MISO;
	SPCR |= (1<<SPIE)|(1<<SPE);
}

void Timer1_start(void) {
	OCR1A = servo_pos_raw[current_servo];
	TCCR1B |= (1<<CS10);
}

void Timer1_stop(void) {
	TCCR1B &= ~(1<<CS10);
	TCNT1 = 0;
	TIFR &= ~(1<<OCF1A);
	if(current_servo<4) {
		current_servo++;
	} else {
		current_servo = 0;
	}
}

/*
 * SPI interrupt handle
 * - r/w
 */
ISR(SPI_STC_vect) {
	btn_state[j] = SPDR;
	if(j<5) {
		j++;
	} else {
		j = 0;
	}
	SPDR = drive_state[j];
}

/*
 * Timer2 interrupt handler
 * - switches servo signal on
 * - turns on Timer1
 */
ISR(TIMER2_COMP_vect) {
	toggle_servo_signal(current_servo);
	btn_get_flag++;
	Timer1_start();
}

/*
 * Timer1 interrupt handler
 * - switches servo signal off
 * - turns off Timer1
 */
ISR(TIMER1_COMPA_vect) {
	toggle_servo_signal(current_servo);
	Timer1_stop();
}

/************************************************* MAIN *************************************************/
int main(void)
{
	servo_ports_init();
	Timer2_init();
	Timer1_init();
	SPI_init();
	sei();
	while(1)
	{
		if (btn_get_flag==16) {
			if (btn_state[0]==BTN_L) {
				drive_state[0]--;
			} else if (btn_state[0]==BTN_R) {
				drive_state[0]++;
			}
			if (btn_state[1]==BTN_L) {
				drive_state[1]--;
			} else if (btn_state[1]==BTN_R) {
				drive_state[1]++;
			}
			if (btn_state[2]==BTN_L) {
				drive_state[2]--;
			} else if (btn_state[2]==BTN_R) {
				drive_state[2]++;
			}
			if (btn_state[3]==BTN_L) {
				drive_state[3]--;
			} else if (btn_state[3]==BTN_R) {
				drive_state[3]++;
			}
			btn_get_flag = 0;
		}
	}
}
/********************************************** END OF MAIN **********************************************/

/*
 * Servo signal toggler for software PWM
 */
void toggle_servo_signal(uint8_t servo) {
	switch(servo) {
		case 0:
			PORTD ^= SERVO0_PIN;
			break;
		case 1:
			PORTD ^= SERVO1_PIN;
			break;
		case 2:
			PORTD ^= SERVO2_PIN;
			break;
		case 3:
			PORTD ^= SERVO3_PIN;
			break;
	}
}

/*
 * Changes software PWM overflow controlling servo position, by move
 */
void servo_move(uint8_t servo, uint8_t dir) {
	if(dir==1 && servo_pos_raw[servo]<SERVO_MAX) {
		servo_pos_raw[servo] += servo_speed;
	} else if(dir==0 && servo_pos_raw[servo]>SERVO_MIN) {
		servo_pos_raw[servo] -= servo_speed;
	}
}

/*
 * Changes software PWM overflow controlling servo position, by target deg
 */
void servo_to_deg(uint8_t servo, uint8_t deg) {
	if(deg>=SERVO_MIN_DEG && deg<=SERVO_MAX_DEG) {
		servo_pos_raw[servo] = SERVO_MIN + (SERVO_STEPS_PER_DEG*deg);
	}
}

/*
 * Changes software PWM overflow controlling servo position, by target raw
 */
void servo_to_raw(uint8_t servo, uint16_t raw) {
	servo_pos_raw[servo] = SERVO_MIN + raw;
}