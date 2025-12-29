<<<<<<< HEAD
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: KEYPAD                                                                *
 *                                                                                 *
 * [FILE NAME]: KEYPAD.h                                                           *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 25/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Header file for 4x4 matrix keypad driver for ATmega328P        *
 *                Rows connected to PORTC (PC0-PC3)                               *
 *                Columns connected to PORTD (PD0-PD3)                            *
 *                                                                                 *
 ***********************************************************************************/

#ifndef KEYPAD_H_
#define KEYPAD_H_

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/*---------------------------------------------------------------------------------*
 *                              GLOBAL CONST MACROS                                *
 *---------------------------------------------------------------------------------*/

/* Keypad Dimensions */
#define KEYPAD_NUM_ROWS             4
#define KEYPAD_NUM_COLS             4

/* Keypad Row Port Configuration - PORTC */
#define KEYPAD_ROW_PORT_DIR         DDRC
#define KEYPAD_ROW_PORT_OUT         PORTC
#define KEYPAD_ROW_PORT_IN          PINC

#define KEYPAD_ROW0_PIN             PC0
#define KEYPAD_ROW1_PIN             PC1
#define KEYPAD_ROW2_PIN             PC2
#define KEYPAD_ROW3_PIN             PC3

/* Keypad Column Port Configuration - PORTD */
#define KEYPAD_COL_PORT_DIR         DDRD
#define KEYPAD_COL_PORT_OUT         PORTD
#define KEYPAD_COL_PORT_IN          PIND

#define KEYPAD_COL0_PIN             PD0
#define KEYPAD_COL1_PIN             PD1
#define KEYPAD_COL2_PIN             PD2
#define KEYPAD_COL3_PIN             PD3

/* Keypad Button Values (ASCII) */
#define KEYPAD_NOT_PRESSED          0xFF

/* Standard 4x4 Keypad Layout:
 *     COL0  COL1  COL2  COL3
 * ROW0  1     2     3     A
 * ROW1  4     5     6     B
 * ROW2  7     8     9     C
 * ROW3  *     0     #     D
 */

/* Debouncing Configuration */
#define KEYPAD_DEBOUNCE_TIME_MS     20      /* Debounce delay in milliseconds */

/* Special Key Definitions (for easy identification) */
#define KEYPAD_KEY_1                '1'
#define KEYPAD_KEY_2                '2'
#define KEYPAD_KEY_3                '3'
#define KEYPAD_KEY_A                'A'
#define KEYPAD_KEY_4                '4'
#define KEYPAD_KEY_5                '5'
#define KEYPAD_KEY_6                '6'
#define KEYPAD_KEY_B                'B'
#define KEYPAD_KEY_7                '7'
#define KEYPAD_KEY_8                '8'
#define KEYPAD_KEY_9                '9'
#define KEYPAD_KEY_C                'C'
#define KEYPAD_KEY_STAR             '*'
#define KEYPAD_KEY_0                '0'
#define KEYPAD_KEY_HASH             '#'
#define KEYPAD_KEY_D                'D'

/*---------------------------------------------------------------------------------*
 *                                     ENUMS                                       *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Enumeration for keypad modes
 *
 * KEYPAD_MODE_BLOCKING     : Wait until key is pressed (blocking)
 * KEYPAD_MODE_NON_BLOCKING : Return immediately if no key pressed
 */
typedef enum
{
    KEYPAD_MODE_BLOCKING = 0,
    KEYPAD_MODE_NON_BLOCKING
} KEYPAD_Mode_t;

/*---------------------------------------------------------------------------------*
 *                              FUNCTION PROTOTYPES                                *
 *---------------------------------------------------------------------------------*/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_init
 *
 * [FUNCTION DESCRIPTION]: Initialize keypad hardware
 *                         Configures rows as output and columns as input
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/
void KEYPAD_init(void);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_getPressedKey
 *
 * [FUNCTION DESCRIPTION]: Get the pressed key value (blocking mode)
 *                         Waits until a key is pressed
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - ASCII value of pressed key
 *
 *---------------------------------------------------------------------------------*/
uint8 KEYPAD_getPressedKey(void);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_getPressedKeyNonBlocking
 *
 * [FUNCTION DESCRIPTION]: Get pressed key value (non-blocking mode)
 *                         Returns immediately with key or NOT_PRESSED
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - ASCII value of pressed key or KEYPAD_NOT_PRESSED
 *
 *---------------------------------------------------------------------------------*/
uint8 KEYPAD_getPressedKeyNonBlocking(void);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_isKeyPressed
 *
 * [FUNCTION DESCRIPTION]: Check if any key is currently pressed
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - 1 if key pressed, 0 if not
 *
 *---------------------------------------------------------------------------------*/
uint8 KEYPAD_isKeyPressed(void);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_waitForRelease
 *
 * [FUNCTION DESCRIPTION]: Wait until all keys are released
 *                         Used to prevent key repeat
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/
void KEYPAD_waitForRelease(void);

