/*
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Martin Wenger <martin.wenger@arcormail.de> and Stefan Rupp <struppi@erlangen.ccc.de>
 * wrote this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me/us a beer in return.
 * (c) 2005-2007 Martin Wenger, Stefan Rupp
 */
#ifndef __TIMER_H__
#define __TIMER_H__

void timer_waitMs( uint32_t delay );
uint32_t timer_getMs( void );
void timer_init( void );

#else
#error "double include timer.h"
#endif //__TIMER_H__
