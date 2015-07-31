/*
	This file is part of LeuchtSchild.
	LeuchtSchild is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	LeuchtSchild is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with LeuchtSchild.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "config.h"
#include "random.h"
#include "led.h"
#include "can/can.h"
#include "timer.h"
#include "pattern.h"


void soft_reset( void );
int  my_can_init(void);
void goto_sleep_mode(void);
int  goto_active_mode(void);


/*
// Function Pototype
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init1")));

// Function Implementation
void wdt_init(void) {
	MCUSR = 0;
	wdt_disable();
	return;
}
*/

void soft_reset( void ) {
	wdt_enable(WDTO_500MS);
	while(1);
}



int main( void ) {
	uint8_t i;
	can_t msg_tx, msg_rx;
	uint32_t now, t_send_heartbeat=0;
	uint32_t t_sleep = 60000; // stay awake for 1 minute initially
	uint8_t l_mode = 7;

	timer_init( );
	led_init( );
	my_can_init( );
	sei( );


	wdt_enable(WDTO_500MS);
	while( 1 ) {
		wdt_reset();

		now = timer_getMs( );
		led_run( );

		
		if( now > t_sleep ) {
			led_clear( );
			uint32_t t_clear = timer_getMs( ) + 100;
			while ( timer_getMs() > c_clear ) {
			  led_run( );
			}
			wdt_reset();
			goto_sleep_mode();
		}
		

		if( t_send_heartbeat + 1000 < now ) {
			t_send_heartbeat = now;

			msg_tx.id = HEARTBEAT_MSG;
			msg_tx.flags.extended = 1;
			msg_tx.flags.rtr = 0;
			msg_tx.length = 2;
			msg_tx.data[0] = l_mode;

			can_send_message( &msg_tx );
			msg_tx.data[1]++;
		}


		// receive CAN
		if ( can_check_message() ) {
			can_get_message( &msg_rx );
			//t_sleep = now;

			switch( msg_rx.id ) {
				case RESET_MSG:
					soft_reset( );
				break;

				case SWITCH_MODE_MSG:
					l_mode = msg_rx.data[0];
				break;

				case SET_COLOR_MSG: // [R][G][B]
				break;

				case REQ_DISP_CONTENT_MSG: // [RATE(fps)]
				break;

				case SET_LED_MSG: // [R][G][B][LEDID]
				break;

				case SET_SLEEPTIME: // Set remaining time to stay awake
					t_sleep = now + ((((msg_rx.data[0])<<8) | ((msg_rx.data[1])<<0))*1000UL);
					if (msg_rx.data[2] != 0xff) {
						l_mode = msg_rx.data[2];
					}
					msg_tx.data[0] = t_sleep&0xff;
					msg_tx.data[1] = (t_sleep>>8)&0xff;
					msg_tx.data[2] = (t_sleep>>16)&0xff;
					msg_tx.data[3] = (t_sleep>>24)&0xff;
					msg_tx.data[4] = now&0xff;
					msg_tx.data[5] = (now>>8)&0xff;
					msg_tx.data[6] = (now>>16)&0xff;
					msg_tx.data[7] = (now>>24)&0xff;
					msg_tx.length = 8;
					msg_tx.id = 0x1337da71;
					msg_tx.flags.extended = 1;
					msg_tx.flags.rtr = 0;
					can_send_message( &msg_tx );
					
				break;
			}
		}


		switch( l_mode ) {

			case 0:
				led_clear( );
			break;

			case 1:
				pattern_fading( );
			break;

			default:
			break;
		}
	}
	return 0;
}



int my_can_init(void) {
	can_init( BITRATE_500_KBPS );

	can_filter_t can_filter = {     .id = 0, .mask = 0, .flags = { .rtr = 0, .extended = 0 } };
	can_set_filter( 0, &can_filter );
	can_set_filter( 1, &can_filter );
	can_set_filter( 2, &can_filter );

	// PB3 == STDBY
	// PB4 == EN
	DDRB  |= (1<<PB3)|(1<<PB4);
	goto_active_mode();

	return 0;
}



void goto_sleep_mode( void ) {
	// PB3 == STDBY
	// PB4 == EN
	PORTB &= ~(1<<PB3);
	PORTB |=  (1<<PB4);

	wdt_enable(WDTO_500MS);
	while(1) { ; } // wait for sleep or watchdog reset
}



int goto_active_mode( void ) {
	// PB3 == STDBY
	// PB4 == EN
	PORTB |= (1<<PB3)|(1<<PB4);
	return 0;
}

