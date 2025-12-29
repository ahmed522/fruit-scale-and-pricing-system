/******************************************************************************
 * HX711 library for AVR ATmega328P
 * With optional Proteus / software simulation support
 * Converted and extended for Fruit Weighing System project
 *
 * Original Arduino C++ library:
 *   https://github.com/bogde/HX711
 *
 * MIT License - (c) 2018 Bogdan Necula
 * C conversion & extensions: Ahmed Abdelaal - 2025
 *
 ******************************************************************************/

#ifndef HX711_H_
#define HX711_H_

/*---------------------------------------------------------------------------------
 * INCLUDES
 *--------------------------------------------------------------------------------*/
#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/*---------------------------------------------------------------------------------
 * PIN CONFIGURATION (REAL HARDWARE)
 *--------------------------------------------------------------------------------*/
/* DOUT (data) on PB4 (Arduino D12) */
#define HX711_DOUT_PORT      PORTB
#define HX711_DOUT_DDR       DDRB
#define HX711_DOUT_PIN       PINB
#define HX711_DOUT_PINNUM    PB4

/* SCK (clock) on PC5 (Arduino A5) */
#define HX711_SCK_PORT       PORTC
#define HX711_SCK_DDR        DDRC
#define HX711_SCK_PIN        PINC
#define HX711_SCK_PINNUM     PC5

/*---------------------------------------------------------------------------------
 * GAIN/CHANNEL DEFINITIONS
 *--------------------------------------------------------------------------------*/
#define HX711_GAINCHANNELA128   128  /* Channel A, gain 128  */
#define HX711_GAINCHANNELA64    64   /* Channel A, gain 64   */
#define HX711_GAINCHANNELB32    32   /* Channel B, gain 32   */

/* Legacy compatibility */
#define HX711_GAIN_128          HX711_GAINCHANNELA128
#define HX711_GAIN_64           HX711_GAINCHANNELA64
#define HX711_GAIN_32           HX711_GAINCHANNELB32

/*---------------------------------------------------------------------------------
 * SIMULATION CONFIGURATION
 *--------------------------------------------------------------------------------*/
/*
 * Enable or disable built-in simulation.
 * When enabled, hx711_read() will generate a synthetic load pattern:
 *   - ~1.0 kg (± 0.001-0.010 kg noise) for 2000 ms
 *   - 2.0 kg for 1000 ms
 *   - back to ~1.0 kg (stable)
 */
#define HX711_SIMULATION_ENABLED_DEFAULT   0  /* 0 = off, 1 = on */

/*---------------------------------------------------------------------------------
 * PUBLIC API
 *--------------------------------------------------------------------------------*/

/* Initialization */
void   hx711_init(uint8 gain, double scale, sint32 offset);

/* Ready check */
uint8  hx711_isready(void);

/* Raw read & averaging */
float32 hx711_read(void);
float32 hx711_readaverage(uint8 times);

/* Calibrated weight (kg) */
double hx711_getweight(void);

/* Calibration helpers */
void   hx711_calibrate1setoffset(void);
void   hx711_calibrate2setscale(double knownWeight);
void   hx711_taretozero(void);

/* Scale/offset getters & setters */
void   hx711_setscale(double scale);
double hx711_getscale(void);

void   hx711_setoffset(sint32 offset);
sint32 hx711_getoffset(void);

uint8  hx711_getgain(void);

/* Power management */
void   hx711_powerdown(void);
void   hx711_powerup(void);

/* Simulation control (optional) */
void   hx711_enableSimulation(void);
void   hx711_disableSimulation(void);
void   hx711_startSimulationPattern(void);
void   hx711_stopSimulationPattern(void);

#endif /* HX711_H_ */
