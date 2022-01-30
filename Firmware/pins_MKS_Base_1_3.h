/*****************************************************************
* Rambo mini 1.0 Pin Assignments
******************************************************************/

#define ELECTRONICS "MKSBase13"

#define KNOWN_BOARD
#ifndef __AVR_ATmega2560__
  #error Oops!  Make sure you have 'Arduino Mega 2560 or Rambo' selected from the 'Tools -> Boards' menu.
#endif

#define PINDA_THERMISTOR

// Software I2C used for MK3 or MK3S printers with IR_SENSOR undefined !
#define SWI2C_SDA      20 //SDA on P3
#define SWI2C_SCL      21 //PH2 on P3, sensor cable must be rewired

// Servo pins, not used in Prusa
#define SERVO0         11 // PB5, OC1A, PCINT5
#define SERVO1         12 // PB6, OC1B, PCINT6

#define X_STEP_PIN             54
#define X_DIR_PIN              55
#define X_MIN_PIN              3
#define X_MAX_PIN              2
#define X_ENABLE_PIN           38
#define X_MS1_PIN              5 //PE3
#define X_MS2_PIN              6 //PH3

#define Y_STEP_PIN             60
#define Y_DIR_PIN              61
#define Y_MIN_PIN              14
#define Y_MAX_PIN              15
#define Y_ENABLE_PIN           56
#define Y_MS1_PIN              59 // analog 5, PF5
#define Y_MS2_PIN              58 // analog 4, PF4

#define Z_STEP_PIN             46
#define Z_DIR_PIN              48
#define Z_MIN_PIN              18
#define Z_MAX_PIN              19
#define Z_ENABLE_PIN           62
#define Z_MS1_PIN              22
#define Z_MS2_PIN              39

#define HEATER_BED_PIN          8  // PH5, OC4C (Timer4)
#define TEMP_BED_PIN            14 // A14, 68, PK6

#define HEATER_0_PIN            10 // PB4, OC2A (Timer2)
#define TEMP_0_PIN              13 // A13, 67, PK5

#define HEATER_1_PIN           -1
#define TEMP_1_PIN             12

#define HEATER_2_PIN           -1
#define TEMP_2_PIN             -1

#define TEMP_AMBIENT_PIN        12

#define TEMP_PINDA_PIN          15 // A15, 69, PK7

#define E0_STEP_PIN            26
#define E0_DIR_PIN             28
#define E0_ENABLE_PIN          24
#define E0_MS1_PIN             63
#define E0_MS2_PIN             64

#define E1_STEP_PIN            36
#define E1_DIR_PIN             34
#define E1_ENABLE_PIN          30
#define E1_MS1_PIN             57
#define E1_MS2_PIN             4

#ifdef SNMM 
  #define E_MUX0_PIN 40
  #define E_MUX1_PIN 42
#endif


// Motor Current Digipot set NOT supported
#if 0
#define MOTOR_CURRENT_PWM_XY_PIN -1
#define MOTOR_CURRENT_PWM_Z_PIN  -1
#define MOTOR_CURRENT_PWM_E_PIN  -1
#endif

#define SDPOWER             -1
#define SDSS                53
#define LED_PIN             13
#define FAN_PIN              9
#define FAN_1_PIN           -1
#define PS_ON_PIN           -1
#define KILL_PIN            41
#define SUICIDE_PIN         -1  // PIN that has to be turned on right after start, to keep power flowing.
#define TACH_0				-1	// noctua extruder fan

#define BEEPER              37  // Beeper on AUX-4
#define LCD_PINS_RS         16
#define LCD_PINS_ENABLE     17
#define LCD_PINS_D4	        23
#define LCD_PINS_D5         25
#define LCD_PINS_D6         27
#define LCD_PINS_D7         29

//buttons are directly attached using AUX-2
#define BTN_EN1                31
#define BTN_EN2                33
#define BTN_ENC                35  // the click

#define SDCARDDETECT           49

#define IR_SENSOR_PIN X_MAX_PIN //idler sensor 

#ifdef LA_DEBUG_LOGIC
#error "LA_DEBUG_LOGIC NOT supported on MKS Base board !"
#endif

#if 0
// Support for an 8 bit logic analyzer, for example the Saleae.
// Channels 0-2 are fast, they could generate 2.667Mhz waveform with a software loop.
#define LOGIC_ANALYZER_CH0		X_MIN_PIN		// PB6
#define LOGIC_ANALYZER_CH1		Y_MIN_PIN		// PB5
#define LOGIC_ANALYZER_CH2		53				// PB0 (PROC_nCS)
// Channels 3-7 are slow, they could generate
// 0.889Mhz waveform with a software loop and interrupt locking,
// 1.333MHz waveform without interrupt locking.
#define LOGIC_ANALYZER_CH3 		73				// PJ3
// PK0 has no Arduino digital pin assigned, so we set it directly.
#define WRITE_LOGIC_ANALYZER_CH4(value) if (value) PORTK |= (1 << 0); else PORTK &= ~(1 << 0) // PK0
#define LOGIC_ANALYZER_CH5		16				// PH0 (RXD2)
#define LOGIC_ANALYZER_CH6		17				// PH1 (TXD2)
#define LOGIC_ANALYZER_CH7 		76				// PJ5

#define LOGIC_ANALYZER_CH0_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH0)
#define LOGIC_ANALYZER_CH1_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH1)
#define LOGIC_ANALYZER_CH2_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH2)
#define LOGIC_ANALYZER_CH3_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH3)
#define LOGIC_ANALYZER_CH4_ENABLE do { DDRK |= 1 << 0; } while (0)
#define LOGIC_ANALYZER_CH5_ENABLE do { cbi(UCSR2B, TXEN2); cbi(UCSR2B, RXEN2); cbi(UCSR2B, RXCIE2); SET_OUTPUT(LOGIC_ANALYZER_CH5); } while (0)
#define LOGIC_ANALYZER_CH6_ENABLE do { cbi(UCSR2B, TXEN2); cbi(UCSR2B, RXEN2); cbi(UCSR2B, RXCIE2); SET_OUTPUT(LOGIC_ANALYZER_CH6); } while (0)
#define LOGIC_ANALYZER_CH7_ENABLE SET_OUTPUT(LOGIC_ANALYZER_CH7)

#endif
