<<<<<<< HEAD
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: KEYPAD                                                                *
 *                                                                                 *
 * [FILE NAME]: KEYPAD.c                                                           *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 25/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Source file for 4x4 matrix keypad driver                        *
 *                                                                                 *
 ***********************************************************************************/

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "KEYPAD.h"
#include <util/delay.h>

/*---------------------------------------------------------------------------------*
 *                              GLOBAL VARIABLES                                   *
 *---------------------------------------------------------------------------------*/

/* 4x4 Keypad layout array */
static const uint8 g_keypadLayout[KEYPAD_NUM_ROWS][KEYPAD_NUM_COLS] = {
    {'1', '2', '3', 'A'},    /* Row 0 */
    {'4', '5', '6', 'B'},    /* Row 1 */
    {'7', '8', '9', 'C'},    /* Row 2 */
    {'*', '0', '#', 'D'}     /* Row 3 */
};

/* Array to hold row pins */
static const uint8 g_rowPins[KEYPAD_NUM_ROWS] = {
    KEYPAD_ROW0_PIN,
    KEYPAD_ROW1_PIN,
    KEYPAD_ROW2_PIN,
    KEYPAD_ROW3_PIN
};

/* Array to hold column pins */
static const uint8 g_colPins[KEYPAD_NUM_COLS] = {
    KEYPAD_COL0_PIN,
    KEYPAD_COL1_PIN,
    KEYPAD_COL2_PIN,
    KEYPAD_COL3_PIN
};

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION PROTOTYPES                           *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Scan keypad matrix and return pressed key
 * Returns: Key value or KEYPAD_NOT_PRESSED
 */
static uint8 KEYPAD_scanMatrix(void);

/*
 * Description: Debounce delay
 */
static void KEYPAD_debounce(void);

/*---------------------------------------------------------------------------------*
 *                              FUNCTION DEFINITIONS                               *
 *---------------------------------------------------------------------------------*/

void KEYPAD_init(void)
{

    /* Configure rows as output (PORTC pins 0-3) */
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW0_PIN);
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW1_PIN);
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW2_PIN);
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW3_PIN);

    /* Set all rows HIGH initially */
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW0_PIN);
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW1_PIN);
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW2_PIN);
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW3_PIN);

    /* Configure columns as input (PORTD pins 0-3) */
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL0_PIN);
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL1_PIN);
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL2_PIN);
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL3_PIN);

    /* Enable internal pull-up resistors for columns */
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL0_PIN);
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL1_PIN);
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL2_PIN);
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL3_PIN);
}

/*---------------------------------------------------------------------------------*/

uint8 KEYPAD_getPressedKey(void)
{
    uint8 key;

    /* Wait until a key is pressed */
    while(1)
    {
        key = KEYPAD_scanMatrix();

        if(key != KEYPAD_NOT_PRESSED)
        {
            /* Key detected, debounce */
            KEYPAD_debounce();

            /* Verify key is still pressed after debounce */
            if(KEYPAD_scanMatrix() == key)
            {
                /* Wait for key release to prevent repeated detection */
                KEYPAD_waitForRelease();
                return key;
            }
        }
    }
}

/*---------------------------------------------------------------------------------*/

uint8 KEYPAD_getPressedKeyNonBlocking(void)
{
    uint8 key;

    /* Scan keypad once */
    key = KEYPAD_scanMatrix();

    if(key != KEYPAD_NOT_PRESSED)
    {
        /* Key detected, debounce */
        KEYPAD_debounce();

        /* Verify key is still pressed after debounce */
        if(KEYPAD_scanMatrix() == key)
        {
            return key;
        }
    }

    return KEYPAD_NOT_PRESSED;
}

/*---------------------------------------------------------------------------------*/

