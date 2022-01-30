//tone04.c
// use atmega timer4 as main tone timer instead of timer2
// timer2 is used for System timer.

#include "system_timer.h"
#include "Configuration_prusa.h"

#ifdef SYSTEM_TIMER_2

#include "pins.h"
#include "fastio.h"
#include "macros.h"

#include "boards.h"
#if (MOTHERBOARD == BOARD_MKS_BASE_1_3)
#define TONE_TIMER3
#endif

void timer4_init(void)
{
	CRITICAL_SECTION_START;
	
	SET_OUTPUT(BEEPER);
	WRITE(BEEPER, LOW);
	
	SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);

#ifdef TONE_TIMER3
	// this board uses OC4C for PWM output
		
	// Set timer mode 9 (PWM,Phase and Frequency Correct)
	// Prescaler is CLK/1024
	// Output compare is disabled on all timer pins
	// Input capture is disabled
	// All interrupts are disabled
	TCCR3A = (1 << WGM30);
	TCCR3B = (1 << WGM33) | (1 << CS32) | (1 << CS30);
	OCR3A = 255;
	OCR3B = 255;
	OCR3C = 255;
	TIMSK3 = 0;
#else	
	// Set timer mode 9 (PWM,Phase and Frequency Correct)
	// Prescaler is CLK/1024
	// Output compare is disabled on all timer pins
	// Input capture is disabled
	// All interrupts are disabled
	TCCR4A = (1 << WGM40);
	TCCR4B = (1 << WGM43) | (1 << CS42) | (1 << CS40);
	OCR4A = 255;
	OCR4B = 255;
	OCR4C = 255;
	TIMSK4 = 0;
#endif
	
	CRITICAL_SECTION_END;
}

void timer4_set_fan0(uint8_t duty)
{
#ifdef EXTRUDER_0_AUTO_FAN_PIN
	if (duty == 0 || duty == 255)
	{
		CRITICAL_SECTION_START;
#ifdef TONE_TIMER3
		// We use digital logic if the duty cycle is 0% or 100%
		TIMSK3 &= ~(1 << OCIE3C);
		OCR3C = 0;
#else
		// We use digital logic if the duty cycle is 0% or 100%
		TCCR4A &= ~(1 << COM4C1);
		OCR4C = 0;
#endif
		CRITICAL_SECTION_END;
		WRITE(EXTRUDER_0_AUTO_FAN_PIN, duty);
	}
	else
	{
		// Use the timer for fan speed. Enable the timer compare output and set the duty cycle.
		// This function also handles the impossible scenario of a fan speed change during a Tone.
		// Better be safe than sorry.
		CRITICAL_SECTION_START;
#ifdef TONE_TIMER3
		// Enable the PWM output on the fan pin.
		TIMSK3 |= (1 << OCIE3C);
		OCR3C = (((uint32_t)duty) * ((uint32_t)((TIMSK3 & (1 << OCIE3A))?OCR3A:255))) / ((uint32_t)255);
#else
		// Enable the PWM output on the fan pin.
		TCCR4A |= (1 << COM4C1);
		OCR4C = (((uint32_t)duty) * ((uint32_t)((TIMSK4 & (1 << OCIE4A))?OCR4A:255))) / ((uint32_t)255);
#endif
		CRITICAL_SECTION_END;
	}
#endif //EXTRUDER_0_AUTO_FAN_PIN
}

// Because of the timer mode change, we need two interrupts. We could also try to assume that the frequency is x2
// and use a TOGGLE(), but this seems to work well enough so I left it as it is now.
#ifdef TONE_TIMER3
ISR(TIMER3_COMPA_vect)
#else
ISR(TIMER4_COMPA_vect)
#endif
{
	WRITE(BEEPER, 1);
}

#ifdef TONE_TIMER3
ISR(TIMER3_COMPC_vect)
{
	WRITE(EXTRUDER_0_AUTO_FAN_PIN, 1);
}

