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
#include <stdint.h>



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
