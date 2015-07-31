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
#include "config.h"
#include "timer.h"
#include "led.h"

#define DAT PB0
#define CLK PB1



typedef enum {
	s_changeWait,
	s_sending,
	s_flush,
	s_flushWait,
} s_ledStates;




static uint8_t memory_leds[NUM_LEDS][3];
static uint8_t memory_leds_change=0;

static void led_pushDataset( uint8_t r, uint8_t g, uint8_t b );
static void led_flush( void );
static void led_sendByte( uint8_t data );
static uint16_t gamma16( uint8_t input );



void led_init( void ) {
	uint8_t i;

	// LEDs
	DDRB |= (1<<PB0); // DAT
	DDRB |= (1<<PB1); // CLK

	// default is black
	for(i=0; i<NUM_LEDS; i++ ) {
		memory_leds[i][0] = 0;
		memory_leds[i][1] = 0;
		memory_leds[i][2] = 0;
	}
}



void led_clear( void ) {
	uint8_t i;
	
	for( i=0; i<NUM_LEDS; i++ ) {
		led_set( i, 0,0,0 );
	}
	memory_leds_change = 1;
}




void led_set( uint8_t ledId, uint8_t r, uint8_t g, uint8_t b ) {
	if( ledId < NUM_LEDS ) {
		memory_leds[ledId][0] = r;
		memory_leds[ledId][1] = g;
		memory_leds[ledId][2] = b;
	}
	memory_leds_change = 1;
	
}



void led_run( void ) {
	static uint32_t timeout=0;
	uint32_t now = timer_getMs();
	static uint8_t state=s_changeWait;
	uint8_t i;

	switch( state ) {
		case s_sending:
			for( i=0; i<NUM_LEDS; i++ ) {
				led_pushDataset( memory_leds[i][0], memory_leds[i][1], memory_leds[i][2] );
			}
			state = s_flush;
		break;

		case s_flush:
			state = s_flushWait;

			timeout = now +1;
			led_flush( );
		break;

		case s_flushWait:
			// protocol requires us to wait for 500 us
			// so we wait at least 1 ms
			if( timeout < now ) {
				state = s_changeWait;
			}
		break;

		case s_changeWait:
			if( memory_leds_change ) {
				memory_leds_change = 0;
				state = s_sending;
			}
		break;

		default:
			state = s_flushWait;
		break;
	}
}



static void led_pushDataset( uint8_t r, uint8_t g, uint8_t b ) {
	led_sendByte( r );
	led_sendByte( g );
	led_sendByte( b );
}



static void led_flush( void ) {
	// activate newest dataset
	// set clock pin to LOW for at least 500 uS
	PORTB &= ~(1<<DAT);
	PORTB &= ~(1<<CLK); // clock
	return;
}



static void led_sendByte( uint8_t data ) {
	int i;

	for(i=0; i<8; i++) {
		PORTB &= ~(1<<CLK);
		if( (data<<i)&0x80 )
			PORTB |= (1<<DAT);
		else
			PORTB &= ~(1<<DAT);

		PORTB |= (1<<CLK); // send
		PORTB &= ~(1<<CLK);
	}
}



static uint16_t gamma16( uint8_t input ) {
	/*

	 * 16 bits to 8 bit CIE Lightness conversion
	 * L* = 116(Y/Yn)^1/3 - 16 , Y/Yn > 0.008856
	 * L* = 903.3(Y/Yn), Y/Yn <= 0.008856
	*/

	uint16_t pwm_table[] = {
		65535,    65508,    65479,    65451,    65422,    65394,    65365,    65337,
		65308,    65280,    65251,    65223,    65195,    65166,    65138,    65109,
		65081,    65052,    65024,    64995,    64967,    64938,    64909,    64878,
		64847,    64815,    64781,    64747,    64711,    64675,    64637,    64599,
		64559,    64518,    64476,    64433,    64389,    64344,    64297,    64249,
		64200,    64150,    64099,    64046,    63992,    63937,    63880,    63822,
		63763,    63702,    63640,    63577,    63512,    63446,    63379,    63310,
		63239,    63167,    63094,    63019,    62943,    62865,    62785,    62704,
		62621,    62537,    62451,    62364,    62275,    62184,    62092,    61998,
		61902,    61804,    61705,    61604,    61501,    61397,    61290,    61182,
		61072,    60961,    60847,    60732,    60614,    60495,    60374,    60251,
		60126,    59999,    59870,    59739,    59606,    59471,    59334,    59195,
		59053,    58910,    58765,    58618,    58468,    58316,    58163,    58007,
		57848,    57688,    57525,    57361,    57194,    57024,    56853,    56679,
		56503,    56324,    56143,    55960,    55774,    55586,    55396,    55203,
		55008,    54810,    54610,    54408,    54203,    53995,    53785,    53572,
		53357,    53140,    52919,    52696,    52471,    52243,    52012,    51778,
		51542,    51304,    51062,    50818,    50571,    50321,    50069,    49813,
		49555,    49295,    49031,    48764,    48495,    48223,    47948,    47670,
		47389,    47105,    46818,    46529,    46236,    45940,    45641,    45340,
		45035,    44727,    44416,    44102,    43785,    43465,    43142,    42815,
		42486,    42153,    41817,    41478,    41135,    40790,    40441,    40089,
		39733,    39375,    39013,    38647,    38279,    37907,    37531,    37153,
		36770,    36385,    35996,    35603,    35207,    34808,    34405,    33999,
		33589,    33175,    32758,    32338,    31913,    31486,    31054,    30619,
		30181,    29738,    29292,    28843,    28389,    27932,    27471,    27007,
		26539,    26066,    25590,    25111,    24627,    24140,    23649,    23153,
		22654,    22152,    21645,    21134,    20619,    20101,    19578,    19051,
		18521,    17986,    17447,    16905,    16358,    15807,    15252,    14693,
		14129,    13562,    12990,    12415,    11835,    11251,    10662,    10070,
		9473,     8872,     8266,     7657,     7043,     6424,     5802,     5175,
		4543,     3908,     3267,     2623,     1974,     1320,     662,      0
	};

	return 0xFFFF - pwm_table[input];
}
