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
#include "led.h"
#include "timer.h"
#include "pattern.h"
#include "random.h"

//##################
//# 20          38 #
//#             39 #
//# 19          01 #
//##################


void hsv_to_rgb( uint8_t h, uint8_t s, uint8_t v, uint8_t* red, uint8_t* green, uint8_t* blue ) {
	unsigned char r,g,b, i, f;
	unsigned int p, q, t;

	if( s == 0 ) {
		r = g = b = v;
	}
	else {
		i=h/43;
		f=h%43;
		p = (v * (255 - s))/256;
		q = (v * ((10710 - (s * f))/42))/256;
		t = (v * ((10710 - (s * (42 - f)))/42))/256;

		switch( i ) {
			case 0:
				r = v; g = t; b = p;
			break;

			case 1:
				r = q; g = v; b = p;
			break;

			case 2:
				r = p; g = v; b = t;
			break;

			case 3:
				r = p; g = q; b = v;
			break;

			case 4:
				r = t; g = p; b = v;
			break;

			case 5:
				r = v; g = p; b = q;
			break;
		}
	}

	*red=r, *green=g, *blue=b;
}



void pattern_fading( void ) {
	int j;
	uint8_t color[3];
	static uint8_t last_color=0;
	uint32_t now;
	static uint32_t delay=0;

	now = timer_getMs( );

	if( delay > now ) {
		return;
	}

	delay = now + 10;
	for( j=0; j<NUM_LEDS; j++ ) {
		hsv_to_rgb( last_color+(6*j), 0xff/*s*/, 0xff/*v*/, &color[0], &color[1], &color[2] );
		led_set( j, color[0], color[1], color[2] );
	}
	last_color++;
}



void pattern_fading_kitsch( void ) {
	int j;
	uint8_t color[3];
	static uint8_t last_color=0;
	uint32_t now;
	static uint32_t delay=0;

	now = timer_getMs( );

	if( delay > now ) {
		return;
	}

	delay = now + 20;
	hsv_to_rgb( last_color+(6*j), 0xff/*s*/, 0xff/*v*/, &color[0], &color[1], &color[2] );
	for( j=0; j<NUM_LEDS; j++ ) {
		led_set( j, color[0], color[1], color[2] );
	}
	last_color++;
}



void pattern_bingo( void ) {
	int j;
	uint8_t color[3];
	static uint8_t last_color=0;
	uint32_t now;
	static uint32_t delay=0;

	now = timer_getMs( );

	if( delay > now ) {
		return;
	}
	delay = now + 20;

	// all grey
	for( j=0; j<NUM_LEDS; j++ ) {
		led_set( j, 0x70, 0x70, 0x70 );
	}

	// add bingo rainbow
	hsv_to_rgb( last_color+(6*j), 0xff/*s*/, 0xff/*v*/, &color[0], &color[1], &color[2] );
	for( j=20; j<25; j++ ) {
		led_set( j,   color[0], color[1], color[2] );
	}
	last_color++;
}



