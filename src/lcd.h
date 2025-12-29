<<<<<<< HEAD
/***********************************************************************************
 * 																				   *
 * 	 [MODULE]: LCD																   *
 * 																				   *
 * 	 [FILE NAME]: lcd.h			    											   *
 * 																				   *
 * 	 [AUTHOR]: Ahmed Abdelaal													   *
 * 																				   *
 * 	 [CREATED ON]: 05/09/2022													   *
 * 																				   *
 *   [DESCRIPTION]: Header file for the lcd driver						    	   *
 * 																				   *
 ***********************************************************************************/

#ifndef LCD_H_
#define LCD_H_

/*----------------------------------INCLUDES---------------------------------------*/

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/***********************************************************************************
 * 																				   *
 * 	                			PREPROCESSOR MACROS								   *
 * 																				   *
 ***********************************************************************************/

/* LCD MODE */
#define MODE 4

/* LCD HW Pins */
#define RS PB0
#define E  PB1
#if (MODE == 8)
#define D0  PD0
#define D1  PD1
#define D2  PD2
#define D3  PD3
#endif
#define D4  PD4
#define D5  PD5
#define D6  PD6
#define D7  PD7
#define LCD_CTRL_PORT PORTB
#define LCD_CTRL_PORT_DIR DDRB
#define LCD_DATA_PORT PORTD
#define LCD_DATA_PORT_DIR DDRD

/* LCD Commands */
#define CLEAR_COMMAND 0x01
#define FOUR_BITS_DATA_MODE 0x02
#define TWO_LINE_LCD_8_BIT_MODE 0x38
#define TWO_LINE_LCD_4_BIT_MODE 0x28
#define DISPLAY_OFF_CURSOR_OFF 0X08
#define DISPLAY_ON_CURSOR_OFF 0x0C
#define DISPLAY_ON_CURSOR_ON 0x0E
#define DISPLAY_ON_CURSOR_BLINK 0x0F
#define SET_CURSOR_LOCATION 0x80
#define SHIFT_CURSOR_RIGHT 0x06
#define SHIFT_CURSOR_LEFT 0x04
#define SHIFT_DISPLAY_RIGHT 0x05
#define SHIFT_DISPLAY_LEFT 0x07

#define DEL_KEY 0x7F
#define BACKSPACE_KEY 0x08


/***********************************************************************************
 * 																				   *
 * 	                			FUNCTION PROTOTYPES								   *
 * 																				   *
 ***********************************************************************************/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_init
 *
 * [FUNCTION DESCRIPTION]: Function responsible for initialize the lcd
 * 						   interface commands.
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_init(void);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_sendCommand
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send command (instruction)
 *                         to the lcd
 * [Params]: [1] uint8 command : command to be executed
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_sendCommand(uint8 command);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayCharacter
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send data
 *                         (character) to the lcd
 * [Params]: [1] uint8 data : character to be sent
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayCharacter(uint8 data);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayString
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send data
 *                         (string) to the lcd
 * [Params]: [1] char *Str : string to be sent
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayString(const char *Str);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_clearScreen
 *
 * [FUNCTION DESCRIPTION]: Function responsible for clear the lcd
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_clearScreen(void);

/*[6]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayStringRowColumn
 *
 * [FUNCTION DESCRIPTION]: Function responsible for display string at
 *                         position (row,col) in the lcd
 * [Params]: [1] uint8 row : index of row
 *           [2] uint8 col : index of column
 *           [3] char *Str : string to be sent
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayStringRowColumn(uint8 row, uint8 col, const char *Str);

/*[7]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_goToRowColumn
 *
 * [FUNCTION DESCRIPTION]: Function responsible for set cursor at
 *                         position (row,col) in the lcd
 * [Params]: [1] uint8 row : index of row
 *           [2] uint8 col : index of column
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_goToRowColumn(uint8 row, uint8 col);

/*[8]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayInteger
 *
 * [FUNCTION DESCRIPTION]: Function responsible for convert integer number to
 *                         its ascci value and send it to lcd to display
 * [Params]: [1] int data : integer data to be display
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayInteger(int data);

/*[9]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftDisplayRight
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting display to right
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftDisplayRight(void);

/*[10]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftDisplayLeft
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting display to left
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftDisplayLeft(void);

/*[11]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftCursorRight
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting cursor to right
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftCursorRight(void);

/*[12]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftCursorLeft
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting cursor to left
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftCursorLeft(void);

/*[13]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayOff
 *
 * [FUNCTION DESCRIPTION]: Function responsible for turn off display and cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayOff(void);

/*[14]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_cursorOff
 *
 * [FUNCTION DESCRIPTION]: Function responsible for turn off cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_cursorOff(void);

/*[15]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_cursorOn
 *
 * [FUNCTION DESCRIPTION]: Function responsible for turn on cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_cursorOn(void);

/*[16]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_cursorBlink
 *
 * [FUNCTION DESCRIPTION]: Function responsible for blink cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_cursorBlink(void);

/*[17]------------------------------------------------------------------------------
 * [FUNCTION NAME]: LCD_backspace
 *
 * [FUNCTION DESCRIPTION]: Function responsible for removing the last character
 *                         by moving cursor left, writing space, then moving left again
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_backspace(void);

/*[18]------------------------------------------------------------------------------
 * [FUNCTION NAME]: LCD_displayCharacterWithBackspace
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send data
 *                         (character) to the lcd but with backspace ability
 *
 * [Params]: [1] uint8 data : character to be sent
 *
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayCharacterWithBackspace(uint8 data);


#endif /* LCD_H_ */
=======
/***********************************************************************************
 * 																				   *
 * 	 [MODULE]: LCD																   *
 * 																				   *
 * 	 [FILE NAME]: lcd.h			    											   *
 * 																				   *
 * 	 [AUTHOR]: Ahmed Abdelaal													   *
 * 																				   *
 * 	 [CREATED ON]: 05/09/2022													   *
 * 																				   *
 *   [DESCRIPTION]: Header file for the lcd driver						    	   *
 * 																				   *
 ***********************************************************************************/

