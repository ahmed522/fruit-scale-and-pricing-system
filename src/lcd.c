<<<<<<< HEAD
/***********************************************************************************
 * 																				   *
 * 	 [MODULE]: LCD																   *
 * 																				   *
 * 	 [FILE NAME]: lcd.c			    											   *
 * 																				   *
 * 	 [AUTHOR]: Ahmed Abdelaal													   *
 * 																				   *
 * 	 [CREATED ON]: 05/09/2022													   *
 * 																				   *
 *   [DESCRIPTION]: source file for the lcd driver						    	   *
 * 																				   *
 ***********************************************************************************/

#include "lcd.h"

/***********************************************************************************
 * 																				   *
 *              	          Functions Definitions                                *
 * 																				   *
 ***********************************************************************************/

void LCD_init(void)
{
	LCD_CTRL_PORT_DIR |= (1 << E) | (1 << RS); /* Configure the control pins(E,RS,RW) as output pins */

#if (MODE == 4)
	LCD_DATA_PORT_DIR |= (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7); /* Configure the data port as output port */
	LCD_sendCommand(FOUR_BITS_DATA_MODE); /* initialize LCD in 4-bit mode */
	LCD_sendCommand(TWO_LINE_LCD_4_BIT_MODE); /* initialize LCD in 4-bit mode */
#elif (MODE == 8)
	LCD_DATA_PORT_DIR = 0xFF; /* Configure the data port as output port */
	LCD_sendCommand(TWO_LINE_LCD_8_BIT_MODE); /* use 2-line lcd + 8-bit Data Mode + 5*7 dot display Mode */
#endif

	LCD_sendCommand(DISPLAY_ON_CURSOR_OFF); /* cursor off */
	LCD_sendCommand(CLEAR_COMMAND); /* clear LCD at the beginning */
}

/*---------------------------------------------------------------------------------*/

void LCD_sendCommand(uint8 command)
{
	CLEAR_BIT(LCD_CTRL_PORT,RS); /* Instruction Mode RS=0 */
	_delay_ms(1); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */
#if (MODE == 4)
	/* out the highest 4 bits of the required command to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((command & 0xF0)>>(4-D4));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* out the lowest 4 bits of the required command to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((command & 0x0F)<<(D4));
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#elif (MODE == 8)
	LCD_DATA_PORT = command; /* out the required command to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#endif
}

/*---------------------------------------------------------------------------------*/

void LCD_displayCharacter(uint8 data)
{

	SET_BIT(LCD_CTRL_PORT,RS); /* Data Mode RS=1 */
	_delay_ms(1); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */
#if (MODE == 4)
	/* out the highest 4 bits of the required data to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((data & 0xF0)>>(4-D4));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* out the lowest 4 bits of the required data to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((data & 0x0F)<<(D4));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

#elif (DATA_BITS_MODE == 8)
	LCD_DATA_PORT = data; /* out the required command to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#endif
}

/*---------------------------------------------------------------------------------*/

void LCD_backspace(void)
{
    /* Step 1: Move cursor left using cursor shift command (0x10) */
    CLEAR_BIT(LCD_CTRL_PORT,RS); /* Instruction Mode RS=0 */
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
    _delay_ms(1);

    #if (MODE == 4)
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0xF0)>>(4-D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0x0F)<<(D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #elif (MODE == 8)
    LCD_DATA_PORT = 0x10;
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #endif

    /* Step 2: Write space character */
    SET_BIT(LCD_CTRL_PORT,RS); /* Data Mode RS=1 */
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);

    #if (MODE == 4)
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x20 & 0xF0)>>(4-D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x20 & 0x0F)<<(D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #elif (MODE == 8)
    LCD_DATA_PORT = 0x20;
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #endif

    /* Step 3: Move cursor left again (0x10) - NOT entry mode command */
    CLEAR_BIT(LCD_CTRL_PORT,RS);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);

    #if (MODE == 4)
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0xF0)>>(4-D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0x0F)<<(D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #elif (MODE == 8)
    LCD_DATA_PORT = 0x10;
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #endif
}



/*---------------------------------------------------------------------------------*/

void LCD_displayCharacterWithBackspace(uint8 data)
{
    if(data == BACKSPACE_KEY)  /* Check for backspace (0x08) */
    {
        LCD_backspace();
    }
    else
    {
        LCD_displayCharacter(data);
    }
}

/*---------------------------------------------------------------------------------*/

void LCD_displayString(const char *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		LCD_displayCharacter(Str[i]);
		i++;
	}
}

/*---------------------------------------------------------------------------------*/

