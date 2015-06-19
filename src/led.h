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

void led_init( void );
void led_clear( void );

void led_set( uint8_t ledId, uint8_t r, uint8_t g, uint8_t b );
void led_run( void );
void led_pushDataset( uint8_t r, uint8_t g, uint8_t b );
void led_flush( void );
void led_sendByte( uint8_t data );
uint16_t gamma16( uint8_t input );



typedef struct led_s {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} led_t;