void pattern_scanner( void ) {
	// move a colored bar over the sign back and forth
	int j;
	static uint8_t frame=0;
	static uint8_t last_color=0;
	static uint32_t delay=0;
	uint8_t color[3];
	uint32_t now;

	now = timer_getMs( );

	if( delay > now ) {
		return; // only continue in set intervals
	}
	delay = now +50;

	// all grey
	for( j=0; j<NUM_LEDS; j++ ) {
		led_set( j, 0x70, 0x70, 0x70 );
	}

	if( frame == 19 || frame == 0 ) {
		last_color = get_random( );
	}

	hsv_to_rgb( last_color, 0xff/*s*/, 0xff/*v*/, &color[0], &color[1], &color[2] );

	switch( frame ) {
		case 0:
			led_set(  0, color[0], color[1], color[2] );
			led_set( 39, color[0], color[1], color[2] );
			led_set(  1, 0x00, 0x00, 0x00 );
			led_set( 38, 0x00, 0x00, 0x00 );
		break;

		case 1:
		case 37:
			led_set(  0, 0x00, 0x00, 0x00 );
			led_set( 39, 0x00, 0x00, 0x00 );
			led_set(  1, color[0], color[1], color[2] );
			led_set( 38, color[0], color[1], color[2] );
			led_set(  2, 0x00, 0x00, 0x00 );
			led_set( 37, 0x00, 0x00, 0x00 );
		break;

		case 2:
		case 36:
			led_set(  1, 0x00, 0x00, 0x00 );
			led_set( 38, 0x00, 0x00, 0x00 );
			led_set(  2, color[0], color[1], color[2] );
			led_set( 37, color[0], color[1], color[2] );
			led_set(  3, 0x00, 0x00, 0x00 );
			led_set( 36, 0x00, 0x00, 0x00 );
		break;

		case 3:
		case 35:
			led_set(  2, 0x00, 0x00, 0x00 );
			led_set( 37, 0x00, 0x00, 0x00 );
			led_set(  3, color[0], color[1], color[2] );
			led_set( 36, color[0], color[1], color[2] );
			led_set(  4, 0x00, 0x00, 0x00 );
			led_set( 35, 0x00, 0x00, 0x00 );
		break;

		case 4:
		case 34:
			led_set(  3, 0x00, 0x00, 0x00 );
			led_set( 36, 0x00, 0x00, 0x00 );
			led_set(  4, color[0], color[1], color[2] );
			led_set( 35, color[0], color[1], color[2] );
			led_set(  5, 0x00, 0x00, 0x00 );
			led_set( 34, 0x00, 0x00, 0x00 );
		break;

		case 5:
		case 33:
			led_set(  4, 0x00, 0x00, 0x00 );
			led_set( 35, 0x00, 0x00, 0x00 );
			led_set(  5, color[0], color[1], color[2] );
			led_set( 34, color[0], color[1], color[2] );
			led_set(  6, 0x00, 0x00, 0x00 );
			led_set( 33, 0x00, 0x00, 0x00 );
		break;

		case 6:
		case 32:
			led_set(  5, 0x00, 0x00, 0x00 );
			led_set( 34, 0x00, 0x00, 0x00 );
			led_set(  6, color[0], color[1], color[2] );
			led_set( 33, color[0], color[1], color[2] );
			led_set(  7, 0x00, 0x00, 0x00 );
			led_set( 32, 0x00, 0x00, 0x00 );
		break;

		case 7:
		case 31:
			led_set(  6, 0x00, 0x00, 0x00 );
			led_set( 33, 0x00, 0x00, 0x00 );
			led_set(  7, color[0], color[1], color[2] );
			led_set( 32, color[0], color[1], color[2] );
			led_set(  8, 0x00, 0x00, 0x00 );
			led_set( 31, 0x00, 0x00, 0x00 );
		break;

		case 8:
		case 30:
			led_set(  7, 0x00, 0x00, 0x00 );
			led_set( 32, 0x00, 0x00, 0x00 );
			led_set(  8, color[0], color[1], color[2] );
			led_set( 31, color[0], color[1], color[2] );
			led_set(  9, 0x00, 0x00, 0x00 );
			led_set( 30, 0x00, 0x00, 0x00 );
		break;

		case 9:
		case 29:
			led_set(  8, 0x00, 0x00, 0x00 );
			led_set( 31, 0x00, 0x00, 0x00 );
			led_set(  9, color[0], color[1], color[2] );
			led_set( 30, color[0], color[1], color[2] );
			led_set( 10, 0x00, 0x00, 0x00 );
			led_set( 29, 0x00, 0x00, 0x00 );
		break;

		case 10:
		case 28:
			led_set(  9, 0x00, 0x00, 0x00 );
			led_set( 30, 0x00, 0x00, 0x00 );
			led_set( 10, color[0], color[1], color[2] );
			led_set( 29, color[0], color[1], color[2] );
			led_set( 11, 0x00, 0x00, 0x00 );
			led_set( 28, 0x00, 0x00, 0x00 );
		break;

		case 11:
		case 27:
			led_set( 10, 0x00, 0x00, 0x00 );
			led_set( 29, 0x00, 0x00, 0x00 );
			led_set( 11, color[0], color[1], color[2] );
			led_set( 28, color[0], color[1], color[2] );
			led_set( 12, 0x00, 0x00, 0x00 );
			led_set( 27, 0x00, 0x00, 0x00 );
		break;

		case 12:
		case 26:
			led_set( 11, 0x00, 0x00, 0x00 );
			led_set( 28, 0x00, 0x00, 0x00 );
			led_set( 12, color[0], color[1], color[2] );
			led_set( 27, color[0], color[1], color[2] );
			led_set( 13, 0x00, 0x00, 0x00 );
			led_set( 26, 0x00, 0x00, 0x00 );
		break;

		case 13:
		case 25:
			led_set( 12, 0x00, 0x00, 0x00 );
			led_set( 27, 0x00, 0x00, 0x00 );
			led_set( 13, color[0], color[1], color[2] );
			led_set( 26, color[0], color[1], color[2] );
			led_set( 14, 0x00, 0x00, 0x00 );
			led_set( 25, 0x00, 0x00, 0x00 );
		break;

		case 14:
		case 24:
			led_set( 13, 0x00, 0x00, 0x00 );
			led_set( 26, 0x00, 0x00, 0x00 );
			led_set( 14, color[0], color[1], color[2] );
			led_set( 25, color[0], color[1], color[2] );
			led_set( 15, 0x00, 0x00, 0x00 );
			led_set( 24, 0x00, 0x00, 0x00 );
		break;

		case 15:
		case 23:
			led_set( 14, 0x00, 0x00, 0x00 );
			led_set( 25, 0x00, 0x00, 0x00 );
			led_set( 15, color[0], color[1], color[2] );
			led_set( 24, color[0], color[1], color[2] );
			led_set( 16, 0x00, 0x00, 0x00 );
			led_set( 23, 0x00, 0x00, 0x00 );
		break;

		case 16:
		case 22:
			led_set( 15, 0x00, 0x00, 0x00 );
			led_set( 24, 0x00, 0x00, 0x00 );
			led_set( 16, color[0], color[1], color[2] );
			led_set( 23, color[0], color[1], color[2] );
			led_set( 17, 0x00, 0x00, 0x00 );
			led_set( 22, 0x00, 0x00, 0x00 );
		break;

		case 17:
		case 21:
			led_set( 16, 0x00, 0x00, 0x00 );
			led_set( 23, 0x00, 0x00, 0x00 );
			led_set( 17, color[0], color[1], color[2] );
			led_set( 22, color[0], color[1], color[2] );
			led_set( 18, 0x00, 0x00, 0x00 );
			led_set( 21, 0x00, 0x00, 0x00 );
		break;

		case 18:
		case 20:
			led_set( 17, 0x00, 0x00, 0x00 );
			led_set( 22, 0x00, 0x00, 0x00 );
			led_set( 18, color[0], color[1], color[2] );
			led_set( 21, color[0], color[1], color[2] );
			led_set( 19, 0x00, 0x00, 0x00 );
			led_set( 20, 0x00, 0x00, 0x00 );
		break;

		case 19:
			led_set( 18, 0x00, 0x00, 0x00 );
			led_set( 21, 0x00, 0x00, 0x00 );
			led_set( 19, color[0], color[1], color[2] );
			led_set( 20, color[0], color[1], color[2] );
		break;

		default:
		break;
	}

	frame++;
	if( frame > 37 ) {
		frame = 0;
	}
}