void LCD_goToRowColumn(uint8 row,uint8 col)
{
	uint8 Address;

	/* first of all calculate the required address */
	switch(row)
	{
		case 0:
				Address=col;
				break;
		case 1:
				Address=col+0x40;
				break;
		case 2:
				Address=col+0x10;
				break;
		case 3:
				Address=col+0x50;
				break;
	}
	/* to write to a specific address in the LCD
	 * we need to apply the corresponding command 0b10000000+Address */
	LCD_sendCommand(Address | SET_CURSOR_LOCATION);
}

/*---------------------------------------------------------------------------------*/

void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *Str)
{
	LCD_goToRowColumn(row,col); /* go to to the required LCD position */
	LCD_displayString(Str); /* display the string */
}

/*---------------------------------------------------------------------------------*/

void LCD_displayInteger(int data)
{
   char buff[16]; /* String to hold the ascii result */
   itoa(data,buff,10); /* 10 for decimal */
   LCD_displayString(buff);
}

/*---------------------------------------------------------------------------------*/

void LCD_clearScreen(void)
{
	LCD_sendCommand(CLEAR_COMMAND);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftDisplayRight(void)
{
	LCD_sendCommand(SHIFT_DISPLAY_RIGHT);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftDisplayLeft(void)
{
	LCD_sendCommand(SHIFT_DISPLAY_LEFT);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftCursorRight(void)
{
	LCD_sendCommand(SHIFT_CURSOR_RIGHT);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftCursorLeft(void)
{
	LCD_sendCommand(SHIFT_CURSOR_LEFT);
}

/*---------------------------------------------------------------------------------*/

void LCD_displayOff(void)
{
	LCD_sendCommand(DISPLAY_OFF_CURSOR_OFF);
}

/*---------------------------------------------------------------------------------*/

void LCD_cursorOff(void)
{
	LCD_sendCommand(DISPLAY_ON_CURSOR_OFF);
}

/*---------------------------------------------------------------------------------*/

void LCD_cursorOn(void)
{
	LCD_sendCommand(DISPLAY_ON_CURSOR_ON);
}

/*---------------------------------------------------------------------------------*/

void LCD_cursorBlink(void)
{
	LCD_sendCommand(DISPLAY_ON_CURSOR_BLINK);
}
=======
/***********************************************************************************
 * 																				   *
 * 	 [MODULE]: LCD																   *
 * 																				   *
 * 	 [FILE NAME]: lcd.c			    											   *
 * 																				   *
 * 	 [AUTHOR]: Ahmed Abdelaal													   *
 * 																				   *
 * 	 [CREATED ON]: 05/09/2022													   *
 * 																				   *
 *   [DESCRIPTION]: source file for the lcd driver						    	   *
 * 																				   *
 ***********************************************************************************/

#include "lcd.h"

/***********************************************************************************
 * 																				   *
 *              	          Functions Definitions                                *
 * 																				   *
 ***********************************************************************************/

void LCD_init(void)
{
	LCD_CTRL_PORT_DIR |= (1 << E) | (1 << RS); /* Configure the control pins(E,RS,RW) as output pins */

#if (MODE == 4)
	LCD_DATA_PORT_DIR |= (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7); /* Configure the data port as output port */
	LCD_sendCommand(FOUR_BITS_DATA_MODE); /* initialize LCD in 4-bit mode */
	LCD_sendCommand(TWO_LINE_LCD_4_BIT_MODE); /* initialize LCD in 4-bit mode */
#elif (MODE == 8)
	LCD_DATA_PORT_DIR = 0xFF; /* Configure the data port as output port */
	LCD_sendCommand(TWO_LINE_LCD_8_BIT_MODE); /* use 2-line lcd + 8-bit Data Mode + 5*7 dot display Mode */
#endif

	LCD_sendCommand(DISPLAY_ON_CURSOR_OFF); /* cursor off */
	LCD_sendCommand(CLEAR_COMMAND); /* clear LCD at the beginning */
}

/*---------------------------------------------------------------------------------*/

void LCD_sendCommand(uint8 command)
{
	CLEAR_BIT(LCD_CTRL_PORT,RS); /* Instruction Mode RS=0 */
	_delay_ms(1); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */
#if (MODE == 4)
	/* out the highest 4 bits of the required command to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((command & 0xF0)>>(4-D4));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* out the lowest 4 bits of the required command to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((command & 0x0F)<<(D4));
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#elif (MODE == 8)
	LCD_DATA_PORT = command; /* out the required command to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#endif
}

/*---------------------------------------------------------------------------------*/

void LCD_displayCharacter(uint8 data)
{

	SET_BIT(LCD_CTRL_PORT,RS); /* Data Mode RS=1 */
	_delay_ms(1); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */
#if (MODE == 4)
	/* out the highest 4 bits of the required data to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((data & 0xF0)>>(4-D4));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* out the lowest 4 bits of the required data to the data bus D4 --> D7 */
	LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((data & 0x0F)<<(D4));

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

#elif (DATA_BITS_MODE == 8)
	LCD_DATA_PORT = data; /* out the required command to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#endif
}

/*---------------------------------------------------------------------------------*/

void LCD_backspace(void)
{
    /* Step 1: Move cursor left using cursor shift command (0x10) */
    CLEAR_BIT(LCD_CTRL_PORT,RS); /* Instruction Mode RS=0 */
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
    _delay_ms(1);

    #if (MODE == 4)
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0xF0)>>(4-D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0x0F)<<(D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #elif (MODE == 8)
    LCD_DATA_PORT = 0x10;
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #endif

    /* Step 2: Write space character */
    SET_BIT(LCD_CTRL_PORT,RS); /* Data Mode RS=1 */
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);

    #if (MODE == 4)
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x20 & 0xF0)>>(4-D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x20 & 0x0F)<<(D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #elif (MODE == 8)
    LCD_DATA_PORT = 0x20;
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #endif

    /* Step 3: Move cursor left again (0x10) - NOT entry mode command */
    CLEAR_BIT(LCD_CTRL_PORT,RS);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);

    #if (MODE == 4)
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0xF0)>>(4-D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    SET_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    LCD_DATA_PORT = (LCD_DATA_PORT & ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7))) | ((0x10 & 0x0F)<<(D4));
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #elif (MODE == 8)
    LCD_DATA_PORT = 0x10;
    _delay_ms(1);
    CLEAR_BIT(LCD_CTRL_PORT,E);
    _delay_ms(1);
    #endif
}



/*---------------------------------------------------------------------------------*/

void LCD_displayCharacterWithBackspace(uint8 data)
{
    if(data == BACKSPACE_KEY)  /* Check for backspace (0x08) */
    {
        LCD_backspace();
    }
    else
    {
        LCD_displayCharacter(data);
    }
}

/*---------------------------------------------------------------------------------*/

void LCD_displayString(const char *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		LCD_displayCharacter(Str[i]);
		i++;
	}
}

/*---------------------------------------------------------------------------------*/

void LCD_goToRowColumn(uint8 row,uint8 col)
{
	uint8 Address;

	/* first of all calculate the required address */
	switch(row)
	{
		case 0:
				Address=col;
				break;
		case 1:
				Address=col+0x40;
				break;
		case 2:
				Address=col+0x10;
				break;
		case 3:
				Address=col+0x50;
				break;
	}
	/* to write to a specific address in the LCD
	 * we need to apply the corresponding command 0b10000000+Address */
	LCD_sendCommand(Address | SET_CURSOR_LOCATION);
}

/*---------------------------------------------------------------------------------*/

void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *Str)
{
	LCD_goToRowColumn(row,col); /* go to to the required LCD position */
	LCD_displayString(Str); /* display the string */
}