#endif /* KEYPAD_H_ */
=======
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: KEYPAD                                                                *
 *                                                                                 *
 * [FILE NAME]: KEYPAD.h                                                           *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 25/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Header file for 4x4 matrix keypad driver for ATmega328P        *
 *                Rows connected to PORTC (PC0-PC3)                               *
 *                Columns connected to PORTD (PD0-PD3)                            *
 *                                                                                 *
 ***********************************************************************************/

#ifndef KEYPAD_H_
#define KEYPAD_H_

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/*---------------------------------------------------------------------------------*
 *                              GLOBAL CONST MACROS                                *
 *---------------------------------------------------------------------------------*/

/* Keypad Dimensions */
#define KEYPAD_NUM_ROWS             4
#define KEYPAD_NUM_COLS             4

/* Keypad Row Port Configuration - PORTC */
#define KEYPAD_ROW_PORT_DIR         DDRC
#define KEYPAD_ROW_PORT_OUT         PORTC
#define KEYPAD_ROW_PORT_IN          PINC

#define KEYPAD_ROW0_PIN             PC0
#define KEYPAD_ROW1_PIN             PC1
#define KEYPAD_ROW2_PIN             PC2
#define KEYPAD_ROW3_PIN             PC3

/* Keypad Column Port Configuration - PORTD */
#define KEYPAD_COL_PORT_DIR         DDRD
#define KEYPAD_COL_PORT_OUT         PORTD
#define KEYPAD_COL_PORT_IN          PIND

#define KEYPAD_COL0_PIN             PD0
#define KEYPAD_COL1_PIN             PD1
#define KEYPAD_COL2_PIN             PD2
#define KEYPAD_COL3_PIN             PD3

/* Keypad Button Values (ASCII) */
#define KEYPAD_NOT_PRESSED          0xFF

/* Standard 4x4 Keypad Layout:
 *     COL0  COL1  COL2  COL3
 * ROW0  1     2     3     A
 * ROW1  4     5     6     B
 * ROW2  7     8     9     C
 * ROW3  *     0     #     D
 */

/* Debouncing Configuration */
#define KEYPAD_DEBOUNCE_TIME_MS     20      /* Debounce delay in milliseconds */

/* Special Key Definitions (for easy identification) */
#define KEYPAD_KEY_1                '1'
#define KEYPAD_KEY_2                '2'
#define KEYPAD_KEY_3                '3'
#define KEYPAD_KEY_A                'A'
#define KEYPAD_KEY_4                '4'
#define KEYPAD_KEY_5                '5'
#define KEYPAD_KEY_6                '6'
#define KEYPAD_KEY_B                'B'
#define KEYPAD_KEY_7                '7'
#define KEYPAD_KEY_8                '8'
#define KEYPAD_KEY_9                '9'
#define KEYPAD_KEY_C                'C'
#define KEYPAD_KEY_STAR             '*'
#define KEYPAD_KEY_0                '0'
#define KEYPAD_KEY_HASH             '#'
#define KEYPAD_KEY_D                'D'

/*---------------------------------------------------------------------------------*
 *                                     ENUMS                                       *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Enumeration for keypad modes
 *
 * KEYPAD_MODE_BLOCKING     : Wait until key is pressed (blocking)
 * KEYPAD_MODE_NON_BLOCKING : Return immediately if no key pressed
 */
typedef enum
{
    KEYPAD_MODE_BLOCKING = 0,
    KEYPAD_MODE_NON_BLOCKING
} KEYPAD_Mode_t;

/*---------------------------------------------------------------------------------*
 *                              FUNCTION PROTOTYPES                                *
 *---------------------------------------------------------------------------------*/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_init
 *
 * [FUNCTION DESCRIPTION]: Initialize keypad hardware
 *                         Configures rows as output and columns as input
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/
void KEYPAD_init(void);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_getPressedKey
 *
 * [FUNCTION DESCRIPTION]: Get the pressed key value (blocking mode)
 *                         Waits until a key is pressed
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - ASCII value of pressed key
 *
 *---------------------------------------------------------------------------------*/
uint8 KEYPAD_getPressedKey(void);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_getPressedKeyNonBlocking
 *
 * [FUNCTION DESCRIPTION]: Get pressed key value (non-blocking mode)
 *                         Returns immediately with key or NOT_PRESSED
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - ASCII value of pressed key or KEYPAD_NOT_PRESSED
 *
 *---------------------------------------------------------------------------------*/
uint8 KEYPAD_getPressedKeyNonBlocking(void);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_isKeyPressed
 *
 * [FUNCTION DESCRIPTION]: Check if any key is currently pressed
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - 1 if key pressed, 0 if not
 *
 *---------------------------------------------------------------------------------*/
uint8 KEYPAD_isKeyPressed(void);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: KEYPAD_waitForRelease
 *
 * [FUNCTION DESCRIPTION]: Wait until all keys are released
 *                         Used to prevent key repeat
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/
void KEYPAD_waitForRelease(void);

#endif /* KEYPAD_H_ */
>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
