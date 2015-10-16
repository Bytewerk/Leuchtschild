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
		led_set( j, color[0], color[1], color[2] );
	}
	last_color++;
}



void pattern_hold( void ) {
	// make use of the framebuffer and hold the last image
}



/*
void patternAnnoying( void ) {
	int j;
	uint16_t rnd, rnd2;
	uint8_t color[3];
	static uint8_t last_color=0;

	for(j=0; j<NUM_LEDS; j++) {
		switch( last_color ) {
			case 0:
				color[0] = 0xff;
				color[1] = 0xff;
				color[2] = 0xff;
			break;

			case 1:
				color[0] = 0xff;
				color[1] = 0x00;
				color[2] = 0x00;
			break;

			case 2:
				color[0] = 0x00;
				color[1] = 0xff;
				color[2] = 0x00;
			break;

			case 3:
				color[0] = 0x00;
				color[1] = 0x00;
				color[2] = 0xff;
			break;

			default:
			break;

		}

		led_pushDataset( color[0], color[1], color[2] );
	}

	// cycle through colors
	if( last_color > 2 )
		last_color = 0;
	else
		last_color++;

	led_execute( );
	_delay_ms(200);
}



void patternRandomDiscret( void ) {
	int j;
	uint16_t rnd;
	uint8_t color[3];

	for(j=0; j<NUM_LEDS; j++) {
		while( !(rnd&7) ) { // we don't want 000
			rnd = get_random( );
		}

		color[0] = rnd& 0x1? 0xff: 0x00;
		color[1] = rnd& 0x2? 0xff: 0x00;
		color[2] = rnd& 0x4? 0xff: 0x00;

		rnd = 0;

		led_pushDataset( color[0], color[1], color[2] );
	}
	led_execute( );
	_delay_ms(200);
}



void patternRandom( void ) {
	int j;
	uint16_t rnd, rnd2;
	uint8_t color[3];

	for(j=0; j<NUM_LEDS; j++) {
		rnd = get_random( );
		//rnd2 = get_random( );
				color[0] = (rnd>>8)/2;
				color[1] = (rnd>>8)/2;
				color[2] = rnd;


		led_pushDataset( color[0], color[1], color[2] );
	}
	led_execute( );
	_delay_ms(200);
}



void patternComets( void ) {
	int color1=0, color2=0, color3=0;
	int i=0, j=0;
	led_t leds[NUM_LEDS];


	for( i=0; i<NUM_LEDS; i++ ) {
		if( color1 == i ) {
			// spot
			leds[i].r = 0x40;

			// tail
			for(j=1; j<5; j++) {
				if( (i-j)<0 )
					leds[i-j+NUM_LEDS].r = 0x02*(5-j);
				else
					leds[i-j].r = 0x02*(5-j);
			}
		}
		else {
			leds[i].r = 0x00;
		}
	}



		for( i=0; i<NUM_LEDS; i++ ) {
				if( color2 == i ) {
					// spot
					leds[i].b = 0x40;

					// tail
					for(j=1; j<5; j++) {
						if( (i-j)<0 )
							leds[i-j+NUM_LEDS].b = 0x02*(5-j);
						else
							leds[i-j].b = 0x02*(5-j);
					}
				}
				else {
					leds[i].b = 0x00;
				}
		}



		for( i=0; i<NUM_LEDS; i++ ) {
				if( color3 == i ) {
					// spot
					leds[i].g = 0x40;

					// tail
					for(j=1; j<5; j++) {
						if( (i-j)<0 )
							leds[i-j+NUM_LEDS].g = 0x02*(5-j);
						else
							leds[i-j].g = 0x02*(5-j);
					}
				}
				else {
					leds[i].g = 0x00;
				}
		}



		color1++;
		if(color1 > NUM_LEDS)
			color1 = 0;

		color2++;
		color2++;
		if(color2 > NUM_LEDS)
			color2 = 0;

		color3++;
		color3++;
		color3++;
		if(color3 > NUM_LEDS)
			color3 = 0;

		for( i=0; i<40; i++ ) {
			led_pushDataset( leds[i].r, leds[i].g, leds[i].b );
		}
		led_execute( );
		_delay_ms(200);

}



#define FADE_STEPS 20
void patternRandomTransition ( void ) {
	int color1=0, color2=0, color3=0;
	int i=0, j=0;
	uint16_t rnd, rnd2;

	led_t cur[NUM_LEDS];
	led_t tar[NUM_LEDS];


	for( i=0; i<NUM_LEDS; i++ ) {
		rnd = get_random( );
		rnd2 = get_random( );

		tar[i].r = rnd&0x3F;
		tar[i].g = rnd>>10;
		tar[i].b = rnd2&0x3F;
	}


	for(j=0; j<FADE_STEPS; j++ ) {
		for(i=0; i<NUM_LEDS; i++ ) {
			if( cur[i].r > tar[i].r )
				cur[i].r -= (cur[i].r - tar[i].r) / FADE_STEPS;
			else
				cur[i].r += (tar[i].r - cur[i].r) / FADE_STEPS;

			if( cur[i].g > tar[i].g )
				cur[i].g -= (cur[i].g - tar[i].g) / FADE_STEPS;
			else
				cur[i].g += (tar[i].g - cur[i].g) / FADE_STEPS;

			if( cur[i].b > tar[i].b )
				cur[i].b  -= (cur[i].b - tar[i].b) / FADE_STEPS;
			else
				cur[i].b += (tar[i].b - cur[i].b) / FADE_STEPS;
		}
		// write to hardware
		for( i=0; i<NUM_LEDS; i++ ) {
			led_pushDataset( cur[i].r, cur[i].g, cur[i].b );
		}
		led_execute( );
	}
}
*/
