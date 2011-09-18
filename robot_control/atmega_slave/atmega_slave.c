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
volatile uint16_t servo_pos_raw[4] = {SERVO_DEFAULT,SERVO_DEFAULT+10,SERVO_DEFAULT,SERVO_DEFAULT};	// + wartoœæ dla synchronizacji
volatile uint8_t servo_speed = 0;
volatile uint8_t current_servo = 0;

/*
 * Variables for stepping motors controls
 */
volatile uint8_t motor_current_step[2] = {1,1};		// 1-8
volatile uint16_t motor_pos[2] = {0,0};
volatile uint8_t motor_speed[2] = {100,100};

/*
 * Variables for SPI
 */
volatile uint8_t drive_state[6] = {0,0,0,0,0,0};
volatile uint8_t btn_state[6] = {BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF};

volatile uint8_t j = 0;
volatile uint8_t btn_get_flag = 0;
volatile uint8_t stepper_flag[2] = {0,0};

/*
 * Prototypes
 */
void toggle_servo_signal(uint8_t servo);
void servo_move(uint8_t servo, uint8_t dir);
void servo_to_deg(uint8_t servo, uint8_t deg);
void servo_to_raw(uint8_t servo, uint16_t raw);
void motor_make_step(uint8_t motor, uint8_t dir);
void motor_move(uint8_t motor, uint8_t dir);

/*
 * Initialize ports for servo signal outputs
 */
void servo_ports_init(void) {
	DDRD |= SERVOS;
	PORTD &= ~(SERVOS);
}

/*
 * Initialize ports for stepping motors outputs
 */
void motor_ports_init(void) {
	DDRD |= MOTOR_PD;
	PORTD &= ~(MOTOR_PD);
	DDRC |= MOTOR_PC;
	PORTD &= ~(MOTOR_PC);
	// set ports to default STEP1
	M0_STEP1;
	M1_STEP1;
}

/*
 * Analog-Digital-Converter interface initialization
 */
void ADC_init(void) {
	ADMUX |= (1<<REFS0)|(1<<REFS1);								// 2,56V
	ADMUX |= (1<<ADLAR);										// ADCH only
																// ADC0 by default
	ADCSRA |= (1<<ADIE);										// ADC interrupt flag
	ADCSRA |= (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);		// prescaler 128
	ADCSRA |= (1<<ADSC);										// initial measurement
}

/*
 * SPI transfer interface initialization
 */