ISR(TIMER3_OVF_vect)
{
	WRITE(BEEPER, 0);
	if(TIMSK3 & (1 << OCIE3C))
	{
		WRITE(EXTRUDER_0_AUTO_FAN_PIN, 0);
	}
}
#else
ISR(TIMER4_OVF_vect)
{
	WRITE(BEEPER, 0);
}
#endif

void tone4(_UNUSED uint8_t _pin, uint16_t frequency)
{
	//this ocr and prescalarbits calculation is taken from the Arduino core and simplified for one type of timer only
	uint8_t prescalarbits = 0b001;
	uint32_t ocr = F_CPU / frequency / 2 - 1;
	
	if (ocr > 0xffff)
	{
		ocr = F_CPU / frequency / 2 / 64 - 1;
		prescalarbits = 0b011;
	}
	
	CRITICAL_SECTION_START;
#ifdef TONE_TIMER3
	// Set calcualted prescaler
	TCCR3B = (TCCR3B & 0b11111000) | prescalarbits;
#ifdef EXTRUDER_0_AUTO_FAN_PIN
	// Scale the fan PWM duty cycle so that it remains constant, but at the tone frequency
	OCR3C = (((uint32_t)OCR3C) * ocr) / (uint32_t)((TIMSK3 & (1 << OCIE3A))?OCR3A:255);
#endif //EXTRUDER_0_AUTO_FAN_PIN
	// Set calcualted ocr
	OCR3A = ocr;
	// Enable Output compare A interrupt and timer overflow interrupt
	TIMSK3 |= (1 << OCIE3A) | (1 << TOIE3);
#else
	// Set calcualted prescaler
	TCCR4B = (TCCR4B & 0b11111000) | prescalarbits;
#ifdef EXTRUDER_0_AUTO_FAN_PIN
	// Scale the fan PWM duty cycle so that it remains constant, but at the tone frequency
	OCR4C = (((uint32_t)OCR4C) * ocr) / (uint32_t)((TIMSK4 & (1 << OCIE4A))?OCR4A:255);
#endif //EXTRUDER_0_AUTO_FAN_PIN
	// Set calcualted ocr
	OCR4A = ocr;
	// Enable Output compare A interrupt and timer overflow interrupt
	TIMSK4 |= (1 << OCIE4A) | (1 << TOIE4);
#endif
	CRITICAL_SECTION_END;
}

void noTone4(_UNUSED uint8_t _pin)
{
	CRITICAL_SECTION_START;
#ifdef TONE_TIMER3
	// Revert prescaler to CLK/1024
	TCCR3B = (TCCR3B & 0b11111000) | (1 << CS32) | (1 << CS30);
#ifdef EXTRUDER_0_AUTO_FAN_PIN
	// Scale the fan OCR back to the original value.
	OCR3C = (((uint32_t)OCR3C) * (uint32_t)255) / (uint32_t)((TIMSK3 & (1 << OCIE3A))?OCR3A:255);
#endif //EXTRUDER_0_AUTO_FAN_PIN
	OCR3A = 255;
	// Disable Output compare A interrupt and timer overflow interrupt
	TIMSK3 &= ~((1 << OCIE3A) | (1 << TOIE3));
#else
	// Revert prescaler to CLK/1024
	TCCR4B = (TCCR4B & 0b11111000) | (1 << CS42) | (1 << CS40);
#ifdef EXTRUDER_0_AUTO_FAN_PIN
	// Scale the fan OCR back to the original value.
	OCR4C = (((uint32_t)OCR4C) * (uint32_t)255) / (uint32_t)((TIMSK4 & (1 << OCIE4A))?OCR4A:255);
#endif //EXTRUDER_0_AUTO_FAN_PIN
	OCR4A = 255;
	// Disable Output compare A interrupt and timer overflow interrupt
	TIMSK4 &= ~((1 << OCIE4A) | (1 << TOIE4));
#endif
	CRITICAL_SECTION_END;
	// Turn beeper off if it was on when noTone was called
	WRITE(BEEPER, 0);
}


#endif //SYSTEM_TIMER_2