void patternRandomDiscret( void ) {
	int j;
	uint16_t rnd;
	uint8_t color[3];
	static uint32_t delay=0;

	uint32_t now = timer_getMs( );

	if( delay > now ) {
		return; // only continue in set intervals
	}
	delay = now +250;

	for(j=0; j<NUM_LEDS; j++) {
		rnd = 0;
		while( !(rnd&7) || ((rnd&7)==7)) { // we don't want 000 or 111
			rnd = get_random( );
		}

		color[0] = rnd& 0x1? 0xff: 0x00;
		color[1] = rnd& 0x2? 0xff: 0x00;
		color[2] = rnd& 0x4? 0xff: 0x00;

		led_set( j, color[0], color[1], color[2] );
	}
}



void patternRandomDiscretPicky( void ) {
	uint8_t j;
	uint16_t rnd;
	uint8_t color[3];
	static uint32_t t_delay=0;

	uint32_t now = timer_getMs( );

	if( t_delay > now ) {
		return; // only continue in set intervals
	}
	t_delay = now +50;

	rnd = 0;
	while( !(rnd&7) || ((rnd&7)==7)) { // we don't want 000 or 111
		rnd = get_random( );
	}

	j = (rnd>>8) % NUM_LEDS; // choose a single LED
	color[0] = rnd& 0x1? 0xff: 0x00;
	color[1] = rnd& 0x2? 0xff: 0x00;
	color[2] = rnd& 0x4? 0xff: 0x00;

	led_set( j, color[0], color[1], color[2] );
}