#ifndef LCD_H_
#define LCD_H_

/*----------------------------------INCLUDES---------------------------------------*/

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

/***********************************************************************************
 * 																				   *
 * 	                			PREPROCESSOR MACROS								   *
 * 																				   *
 ***********************************************************************************/

/* LCD MODE */
#define MODE 4

/* LCD HW Pins */
#define RS PB0
#define E  PB1
#if (MODE == 8)
#define D0  PD0
#define D1  PD1
#define D2  PD2
#define D3  PD3
#endif
#define D4  PD4
#define D5  PD5
#define D6  PD6
#define D7  PD7
#define LCD_CTRL_PORT PORTB
#define LCD_CTRL_PORT_DIR DDRB
#define LCD_DATA_PORT PORTD
#define LCD_DATA_PORT_DIR DDRD

/* LCD Commands */
#define CLEAR_COMMAND 0x01
#define FOUR_BITS_DATA_MODE 0x02
#define TWO_LINE_LCD_8_BIT_MODE 0x38
#define TWO_LINE_LCD_4_BIT_MODE 0x28
#define DISPLAY_OFF_CURSOR_OFF 0X08
#define DISPLAY_ON_CURSOR_OFF 0x0C
#define DISPLAY_ON_CURSOR_ON 0x0E
#define DISPLAY_ON_CURSOR_BLINK 0x0F
#define SET_CURSOR_LOCATION 0x80
#define SHIFT_CURSOR_RIGHT 0x06
#define SHIFT_CURSOR_LEFT 0x04
#define SHIFT_DISPLAY_RIGHT 0x05
#define SHIFT_DISPLAY_LEFT 0x07

#define DEL_KEY 0x7F
#define BACKSPACE_KEY 0x08


/***********************************************************************************
 * 																				   *
 * 	                			FUNCTION PROTOTYPES								   *
 * 																				   *
 ***********************************************************************************/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_init
 *
 * [FUNCTION DESCRIPTION]: Function responsible for initialize the lcd
 * 						   interface commands.
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_init(void);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_sendCommand
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send command (instruction)
 *                         to the lcd
 * [Params]: [1] uint8 command : command to be executed
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_sendCommand(uint8 command);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayCharacter
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send data
 *                         (character) to the lcd
 * [Params]: [1] uint8 data : character to be sent
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayCharacter(uint8 data);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayString
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send data
 *                         (string) to the lcd
 * [Params]: [1] char *Str : string to be sent
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayString(const char *Str);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_clearScreen
 *
 * [FUNCTION DESCRIPTION]: Function responsible for clear the lcd
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_clearScreen(void);

/*[6]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayStringRowColumn
 *
 * [FUNCTION DESCRIPTION]: Function responsible for display string at
 *                         position (row,col) in the lcd
 * [Params]: [1] uint8 row : index of row
 *           [2] uint8 col : index of column
 *           [3] char *Str : string to be sent
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayStringRowColumn(uint8 row, uint8 col, const char *Str);

/*[7]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_goToRowColumn
 *
 * [FUNCTION DESCRIPTION]: Function responsible for set cursor at
 *                         position (row,col) in the lcd
 * [Params]: [1] uint8 row : index of row
 *           [2] uint8 col : index of column
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_goToRowColumn(uint8 row, uint8 col);

/*[8]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayInteger
 *
 * [FUNCTION DESCRIPTION]: Function responsible for convert integer number to
 *                         its ascci value and send it to lcd to display
 * [Params]: [1] int data : integer data to be display
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayInteger(int data);

/*[9]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftDisplayRight
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting display to right
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftDisplayRight(void);

/*[10]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftDisplayLeft
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting display to left
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftDisplayLeft(void);

/*[11]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftCursorRight
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting cursor to right
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftCursorRight(void);

/*[12]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_shiftCursorLeft
 *
 * [FUNCTION DESCRIPTION]: Function responsible for shifting cursor to left
 *                         by 1 digit
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_shiftCursorLeft(void);

/*[13]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_displayOff
 *
 * [FUNCTION DESCRIPTION]: Function responsible for turn off display and cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayOff(void);

/*[14]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_cursorOff
 *
 * [FUNCTION DESCRIPTION]: Function responsible for turn off cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_cursorOff(void);

/*[15]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_cursorOn
 *
 * [FUNCTION DESCRIPTION]: Function responsible for turn on cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_cursorOn(void);

/*[16]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: LCD_cursorBlink
 *
 * [FUNCTION DESCRIPTION]: Function responsible for blink cursor
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_cursorBlink(void);

/*[17]------------------------------------------------------------------------------
 * [FUNCTION NAME]: LCD_backspace
 *
 * [FUNCTION DESCRIPTION]: Function responsible for removing the last character
 *                         by moving cursor left, writing space, then moving left again
 *
 * [Params]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_backspace(void);

/*[18]------------------------------------------------------------------------------
 * [FUNCTION NAME]: LCD_displayCharacterWithBackspace
 *
 * [FUNCTION DESCRIPTION]: Function responsible for send data
 *                         (character) to the lcd but with backspace ability
 *
 * [Params]: [1] uint8 data : character to be sent
 *
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/

void LCD_displayCharacterWithBackspace(uint8 data);


#endif /* LCD_H_ */
>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
