/***********************************************************************************
 *                                                                                 *
 * [MODULE]: EEPROM                                                                *
 *                                                                                 *
 * [FILE NAME]: EEPROM.c                                                           *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 13/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Source file for the EEPROM driver for ATmega328P                *
 *                                                                                 *
 ***********************************************************************************/

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "eeprom.h"


/*---------------------------------------------------------------------------------*
 *                              GLOBAL VARIABLES                                   *
 *---------------------------------------------------------------------------------*/

/* Current operation mode */
static EEPROM_Mode_t g_eepromMode = EEPROM_POLLING_MODE;

/* Last error status */
static volatile EEPROM_Error_t g_lastError = EEPROM_NO_ERROR;

/* Programming mode */
static uint8 g_programmingMode = EEPROM_PROGRAMMING_MODE;

/* Operation complete flag (for interrupt mode) */
static volatile uint8 g_operationComplete = 1;

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION PROTOTYPES                           *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 *                              FUNCTION DEFINITIONS                               *
 *---------------------------------------------------------------------------------*/

void EEPROM_init(const EEPROM_Config_t* config)
{
    /* Store configuration */
    g_eepromMode = config->mode;
    g_programmingMode = config->programmingMode;

    /* Clear error status */
    g_lastError = EEPROM_NO_ERROR;
    g_operationComplete = 1;

    /* Enable interrupt if requested */
    if(config->enableInterrupt && config->mode == EEPROM_INTERRUPT_MODE)
    {
        /* Enable EEPROM Ready Interrupt */
        SET_BIT(EECR, EERIE);

        /* Enable global interrupts */
        sei();
    }
    else
    {
        /* Disable EEPROM Ready Interrupt */
        CLEAR_BIT(EECR, EERIE);
    }
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_writeByte(uint16 address, uint8 data)
{
    /* Validate address */
    if(!EEPROM_validateAddress(address))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Use AVR libc function (handles timing automatically) */
    eeprom_write_byte((uint8_t*)address, data);

    g_lastError = EEPROM_NO_ERROR;
    return EEPROM_NO_ERROR;
}


/*---------------------------------------------------------------------------------*/

uint8 EEPROM_readByte(uint16 address)
{
    /* Validate address */
    if(!EEPROM_validateAddress(address))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return 0xFF;
    }

    /* Use AVR libc function */
    return eeprom_read_byte((const uint8_t*)address);
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_writeBlock(uint16 startAddress, const uint8* data, uint16 length)
{
    uint16 i;
    EEPROM_Error_t status;

    /* Validate parameters */
    if(data == NULL)
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    if(!EEPROM_validateAddress(startAddress) ||
       !EEPROM_validateAddress(startAddress + length - 1))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Write each byte */
    for(i = 0; i < length; i++)
    {
        status = EEPROM_writeByte(startAddress + i, data[i]);
        if(status != EEPROM_NO_ERROR)
        {
            return status;
        }
    }

    return EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_readBlock(uint16 startAddress, uint8* data, uint16 length)
{
    uint16 i;

    /* Validate parameters */
    if(data == NULL)
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    if(!EEPROM_validateAddress(startAddress) ||
       !EEPROM_validateAddress(startAddress + length - 1))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Read each byte */
    for(i = 0; i < length; i++)
    {
        data[i] = EEPROM_readByte(startAddress + i);
    }

    return EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_writeString(uint16 startAddress, const char* str)
{
    uint16 length;

    /* Validate parameter */
    if(str == NULL)
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Get string length including null terminator */
    length = strlen(str) + 1;

    /* Write string as block */
    return EEPROM_writeBlock(startAddress, (const uint8*)str, length);
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_readString(uint16 startAddress, char* str, uint16 maxLength)
{
    uint16 i;
    uint8 currentChar;

    /* Validate parameters */
    if(str == NULL || maxLength == 0)
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    if(!EEPROM_validateAddress(startAddress))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Read characters until null terminator or max length */
    for(i = 0; i < maxLength - 1; i++)
    {
        currentChar = EEPROM_readByte(startAddress + i);
        str[i] = currentChar;

        if(currentChar == '\0')
        {
            break;
        }
    }

    /* Ensure null termination */
    str[i] = '\0';

    return EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_writeInteger(uint16 address, uint32 value, uint8 size)
{
    uint8 i;
    uint8 byteData;

    /* Validate size */
    if(size != 2 && size != 4)
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Validate address range */
    if(!EEPROM_validateAddress(address) ||
       !EEPROM_validateAddress(address + size - 1))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Write each byte (little-endian) */
    for(i = 0; i < size; i++)
    {
        byteData = (uint8)(value >> (i * 8));
        if(EEPROM_writeByte(address + i, byteData) != EEPROM_NO_ERROR)
        {
            return g_lastError;
        }
    }

    return EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

uint32 EEPROM_readInteger(uint16 address, uint8 size)
{
    uint8 i;
    uint32 result = 0;
    uint8 byteData;

    /* Validate size */
    if(size != 2 && size != 4)
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return 0;
    }

    /* Validate address range */
    if(!EEPROM_validateAddress(address) ||
       !EEPROM_validateAddress(address + size - 1))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return 0;
    }

    /* Read each byte (little-endian) */
    for(i = 0; i < size; i++)
    {
        byteData = EEPROM_readByte(address + i);
        result |= ((uint32)byteData << (i * 8));
    }

    return result;
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_erase(void)
{
    uint16 i;

    /* Write 0xFF to all EEPROM locations */
    for(i = 0; i < EEPROM_SIZE; i++)
    {
        if(EEPROM_writeByte(i, 0xFF) != EEPROM_NO_ERROR)
        {
            return g_lastError;
        }
    }

    return EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

uint8 EEPROM_isReady(void)
{
    /* Check if EEPE bit is cleared (ready) */
    return BIT_IS_CLEAR(EECR, EEPE);
}

/*---------------------------------------------------------------------------------*/

EEPROM_Status_t EEPROM_getStatus(void)
{
    if(EEPROM_isReady())
    {
        return EEPROM_READY;
    }
    else
    {
        return EEPROM_BUSY;
    }
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_getLastError(void)
{
    return g_lastError;
}

/*---------------------------------------------------------------------------------*/

void EEPROM_clearError(void)
{
    g_lastError = EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

uint8 EEPROM_validateAddress(uint16 address)
{
    return (address <= EEPROM_END_ADDRESS);
}
/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_writeFloat(uint16 address, float value)
{
    uint8 i;
    uint8 byteData;
    uint32 floatAsInt;

    /* Validate address range (need 4 bytes) */
    if(!EEPROM_validateAddress(address) ||
       !EEPROM_validateAddress(address + 3))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Convert float to bytes using union */
    union {
        float f;
        uint32 i;
    } floatConverter;

    floatConverter.f = value;
    floatAsInt = floatConverter.i;

    /* Write each byte (little-endian) */
    for(i = 0; i < 4; i++)
    {
        byteData = (uint8)(floatAsInt >> (i * 8));
        if(EEPROM_writeByte(address + i, byteData) != EEPROM_NO_ERROR)
        {
            return g_lastError;
        }
    }

    return EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

float EEPROM_readFloat(uint16 address)
{
    uint8 i;
    uint32 floatAsInt = 0;
    uint8 byteData;

    /* Validate address range (need 4 bytes) */
    if(!EEPROM_validateAddress(address) ||
       !EEPROM_validateAddress(address + 3))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return 0.0f;
    }

    /* Read each byte (little-endian) */
    for(i = 0; i < 4; i++)
    {
        byteData = EEPROM_readByte(address + i);
        floatAsInt |= ((uint32)byteData << (i * 8));
    }

    /* Convert bytes to float using union */
    union {
        float f;
        uint32 i;
    } floatConverter;

    floatConverter.i = floatAsInt;

    return floatConverter.f;
}

/*---------------------------------------------------------------------------------*/

EEPROM_Error_t EEPROM_writeDouble(uint16 address, double value)
{
    uint8 i;
    uint8 byteData;
    unsigned long long int doubleAsInt;

    /* Validate address range (need 8 bytes) */
    if(!EEPROM_validateAddress(address) ||
       !EEPROM_validateAddress(address + 7))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return EEPROM_ADDRESS_ERROR;
    }

    /* Convert double to bytes using union */
    union {
        double d;
        unsigned long long int i;
    } doubleConverter;

    doubleConverter.d = value;
    doubleAsInt = doubleConverter.i;

    /* Write each byte (little-endian) */
    for(i = 0; i < 8; i++)
    {
        byteData = (uint8)(doubleAsInt >> (i * 8));
        if(EEPROM_writeByte(address + i, byteData) != EEPROM_NO_ERROR)
        {
            return g_lastError;
        }
    }

    return EEPROM_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

double EEPROM_readDouble(uint16 address)
{
    uint8 i;
    unsigned long long int doubleAsInt = 0;
    uint8 byteData;

    /* Validate address range (need 8 bytes) */
    if(!EEPROM_validateAddress(address) ||
       !EEPROM_validateAddress(address + 7))
    {
        g_lastError = EEPROM_ADDRESS_ERROR;
        return 0.0;
    }

    /* Read each byte (little-endian) */
    for(i = 0; i < 8; i++)
    {
        byteData = EEPROM_readByte(address + i);
        doubleAsInt |= ((unsigned long long int)byteData << (i * 8));
    }

    /* Convert bytes to double using union */
    union {
        double d;
        unsigned long long int i;
    } doubleConverter;

    doubleConverter.i = doubleAsInt;

    return doubleConverter.d;
}

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION DEFINITIONS                          *
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 *                              INTERRUPT SERVICE ROUTINES                         *
 *---------------------------------------------------------------------------------*/

/*
 * ISR: EEPROM Ready Interrupt
 * Description: Triggered when EEPROM write/erase operation completes
 */
ISR(EE_READY_vect)
{
    /* Set operation complete flag */
    g_operationComplete = 1;

    /* Optional: Add callback mechanism here */
}
