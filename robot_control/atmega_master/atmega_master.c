/*
 * atmega_master.c
 *
 * Created on: 26.06.2011
 *     Author: Seweryn 'cadavre' Zeman
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include "atmega_master_conf.h"
#include "lcd/lcd44780.h"

uint8_t deg_sign[] = {6,9,9,6,0,0,0,0};

volatile uint8_t btn_state[6] = {BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF};
volatile uint8_t drive_state[6] = {0,0,0,0,0,0};

volatile char btn_sign;
volatile uint8_t lcd_refresh_flag = 0;
volatile uint8_t i = 0;
volatile uint8_t j = 0;

/*
 * SPI transfer interface initialization
 */
void SPI_init(void) {
	DDRB |= MOSI|SCK|SS|SS_CTRL;				// master out
	PORTB |= SS;								// SS=H
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1);		// prescaler 64
}

/*
 * Timer2 initialization
 */
void Timer2_init(void) {
	TCCR2 |= (1<<WGM21);						// CTC
	TCCR2 |= (1<<CS21)|(1<<CS22);				// prescaler 256
	OCR2 = 0x24;								// 36 = ~1.15ms
	TIMSK |= (1<<OCIE2);						// compare match IRQ
}

/*
 * Analog-Digital-Converter interface initialization
 */
void ADC_init(void) {
	ADMUX |= (1<<REFS0)|(1<<REFS1);				// 2,56V
	ADMUX |= (1<<ADLAR);						// ADCH only
	ADCSRA |= (1<<ADIE);						// ADC interrupt flag
	ADCSRA |= (1<<ADEN)|(1<<ADPS0)|(1<<ADPS2);	// prescaler 32
	ADCSRA |= (1<<ADSC);						// initial measurement
}

/*
 * Set SS line idle
 */
void SPI_SS_low(void) {
	PORTB &= ~SS_CTRL;
}

/*
 * Set SS line in transfer mode
 */
void SPI_SS_high(void) {
	PORTB |= SS_CTRL;
}

/*
 * SPI transfer execution
 */
uint8_t SPI_transfer(uint8_t byte) {
	SPDR = byte;
	while( !(SPSR & (1<<SPIF)) );
	return SPDR;
}

/*
 * Refresh LCD display
 */
void lcd_refresh(void) {
	if (lcd_refresh_flag == 10) {
		//lcd_cls();

		// J1
		lcd_locate(0,2);
		lcd_int(drive_state[0]);
		// J2
		lcd_locate(0,8);
		lcd_int(drive_state[1]);
		// D3
		lcd_locate(1,2);
		lcd_int(drive_state[2]);
		// J4
		lcd_locate(1,8);
		lcd_int(drive_state[3]);

		lcd_locate(0,0);
		btn_sign = (btn_state[0]==BTN_L)  ? '-'  : ( (btn_state[0]==BTN_R) ? '+' : ' ' ) ;
		lcd_char(btn_sign);
		lcd_locate(0,6);
		btn_sign = (btn_state[1]==BTN_L)  ? '-'  : ( (btn_state[1]==BTN_R) ? '+' : ' ' ) ;
		lcd_char(btn_sign);
		lcd_locate(1,0);
		btn_sign = (btn_state[2]==BTN_L)  ? '-'  : ( (btn_state[2]==BTN_R) ? '+' : ' ' ) ;
		lcd_char(btn_sign);
		lcd_locate(1,6);
		btn_sign = (btn_state[3]==BTN_L)  ? '-'  : ( (btn_state[3]==BTN_R) ? '+' : ' ' ) ;
		lcd_char(btn_sign);

		lcd_locate(0,15);
		lcd_int(btn_state[4]);
		lcd_locate(1,15);
		lcd_int(btn_state[5]);

		lcd_refresh_flag = 0;
	} else {
		lcd_refresh_flag++;
	}
}

/*
 * Button mapping ADC measurement to btn_state
 */
void map_btn_state(uint8_t btn_meas) {
	if ( i != 4) {
		if ( (btn_meas > 100) && (btn_meas < 130) ) {
			btn_state[i] = BTN_L;
		} else if ( (btn_meas > 150) && (btn_meas < 210) ) {
			btn_state[i] = BTN_R;
		} else {
			btn_state[i] = BTN_OFF;
		}
	} else if ( i == 4 ) {
		if ( (btn_meas > 100) && (btn_meas < 130) ) {
			btn_state[4] = BTN_ON;
		} else if ( (btn_meas > 150) && (btn_meas < 210) ) {
			btn_state[5] = BTN_ON;
		} else {
			btn_state[4] = btn_state[5] = BTN_OFF;
		}
	}
}

/*
 * ADC interrupt handle
 * - Reading button states
 */
ISR(ADC_vect) {
	map_btn_state(ADCH);
	ADMUX = (ADMUX & 0xE0) + i;
	if(i<4) {
		i++;
	} else {
		i = 0;
	}
	ADCSRA |= (1<<ADSC);	// start next measurement
}

/*
 * Timer2 interrupt handle
 * - Makes SPI connection and transfers data
 */
ISR(TIMER2_COMP_vect) {
	SPI_SS_low();
	drive_state[j] = SPI_transfer(btn_state[j]);
	SPI_SS_high();
	if(j<5) {
		j++;
	} else {
		j = 0;
	}
	lcd_refresh();
}

int main(void)
{
	Timer2_init();
	sei();
	ADC_init();
	SPI_init();
	lcd_init();
	lcd_defchar(0x80, deg_sign);
    while(1)
    {

    }
	return 0;
}