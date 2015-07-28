#ifndef __LOCAL_H_
#define __LOCAL_H_

#define NUM_LEDS 40

#define HEARTBEAT_MSG        0x00010100
#define DISP_CONTENT_MSG     0x00010101 /* to 0x0001010F */

#define SWITCH_MODE_MSG      0x00010111 // [mode]
#define SET_COLOR_MSG        0x00010112 // [R][G][B]
#define SET_LED_MSG          0x00010113 // [R][G][B][ID]
#define REQ_DISP_CONTENT_MSG 0x00010120 // [RATE(fps)]
#define SET_SLEEPTIME        0x00010130 // Remaining Time to stay awake in s
#define RESET_MSG            0x000101FF


#endif // __LOCAL_H_