uint8 KEYPAD_isKeyPressed(void)
{
    return (KEYPAD_scanMatrix() != KEYPAD_NOT_PRESSED) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/

void KEYPAD_waitForRelease(void)
{
    /* Wait until all keys are released */
    while(KEYPAD_scanMatrix() != KEYPAD_NOT_PRESSED)
    {
        _delay_ms(10);
    }

    /* Additional debounce after release */
    KEYPAD_debounce();
}

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION DEFINITIONS                          *
 *---------------------------------------------------------------------------------*/

static uint8 KEYPAD_scanMatrix(void)
{
    uint8 row, col;

    /* Scan each row */
    for(row = 0; row < KEYPAD_NUM_ROWS; row++)
    {
        /* Set current row LOW, others HIGH */
        KEYPAD_ROW_PORT_OUT = 0xFF;  /* Set all rows HIGH first */
        CLEAR_BIT(KEYPAD_ROW_PORT_OUT, g_rowPins[row]);  /* Set current row LOW */

        /* Small delay for signal stabilization */
        _delay_us(5);

        /* Check each column */
        for(col = 0; col < KEYPAD_NUM_COLS; col++)
        {
            /* If column is LOW, key is pressed */
            if(BIT_IS_CLEAR(KEYPAD_COL_PORT_IN, g_colPins[col]))
            {
                /* Return the key value from layout array */
                return g_keypadLayout[row][col];
            }
        }
    }

    /* No key pressed */
    return KEYPAD_NOT_PRESSED;
}

/*---------------------------------------------------------------------------------*/

static void KEYPAD_debounce(void)
{
    _delay_ms(KEYPAD_DEBOUNCE_TIME_MS);
}
=======
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: KEYPAD                                                                *
 *                                                                                 *
 * [FILE NAME]: KEYPAD.c                                                           *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 25/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Source file for 4x4 matrix keypad driver                        *
 *                                                                                 *
 ***********************************************************************************/

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "KEYPAD.h"
#include <util/delay.h>

/*---------------------------------------------------------------------------------*
 *                              GLOBAL VARIABLES                                   *
 *---------------------------------------------------------------------------------*/

/* 4x4 Keypad layout array */
static const uint8 g_keypadLayout[KEYPAD_NUM_ROWS][KEYPAD_NUM_COLS] = {
    {'1', '2', '3', 'A'},    /* Row 0 */
    {'4', '5', '6', 'B'},    /* Row 1 */
    {'7', '8', '9', 'C'},    /* Row 2 */
    {'*', '0', '#', 'D'}     /* Row 3 */
};

/* Array to hold row pins */
static const uint8 g_rowPins[KEYPAD_NUM_ROWS] = {
    KEYPAD_ROW0_PIN,
    KEYPAD_ROW1_PIN,
    KEYPAD_ROW2_PIN,
    KEYPAD_ROW3_PIN
};

/* Array to hold column pins */
static const uint8 g_colPins[KEYPAD_NUM_COLS] = {
    KEYPAD_COL0_PIN,
    KEYPAD_COL1_PIN,
    KEYPAD_COL2_PIN,
    KEYPAD_COL3_PIN
};

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION PROTOTYPES                           *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Scan keypad matrix and return pressed key
 * Returns: Key value or KEYPAD_NOT_PRESSED
 */
static uint8 KEYPAD_scanMatrix(void);

/*
 * Description: Debounce delay
 */
static void KEYPAD_debounce(void);

/*---------------------------------------------------------------------------------*
 *                              FUNCTION DEFINITIONS                               *
 *---------------------------------------------------------------------------------*/

void KEYPAD_init(void)
{

    /* Configure rows as output (PORTC pins 0-3) */
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW0_PIN);
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW1_PIN);
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW2_PIN);
    SET_BIT(KEYPAD_ROW_PORT_DIR, KEYPAD_ROW3_PIN);

    /* Set all rows HIGH initially */
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW0_PIN);
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW1_PIN);
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW2_PIN);
    SET_BIT(KEYPAD_ROW_PORT_OUT, KEYPAD_ROW3_PIN);

    /* Configure columns as input (PORTD pins 0-3) */
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL0_PIN);
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL1_PIN);
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL2_PIN);
    CLEAR_BIT(KEYPAD_COL_PORT_DIR, KEYPAD_COL3_PIN);

    /* Enable internal pull-up resistors for columns */
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL0_PIN);
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL1_PIN);
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL2_PIN);
    SET_BIT(KEYPAD_COL_PORT_OUT, KEYPAD_COL3_PIN);
}

/*---------------------------------------------------------------------------------*/

uint8 KEYPAD_getPressedKey(void)
{
    uint8 key;

    /* Wait until a key is pressed */
    while(1)
    {
        key = KEYPAD_scanMatrix();

        if(key != KEYPAD_NOT_PRESSED)
        {
            /* Key detected, debounce */
            KEYPAD_debounce();

            /* Verify key is still pressed after debounce */
            if(KEYPAD_scanMatrix() == key)
            {
                /* Wait for key release to prevent repeated detection */
                KEYPAD_waitForRelease();
                return key;
            }
        }
    }
}

/*---------------------------------------------------------------------------------*/

uint8 KEYPAD_getPressedKeyNonBlocking(void)
{
    uint8 key;

    /* Scan keypad once */
    key = KEYPAD_scanMatrix();

    if(key != KEYPAD_NOT_PRESSED)
    {
        /* Key detected, debounce */
        KEYPAD_debounce();

        /* Verify key is still pressed after debounce */
        if(KEYPAD_scanMatrix() == key)
        {
            return key;
        }
    }

    return KEYPAD_NOT_PRESSED;
}

/*---------------------------------------------------------------------------------*/

uint8 KEYPAD_isKeyPressed(void)
{
    return (KEYPAD_scanMatrix() != KEYPAD_NOT_PRESSED) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/

void KEYPAD_waitForRelease(void)
{
    /* Wait until all keys are released */
    while(KEYPAD_scanMatrix() != KEYPAD_NOT_PRESSED)
    {
        _delay_ms(10);
    }

    /* Additional debounce after release */
    KEYPAD_debounce();
}

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION DEFINITIONS                          *
 *---------------------------------------------------------------------------------*/

static uint8 KEYPAD_scanMatrix(void)
{
    uint8 row, col;

    /* Scan each row */
    for(row = 0; row < KEYPAD_NUM_ROWS; row++)
    {
        /* Set current row LOW, others HIGH */
        KEYPAD_ROW_PORT_OUT = 0xFF;  /* Set all rows HIGH first */
        CLEAR_BIT(KEYPAD_ROW_PORT_OUT, g_rowPins[row]);  /* Set current row LOW */

        /* Small delay for signal stabilization */
        _delay_us(5);

        /* Check each column */
        for(col = 0; col < KEYPAD_NUM_COLS; col++)
        {
            /* If column is LOW, key is pressed */
            if(BIT_IS_CLEAR(KEYPAD_COL_PORT_IN, g_colPins[col]))
            {
                /* Return the key value from layout array */
                return g_keypadLayout[row][col];
            }
        }
    }

    /* No key pressed */
    return KEYPAD_NOT_PRESSED;
}

/*---------------------------------------------------------------------------------*/

static void KEYPAD_debounce(void)
{
    _delay_ms(KEYPAD_DEBOUNCE_TIME_MS);
}
>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
