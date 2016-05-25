#ifndef __LOCAL_H_
#define __LOCAL_H_

#define NUM_LEDS 40

#define HEARTBEAT_MSG        0x00101000
#define DISP_CONTENT_MSG     0x00101010 /* to 0x0001010F */

#define SWITCH_MODE_MSG      0x00101011 // [mode]
#define SET_COLOR_MSG        0x00101012 // [R][G][B]
#define SET_LED_MSG          0x00101013 // [R][G][B][ID]
#define REQ_DISP_CONTENT_MSG 0x00101020 // [RATE(fps)]
#define SET_SLEEPTIME        0x00101030 // Remaining Time to stay awake in seconds
#define RESET_MSG            0x00101FFF


#endif // __LOCAL_H_