void SPI_init(void) {
	DDRB |= MISO;
	SPCR |= (1<<SPIE)|(1<<SPE);
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
 * Initialize Timer0 for switching steps on stepping motors
 */
void Timer0_init(void) {
	TCCR0 |= (1<<CS00);							// no prescaler
	TIMSK |= (1<<TOIE0);						// overflow IRQ
}

/*
 * Start counting on Timer1
 */
void Timer1_start(void) {
	OCR1A = servo_pos_raw[current_servo];
	TCCR1B |= (1<<CS10);
}

/*
 * Stop counting on Timer1
 */
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
 * - switch servo signal on
 * - turn on Timer1
 */
ISR(TIMER2_COMP_vect) {
	toggle_servo_signal(current_servo);
	btn_get_flag++;
	Timer1_start();
}

/*
 * Timer1 interrupt handler
 * - switch servo signal off
 * - turn off Timer1
 */
ISR(TIMER1_COMPA_vect) {
	toggle_servo_signal(current_servo);
	Timer1_stop();
}

/*
 * Timer0 interrupt handler
 */
ISR(TIMER0_OVF_vect) {
	if (stepper_flag[0] > (255 - motor_speed[0])) {
		stepper_flag[0] = 0;
	} else {
		stepper_flag[0]++;
	}
	if (stepper_flag[1] > (255 - motor_speed[1])) {
		stepper_flag[1] = 0;
	} else {
		stepper_flag[1]++;
	}

	if (stepper_flag[0] == 0) {
		if (btn_state[0]==BTN_L) {
			motor_move(0,0);
		} else if (btn_state[0]==BTN_R) {
			motor_move(0,1);
		}
	}
	//TODO ograniczenie posuwu przez M1_POS_MIN i M1_POS_MAX
	if (stepper_flag[1] == 0) {
		if (btn_state[2]==BTN_L) {
			motor_move(1,0);
		} else if (btn_state[2]==BTN_R) {
			motor_move(1,1);
		}
	}
}

/*
 * ADC interrupt handler
 * - take value as raw 8bit reading
 * - update SPI data of speed
 */
ISR(ADC_vect) {
	// 0 - 255
	servo_speed = drive_state[5] = ADCH;
}

/************************************************* MAIN *************************************************/
int main(void)
{
	servo_ports_init();
	motor_ports_init();
	Timer2_init();
	Timer1_init();
	Timer0_init();
	SPI_init();
	ADC_init();
	sei();
	while(1)
	{
		if (btn_get_flag==BTN_GET_ON) {

			/* buttons for servos */
			if (btn_state[1]==BTN_L) {
				servo_move(0,0);
				servo_move(1,1);
			} else if (btn_state[1]==BTN_R) {
				servo_move(0,1);
				servo_move(1,0);
			}
			if (btn_state[3]==BTN_L) {
				servo_move(2,0);
			} else if (btn_state[3]==BTN_R) {
				servo_move(2,1);
			}

			// refresh current drives positions
			drive_state[0] = motor_pos[0] * M0_RATIO;
			drive_state[1] = (servo_pos_raw[1] - SERVO_MIN+50) / SERVO_STEPS_PER_DEG;		// +50 dla wyrównania 0 stopni
			drive_state[2] = motor_pos[1] / M1_RATIO;																			// TEMP: dzielenie zamiast mno¿enia!
			drive_state[3] = (servo_pos_raw[3] - SERVO_MIN+50) / SERVO_STEPS_PER_DEG;

			// measure value for speed
			ADCSRA |= (1<<ADSC);

			// set motors speed
			if (servo_speed > M0_SPD_H) {
				motor_speed[0] = M0_SPD_H;
			} else if (servo_speed < M0_SPD_L) {
				motor_speed[0] = M0_SPD_L;
			} else {
				motor_speed[0] = servo_speed;
			}
			if (servo_speed > M1_SPD_H) {
				motor_speed[1] = M1_SPD_H;
			} else if (servo_speed < M0_SPD_L) {
				motor_speed[1] = M1_SPD_L;
			} else {
				motor_speed[1] = servo_speed;
			}

			// clear button flag
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
	if (dir==1 && servo_pos_raw[servo]<SERVO_MAX) {
		servo_pos_raw[servo] += (servo_speed + SERVO_MIN_SPEED);
	} else if (dir==0 && servo_pos_raw[servo]>SERVO_MIN) {
		servo_pos_raw[servo] -= (servo_speed + SERVO_MIN_SPEED);
	}
}

/*
 * Changes software PWM overflow controlling servo position, by target deg
 */
void servo_to_deg(uint8_t servo, uint8_t deg) {
	if (deg>=SERVO_MIN_DEG && deg<=SERVO_MAX_DEG) {
		servo_pos_raw[servo] = SERVO_MIN + (SERVO_STEPS_PER_DEG*deg);
	}
}

/*
 * Changes software PWM overflow controlling servo position, by target raw
 */
void servo_to_raw(uint8_t servo, uint16_t raw) {
	servo_pos_raw[servo] = SERVO_MIN + raw;
}

/*
 *	Toggle ports states to switch coil set - make step
 */
void motor_make_step(uint8_t motor, uint8_t step) {
	if (step>8) {
		step = 1;
	} else if (step<1) {
		step = 8;
	}
	if (motor==0) {
		switch(step) {
			case 1:
				M0_STEP1;
				break;
			case 2:
				M0_STEP2;
				break;
			case 3:
				M0_STEP3;
				break;
			case 4:
				M0_STEP4;
				break;
			case 5:
				M0_STEP5;
				break;
			case 6:
				M0_STEP6;
				break;
			case 7:
				M0_STEP7;
				break;
			case 8:
				M0_STEP8;
				break;
		}
	} else if (motor==1) {
		switch(step) {
			case 1:
				M1_STEP1;
				break;
			case 2:
				M1_STEP2;
				break;
			case 3:
				M1_STEP3;
				break;
			case 4:
				M1_STEP4;
				break;
			case 5:
				M1_STEP5;
				break;
			case 6:
				M1_STEP6;
				break;
			case 7:
				M1_STEP7;
				break;
			case 8:
				M1_STEP8;
				break;
		}
	}
	motor_current_step[motor] = step;
}

/*
 * Move servo in particular directory by requesting step change
 */
void motor_move(uint8_t motor, uint8_t dir) {
	uint8_t next_step = (dir==1) ? motor_current_step[motor]+1 : motor_current_step[motor]-1;		// 0-8
	motor_make_step(motor, next_step);
	// count current position of infinite rotation motor
	if (motor==0) {
		if (dir==0 && motor_pos[0]==M0_POS_MIN)
			motor_pos[0] = M0_POS_MAX;
		} else if (dir==1 && motor_pos[0]==M0_POS_MAX) {
			motor_pos[0] = M0_POS_MIN;
		} else {
			motor_pos[0] = (dir==1) ? motor_pos[0]+1 : motor_pos[0]-1;
		}
	} else if (motor==1) { // count current position of finite transitional motor
		//TODO pozycja silnika krokowego posuwu
	}
}