/*---------------------------------------------------------------------------------*/

void LCD_displayInteger(int data)
{
   char buff[16]; /* String to hold the ascii result */
   itoa(data,buff,10); /* 10 for decimal */
   LCD_displayString(buff);
}

/*---------------------------------------------------------------------------------*/

void LCD_clearScreen(void)
{
	LCD_sendCommand(CLEAR_COMMAND);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftDisplayRight(void)
{
	LCD_sendCommand(SHIFT_DISPLAY_RIGHT);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftDisplayLeft(void)
{
	LCD_sendCommand(SHIFT_DISPLAY_LEFT);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftCursorRight(void)
{
	LCD_sendCommand(SHIFT_CURSOR_RIGHT);
}

/*---------------------------------------------------------------------------------*/

void LCD_shiftCursorLeft(void)
{
	LCD_sendCommand(SHIFT_CURSOR_LEFT);
}

/*---------------------------------------------------------------------------------*/

void LCD_displayOff(void)
{
	LCD_sendCommand(DISPLAY_OFF_CURSOR_OFF);
}

/*---------------------------------------------------------------------------------*/

void LCD_cursorOff(void)
{
	LCD_sendCommand(DISPLAY_ON_CURSOR_OFF);
}

/*---------------------------------------------------------------------------------*/

void LCD_cursorOn(void)
{
	LCD_sendCommand(DISPLAY_ON_CURSOR_ON);
}

/*---------------------------------------------------------------------------------*/

void LCD_cursorBlink(void)
{
	LCD_sendCommand(DISPLAY_ON_CURSOR_BLINK);
}
>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