//##################
//# 20          38 #
//#             39 #
//# 19          01 #
//##################
/*
void pattern_goldenWave( void ) {
	// move a wave of brighte and dim yellow over the display
	const uint8_t color[][] = {
		{ 0xff, 0xff, 0x00 },
		{ 0xf0, 0xf0, 0x00 },
		{ 0xe0, 0xe0, 0x00 },
		{ 0xd0, 0xd0, 0x00 },
		{ 0xc0, 0xc0, 0x00 },
		{ 0xb0, 0xb0, 0x00 },
		{ 0xa0, 0xa0, 0x00 },
		{ 0x90, 0x90, 0x00 },
		{ 0x80, 0x80, 0x00 }
	};

	const uint8_t leds[][] = {
		{ 0, 39 },// 0
		{ 1, 38 },
		{ 2, 37 },
		{ 3, 36 },
		{ 4, 35 },
		{ 5, 34 },
		{ 6, 33 },
		{ 7, 32 },
		{ 8, 31 },
		{ 9, 29 },
		{10, 28 },
		{11, 27 },
		{12, 26 },
		{13, 25 },
		{14, 24 },
		{15, 23 },
		{16, 22 },
		{17, 21 },
		{18, 20 }// 18
	};



	int i;
	static uint32_t delay=0;
	static uint8_t highlight_pos=0;
	uint32_t now;

	now = timer_getMs( );

	if( delay > now ) {
		return; // only continue in set intervals
	}
	delay = now +50;

	// all dark yellow
	for( i=0; i<NUM_LEDS; i++ ) {
		led_set( i, 0x80, 0x80, 0x00 );
	}

	for( i=0; i<18; i++ ) {
		if( highlight_pos > i) {
			// TODO
		}
		else {
		}
	}


	if( frame == 19 || frame == 0 ) {
		last_color = get_random( );
	}
*/
//	hsv_to_rgb( last_color, 0xff/*s*/, 0xff/*v*/, &color[0], &color[1], &color[2] );
/*
	switch( frame ) {
		case 0:
			led_set(  0, color[0][0], color[0][1], color[0][1] );
			led_set( 39, color[0][0], color[0][1], color[0][1] );

			led_set(  1, color[1][0], color[1][1], color[1][1] );
			led_set( 38, color[1][0], color[1][1], color[1][1] );

			led_set(  2, color[2][0], color[2][1], color[2][1] );
			led_set( 37, color[2][0], color[2][1], color[2][1] );

			led_set(  3, color[3][0], color[3][1], color[3][1] );
			led_set( 36, color[3][0], color[3][1], color[3][1] );

			led_set(  4, color[4][0], color[4][1], color[4][1] );
			led_set( 35, color[4][0], color[4][1], color[4][1] );

			led_set(  1, color[1][0], color[1][1], color[1][1] );
			led_set( 38, color[1][0], color[1][1], color[1][1] );
			led_set(  1, color[1][0], color[1][1], color[1][1] );
			led_set( 38, color[1][0], color[1][1], color[1][1] );
		break;

		case 1:
		case 37:
			led_set(  0, 0x00, 0x00, 0x00 );
			led_set( 39, 0x00, 0x00, 0x00 );
			led_set(  1, color[0], color[1], color[2] );
			led_set( 38, color[0], color[1], color[2] );
			led_set(  2, 0x00, 0x00, 0x00 );
			led_set( 37, 0x00, 0x00, 0x00 );
		break;

		case 2:
		case 36:
			led_set(  1, 0x00, 0x00, 0x00 );
			led_set( 38, 0x00, 0x00, 0x00 );
			led_set(  2, color[0], color[1], color[2] );
			led_set( 37, color[0], color[1], color[2] );
			led_set(  3, 0x00, 0x00, 0x00 );
			led_set( 36, 0x00, 0x00, 0x00 );
		break;

		case 3:
		case 35:
			led_set(  2, 0x00, 0x00, 0x00 );
			led_set( 37, 0x00, 0x00, 0x00 );
			led_set(  3, color[0], color[1], color[2] );
			led_set( 36, color[0], color[1], color[2] );
			led_set(  4, 0x00, 0x00, 0x00 );
			led_set( 35, 0x00, 0x00, 0x00 );
		break;

		case 4:
		case 34:
			led_set(  3, 0x00, 0x00, 0x00 );
			led_set( 36, 0x00, 0x00, 0x00 );
			led_set(  4, color[0], color[1], color[2] );
			led_set( 35, color[0], color[1], color[2] );
			led_set(  5, 0x00, 0x00, 0x00 );
			led_set( 34, 0x00, 0x00, 0x00 );
		break;

		case 5:
		case 33:
			led_set(  4, 0x00, 0x00, 0x00 );
			led_set( 35, 0x00, 0x00, 0x00 );
			led_set(  5, color[0], color[1], color[2] );
			led_set( 34, color[0], color[1], color[2] );
			led_set(  6, 0x00, 0x00, 0x00 );
			led_set( 33, 0x00, 0x00, 0x00 );
		break;

		case 6:
		case 32:
			led_set(  5, 0x00, 0x00, 0x00 );
			led_set( 34, 0x00, 0x00, 0x00 );
			led_set(  6, color[0], color[1], color[2] );
			led_set( 33, color[0], color[1], color[2] );
			led_set(  7, 0x00, 0x00, 0x00 );
			led_set( 32, 0x00, 0x00, 0x00 );
		break;

		case 7:
		case 31:
			led_set(  6, 0x00, 0x00, 0x00 );
			led_set( 33, 0x00, 0x00, 0x00 );
			led_set(  7, color[0], color[1], color[2] );
			led_set( 32, color[0], color[1], color[2] );
			led_set(  8, 0x00, 0x00, 0x00 );
			led_set( 31, 0x00, 0x00, 0x00 );
		break;

		case 8:
		case 30:
			led_set(  7, 0x00, 0x00, 0x00 );
			led_set( 32, 0x00, 0x00, 0x00 );
			led_set(  8, color[0], color[1], color[2] );
			led_set( 31, color[0], color[1], color[2] );
			led_set(  9, 0x00, 0x00, 0x00 );
			led_set( 30, 0x00, 0x00, 0x00 );
		break;

		case 9:
		case 29:
			led_set(  8, 0x00, 0x00, 0x00 );
			led_set( 31, 0x00, 0x00, 0x00 );
			led_set(  9, color[0], color[1], color[2] );
			led_set( 30, color[0], color[1], color[2] );
			led_set( 10, 0x00, 0x00, 0x00 );
			led_set( 29, 0x00, 0x00, 0x00 );
		break;

		case 10:
		case 28:
			led_set(  9, 0x00, 0x00, 0x00 );
			led_set( 30, 0x00, 0x00, 0x00 );
			led_set( 10, color[0], color[1], color[2] );
			led_set( 29, color[0], color[1], color[2] );
			led_set( 11, 0x00, 0x00, 0x00 );
			led_set( 28, 0x00, 0x00, 0x00 );
		break;

		case 11:
		case 27:
			led_set( 10, 0x00, 0x00, 0x00 );
			led_set( 29, 0x00, 0x00, 0x00 );
			led_set( 11, color[0], color[1], color[2] );
			led_set( 28, color[0], color[1], color[2] );
			led_set( 12, 0x00, 0x00, 0x00 );
			led_set( 27, 0x00, 0x00, 0x00 );
		break;

		case 12:
		case 26:
			led_set( 11, 0x00, 0x00, 0x00 );
			led_set( 28, 0x00, 0x00, 0x00 );
			led_set( 12, color[0], color[1], color[2] );
			led_set( 27, color[0], color[1], color[2] );
			led_set( 13, 0x00, 0x00, 0x00 );
			led_set( 26, 0x00, 0x00, 0x00 );
		break;

		case 13:
		case 25:
			led_set( 12, 0x00, 0x00, 0x00 );
			led_set( 27, 0x00, 0x00, 0x00 );
			led_set( 13, color[0], color[1], color[2] );
			led_set( 26, color[0], color[1], color[2] );
			led_set( 14, 0x00, 0x00, 0x00 );
			led_set( 25, 0x00, 0x00, 0x00 );
		break;

		case 14:
		case 24:
			led_set( 13, 0x00, 0x00, 0x00 );
			led_set( 26, 0x00, 0x00, 0x00 );
			led_set( 14, color[0], color[1], color[2] );
			led_set( 25, color[0], color[1], color[2] );
			led_set( 15, 0x00, 0x00, 0x00 );
			led_set( 24, 0x00, 0x00, 0x00 );
		break;

		case 15:
		case 23:
			led_set( 14, 0x00, 0x00, 0x00 );
			led_set( 25, 0x00, 0x00, 0x00 );
			led_set( 15, color[0], color[1], color[2] );
			led_set( 24, color[0], color[1], color[2] );
			led_set( 16, 0x00, 0x00, 0x00 );
			led_set( 23, 0x00, 0x00, 0x00 );
		break;

		case 16:
		case 22:
			led_set( 15, 0x00, 0x00, 0x00 );
			led_set( 24, 0x00, 0x00, 0x00 );
			led_set( 16, color[0], color[1], color[2] );
			led_set( 23, color[0], color[1], color[2] );
			led_set( 17, 0x00, 0x00, 0x00 );
			led_set( 22, 0x00, 0x00, 0x00 );
		break;

		case 17:
		case 21:
			led_set( 16, 0x00, 0x00, 0x00 );
			led_set( 23, 0x00, 0x00, 0x00 );
			led_set( 17, color[0], color[1], color[2] );
			led_set( 22, color[0], color[1], color[2] );
			led_set( 18, 0x00, 0x00, 0x00 );
			led_set( 21, 0x00, 0x00, 0x00 );
		break;

		case 18:
		case 20:
			led_set( 17, 0x00, 0x00, 0x00 );
			led_set( 22, 0x00, 0x00, 0x00 );
			led_set( 18, color[0], color[1], color[2] );
			led_set( 21, color[0], color[1], color[2] );
			led_set( 19, 0x00, 0x00, 0x00 );
			led_set( 20, 0x00, 0x00, 0x00 );
		break;

		case 19:
			led_set( 18, 0x00, 0x00, 0x00 );
			led_set( 21, 0x00, 0x00, 0x00 );
			led_set( 19, color[0], color[1], color[2] );
			led_set( 20, color[0], color[1], color[2] );
		break;

		default:
		break;
	}

	frame++;
	if( frame > 37 ) {
		frame = 0;
	}
}
*/


void pattern_hold( void ) {
	// make use of the framebuffer and hold the last image
}

