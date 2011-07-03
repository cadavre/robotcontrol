/*
 * atmega_slave.c
 *
 * Created on: 26.06.2011
 *     Author: Seweryn 'cadavre' Zeman
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include "atmega_slave_conf.h"

uint8_t drive_state[6] = {49,50,51,180,0,0};
volatile uint8_t btn_state[6] = {BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF,BTN_OFF};

volatile uint8_t j = 0;

///*
 //* Timer2 initialization
 //*/
//void Timer2_init(void) {
	//TCCR2 |= (1<<WGM21);						// CTC
	//TCCR2 |= (1<<CS21)|(1<<CS22);				// prescaler 256
	//OCR2 = 0x24;								// 36 = ~1.15ms
	//TIMSK |= (1<<OCIE2);						// compare match IRQ
//}

/*
 * SPI transfer interface initialization
 */
void SPI_init(void) {
	DDRB |= MISO;
	SPCR |= (1<<SPE)|(1<<SPIE);
	SPDR = 0;
}

///*
 //* SPI transfer execution
 //*/
//uint8_t SPI_transfer(uint8_t byte) {
	//SPDR = byte;
	//while( !(SPSR & (1<<SPIF)) );
	//return SPDR;
//}

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

//ISR(TIMER2_COMP_vect) {
	//btn_state[j] = SPI_transfer(drive_state[j]);
	//if(j<5) {
		//j++;
	//} else {
		//j = 0;
	//}
//}

int main(void)
{
	//Timer2_init();
	sei();
	SPI_init();
    while(1)
    {
		drive_state[0]++;
    }
}