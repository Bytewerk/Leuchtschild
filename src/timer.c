/*
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Martin Wenger <martin.wenger@arcormail.de> and Stefan Rupp <struppi@erlangen.ccc.de>
 * wrote this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me/us a beer in return.
 * (c) 2005-2007 Martin Wenger, Stefan Rupp
 */

#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include "timer.h"

/* TimerTime gets increased on every SIG_OUTPUT_COMPARE2 interrupt
	* those interrupts happen every 1ms
	*/
static volatile uint32_t timer_time;

ISR(TIMER0_COMPA_vect) {
	++timer_time;
	return;
}

/**
	* Initialize the timer
	* This function has to be called first, before calling TimerWait and/or TimerGet,
	* else TimerGet will always return 0, and TimerWait will wait forever!
	*/

void timer_init(void)
{
	// Reset timer to zero
	timer_time = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// - Time accuracy: 1 millisecond (corresponding frequency: 1kHz)
		// ==> F_CPU = 16Mhz
		// ==> 16Mhz / 64 = 250 kHz
		// ==> let timer count to 249 to get 1kHz frequency
		// therefore:
		// - Set Timer/Counter2 prescaler to 64 ==> (1<<CS22)
		// - Set OCR2 to 249
		// - CTC ( i.e. clear counter, when COUNTER == OCR2) ==> (1<<WGM21)
		TCCR0A = (1<<WGM01);
		TCCR0B = (1<<CS01)|(1<<CS00);
		OCR0A = 249;

		// Interrupts setzen
		TIMSK0 |= (1<<OCIE0A);
	}
}


/**
	* Get the current time
	* \return the current time (in ms)
	*/
uint32_t timer_get(void)
{
	uint32_t result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		result = timer_time;
	}
	return result;
}

/**
	* Wait for (at least) the specified amount of time
	* \param delay The time to wait (in ms)
	*/

void timer_wait(uint32_t delay)
{
	uint32_t end = timer_get() + delay +1;
	while ( end > timer_get() );
	return;
}
