//! @file

#ifndef FIRMWARE_SYSTEM_TIMER_H_
#define FIRMWARE_SYSTEM_TIMER_H_

#include "Arduino.h"

/* 
Rambo boards:
- HEATER_BED -> PG5 -> OC0B -> Timer0
- HEATER_END -> PE5 -> OC3C -> Timer3
- AUTO_FAN   -> PH5 -> OC4C -> Timer4
- FAN        -> PH3 -> OC4A -> Timer4

MKS Base board:
- HEATER_BED -> PH5 -> OC4C -> Timer4
- HEATER_END -> PB4 -> OC2A -> Timer2
- AUTO_FAN   -> PH4 -> OC4B -> Timer4
- FAN        -> PH6 -> OC2B -> Timer2

+----------------------+--------+----------+
| Timer usage          | Rambo  | MKS Base |
+----------------------+--------+----------+
| Bed PWM              | timer0 | timer4   |
+----------------------+--------+----------+
| millis, micro, delay | timer2 | timer2   |
+----------------------+--------+----------+
| tone, auto fan       | timer4 | timer3   |
+----------------------+--------+----------+
*/

#define SYSTEM_TIMER_2

#ifdef SYSTEM_TIMER_2
#include "timer02.h"
#include "tone04.h"
#define _millis millis2
#define _micros micros2
#define _delay delay2
#define _tone tone4
#define _noTone noTone4

#define timer02_set_pwm0(pwm0)

#else //SYSTEM_TIMER_2
#define _millis millis
#define _micros micros
#define _delay delay
#define _tone tone
#define _noTone noTone
#define timer02_set_pwm0(pwm0)
#endif //SYSTEM_TIMER_2

#endif /* FIRMWARE_SYSTEM_TIMER_H_ */
