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
#ifndef __PATTERN_H__
#define __PATTERN_H__

void hsv_to_rgb( uint8_t h, uint8_t s, uint8_t v, uint8_t* red, uint8_t* green, uint8_t* blue );

void pattern_fading( void );
void pattern_fading_kitsch( void );
void pattern_bingo( void );
void pattern_hold( void );

#else
#error "double include pattern.h"
#endif
