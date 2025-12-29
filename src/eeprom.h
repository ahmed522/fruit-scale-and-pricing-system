<<<<<<< HEAD
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: EEPROM                                                                *
 *                                                                                 *
 * [FILE NAME]: EEPROM.h                                                           *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 13/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Header file for the EEPROM driver for ATmega328P                *
 *                Provides functions for reading, writing, and managing            *
 *                internal EEPROM memory with interrupt support                    *
 *                                                                                 *
 ***********************************************************************************/

#ifndef EEPROM_H_
#define EEPROM_H_

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"
#include <avr/interrupt.h>
#include <string.h>
#include <avr/eeprom.h>

/*---------------------------------------------------------------------------------*
 *                              GLOBAL CONST MACROS                                *
 *---------------------------------------------------------------------------------*/

/* ATmega328P EEPROM Size: 1024 bytes (1KB) */
#define EEPROM_SIZE                     1024

/* EEPROM Address Range */
#define EEPROM_START_ADDRESS            0x0000
#define EEPROM_END_ADDRESS              (EEPROM_SIZE - 1)

/* EEPROM Register Bit Definitions */
/* EECR - EEPROM Control Register */
#define EERE    0  /* EEPROM Read Enable */
#define EEPE    1  /* EEPROM Write Enable (formerly EEWE) */
#define EEMPE   2  /* EEPROM Master Write Enable (formerly EEMWE) */
#define EERIE   3  /* EEPROM Ready Interrupt Enable */
#define EEPM0   4  /* EEPROM Programming Mode Bit 0 */
#define EEPM1   5  /* EEPROM Programming Mode Bit 1 */

/* EEPROM Programming Modes */
#define EEPROM_ERASE_AND_WRITE_MODE     0x00  /* 3.4ms - Erase then Write */
#define EEPROM_ERASE_ONLY_MODE          0x01  /* 1.8ms - Erase only */
#define EEPROM_WRITE_ONLY_MODE          0x02  /* 1.8ms - Write only */

/* Default Programming Mode */
#ifndef EEPROM_PROGRAMMING_MODE
#define EEPROM_PROGRAMMING_MODE         EEPROM_ERASE_AND_WRITE_MODE
#endif

/* EEPROM Timing Constants (in milliseconds) */
#define EEPROM_WRITE_TIME_MS            4     /* Max write time */
#define EEPROM_ERASE_TIME_MS            2     /* Max erase time */

/* EEPROM Page Size (for optimization) */
#define EEPROM_PAGE_SIZE                4     /* ATmega328P has 4-byte pages */

/*---------------------------------------------------------------------------------*
 *                                     ENUMS                                       *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Enumeration for EEPROM operation mode
 *
 * EEPROM_POLLING_MODE     : Blocking polling mode
 * EEPROM_INTERRUPT_MODE   : Non-blocking interrupt mode
 */
typedef enum
{
    EEPROM_POLLING_MODE = 0,
    EEPROM_INTERRUPT_MODE
} EEPROM_Mode_t;

/*
 * Description: Enumeration for EEPROM error types
 *
 * EEPROM_NO_ERROR          : No error
 * EEPROM_ADDRESS_ERROR     : Invalid address (out of range)
 * EEPROM_WRITE_ERROR       : Write operation failed
 * EEPROM_BUSY_ERROR        : EEPROM is busy
 */
typedef enum
{
    EEPROM_NO_ERROR = 0,
    EEPROM_ADDRESS_ERROR,
    EEPROM_WRITE_ERROR,
    EEPROM_BUSY_ERROR
} EEPROM_Error_t;

/*
 * Description: Enumeration for EEPROM operation status
 *
 * EEPROM_READY      : EEPROM ready for operation
 * EEPROM_BUSY       : EEPROM busy with operation
 */
typedef enum
{
    EEPROM_READY = 0,
    EEPROM_BUSY
} EEPROM_Status_t;

/*---------------------------------------------------------------------------------*
 *                              STRUCTS AND UNIONS                                 *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Structure for EEPROM configuration
 *
 * mode             : Operation mode (polling or interrupt)
 * programmingMode  : Programming mode (erase+write, erase only, write only)
 * enableInterrupt  : Enable ready interrupt
 */
typedef struct
{
    EEPROM_Mode_t mode;
    uint8 programmingMode;
    uint8 enableInterrupt;
} EEPROM_Config_t;

/*---------------------------------------------------------------------------------*
 *                              FUNCTION PROTOTYPES                                *
 *---------------------------------------------------------------------------------*/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_init
 *
 * [FUNCTION DESCRIPTION]: Initialize EEPROM module with specified configuration
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: const EEPROM_Config_t* config - pointer to configuration
 *           [out]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/
void EEPROM_init(const EEPROM_Config_t* config);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeByte
 *
 * [FUNCTION DESCRIPTION]: Write a single byte to EEPROM at specified address
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address (0-1023)
 *                 uint8 data - byte to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeByte(uint16 address, uint8 data);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readByte
 *
 * [FUNCTION DESCRIPTION]: Read a single byte from EEPROM at specified address
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address (0-1023)
 *           [out]: none
 *
 * [return]: uint8 - byte read from EEPROM
 *
 *---------------------------------------------------------------------------------*/
uint8 EEPROM_readByte(uint16 address);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeBlock
 *
 * [FUNCTION DESCRIPTION]: Write a block of data to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 const uint8* data - pointer to data buffer
 *                 uint16 length - number of bytes to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeBlock(uint16 startAddress, const uint8* data, uint16 length);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readBlock
 *
 * [FUNCTION DESCRIPTION]: Read a block of data from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 uint16 length - number of bytes to read
 *           [out]: uint8* data - pointer to buffer for read data
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_readBlock(uint16 startAddress, uint8* data, uint16 length);

/*[6]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeString
 *
 * [FUNCTION DESCRIPTION]: Write a null-terminated string to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 const char* str - pointer to string
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeString(uint16 startAddress, const char* str);

/*[7]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readString
 *
 * [FUNCTION DESCRIPTION]: Read a null-terminated string from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 uint16 maxLength - maximum string length (including null)
 *           [out]: char* str - pointer to buffer for string
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_readString(uint16 startAddress, char* str, uint16 maxLength);

/*[8]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeInteger
 *
 * [FUNCTION DESCRIPTION]: Write an integer (16-bit or 32-bit) to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 uint32 value - integer value to write
 *                 uint8 size - size in bytes (2 or 4)
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeInteger(uint16 address, uint32 value, uint8 size);

/*[9]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readInteger
 *
 * [FUNCTION DESCRIPTION]: Read an integer (16-bit or 32-bit) from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 uint8 size - size in bytes (2 or 4)
 *           [out]: none
 *
 * [return]: uint32 - integer value read
 *
 *---------------------------------------------------------------------------------*/
uint32 EEPROM_readInteger(uint16 address, uint8 size);

/*[10]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_erase
 *
 * [FUNCTION DESCRIPTION]: Erase EEPROM by writing 0xFF to all locations
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_erase(void);

/*[11]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_isReady
 *
 * [FUNCTION DESCRIPTION]: Check if EEPROM is ready for operation
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - 1 if ready, 0 if busy
 *
 *---------------------------------------------------------------------------------*/
uint8 EEPROM_isReady(void);

/*[12]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_getStatus
 *
 * [FUNCTION DESCRIPTION]: Get current EEPROM status
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: EEPROM_Status_t - current status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Status_t EEPROM_getStatus(void);

/*[13]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_getLastError
 *
 * [FUNCTION DESCRIPTION]: Get last error that occurred
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - last error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_getLastError(void);

/*[14]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_clearError
 *
 * [FUNCTION DESCRIPTION]: Clear error flag
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
void EEPROM_clearError(void);

/*[15]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_validateAddress
 *
 * [FUNCTION DESCRIPTION]: Validate if address is within EEPROM range
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - address to validate
 *           [out]: none
 *
 * [return]: uint8 - 1 if valid, 0 if invalid
 *
 *---------------------------------------------------------------------------------*/
uint8 EEPROM_validateAddress(uint16 address);
/*[16]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeFloat
 *
 * [FUNCTION DESCRIPTION]: Write a float (32-bit) to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 float value - float value to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeFloat(uint16 address, float value);

/*[17]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readFloat
 *
 * [FUNCTION DESCRIPTION]: Read a float (32-bit) from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *           [out]: none
 *
 * [return]: float - float value read
 *
 *---------------------------------------------------------------------------------*/
float EEPROM_readFloat(uint16 address);

/*[18]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeDouble
 *
 * [FUNCTION DESCRIPTION]: Write a double (64-bit) to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 double value - double value to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeDouble(uint16 address, double value);

/*[19]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readDouble
 *
 * [FUNCTION DESCRIPTION]: Read a double (64-bit) from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *           [out]: none
 *
 * [return]: double - double value read
 *
 *---------------------------------------------------------------------------------*/
double EEPROM_readDouble(uint16 address);

#endif /* EEPROM_H_ */
=======
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: EEPROM                                                                *
 *                                                                                 *
 * [FILE NAME]: EEPROM.h                                                           *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 13/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Header file for the EEPROM driver for ATmega328P                *
 *                Provides functions for reading, writing, and managing            *
 *                internal EEPROM memory with interrupt support                    *
 *                                                                                 *
 ***********************************************************************************/

#ifndef EEPROM_H_
#define EEPROM_H_

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"
#include <avr/interrupt.h>
#include <string.h>
#include <avr/eeprom.h>

/*---------------------------------------------------------------------------------*
 *                              GLOBAL CONST MACROS                                *
 *---------------------------------------------------------------------------------*/

/* ATmega328P EEPROM Size: 1024 bytes (1KB) */
#define EEPROM_SIZE                     1024

/* EEPROM Address Range */
#define EEPROM_START_ADDRESS            0x0000
#define EEPROM_END_ADDRESS              (EEPROM_SIZE - 1)

/* EEPROM Register Bit Definitions */
/* EECR - EEPROM Control Register */
#define EERE    0  /* EEPROM Read Enable */
#define EEPE    1  /* EEPROM Write Enable (formerly EEWE) */
#define EEMPE   2  /* EEPROM Master Write Enable (formerly EEMWE) */
#define EERIE   3  /* EEPROM Ready Interrupt Enable */
#define EEPM0   4  /* EEPROM Programming Mode Bit 0 */
#define EEPM1   5  /* EEPROM Programming Mode Bit 1 */

/* EEPROM Programming Modes */
#define EEPROM_ERASE_AND_WRITE_MODE     0x00  /* 3.4ms - Erase then Write */
#define EEPROM_ERASE_ONLY_MODE          0x01  /* 1.8ms - Erase only */
#define EEPROM_WRITE_ONLY_MODE          0x02  /* 1.8ms - Write only */

/* Default Programming Mode */
#ifndef EEPROM_PROGRAMMING_MODE
#define EEPROM_PROGRAMMING_MODE         EEPROM_ERASE_AND_WRITE_MODE
#endif

/* EEPROM Timing Constants (in milliseconds) */
#define EEPROM_WRITE_TIME_MS            4     /* Max write time */
#define EEPROM_ERASE_TIME_MS            2     /* Max erase time */

/* EEPROM Page Size (for optimization) */
#define EEPROM_PAGE_SIZE                4     /* ATmega328P has 4-byte pages */

/*---------------------------------------------------------------------------------*
 *                                     ENUMS                                       *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Enumeration for EEPROM operation mode
 *
 * EEPROM_POLLING_MODE     : Blocking polling mode
 * EEPROM_INTERRUPT_MODE   : Non-blocking interrupt mode
 */
typedef enum
{
    EEPROM_POLLING_MODE = 0,
    EEPROM_INTERRUPT_MODE
} EEPROM_Mode_t;

/*
 * Description: Enumeration for EEPROM error types
 *
 * EEPROM_NO_ERROR          : No error
 * EEPROM_ADDRESS_ERROR     : Invalid address (out of range)
 * EEPROM_WRITE_ERROR       : Write operation failed
 * EEPROM_BUSY_ERROR        : EEPROM is busy
 */
typedef enum
{
    EEPROM_NO_ERROR = 0,
    EEPROM_ADDRESS_ERROR,
    EEPROM_WRITE_ERROR,
    EEPROM_BUSY_ERROR
} EEPROM_Error_t;

/*
 * Description: Enumeration for EEPROM operation status
 *
 * EEPROM_READY      : EEPROM ready for operation
 * EEPROM_BUSY       : EEPROM busy with operation
 */
typedef enum
{
    EEPROM_READY = 0,
    EEPROM_BUSY
} EEPROM_Status_t;

/*---------------------------------------------------------------------------------*
 *                              STRUCTS AND UNIONS                                 *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Structure for EEPROM configuration
 *
 * mode             : Operation mode (polling or interrupt)
 * programmingMode  : Programming mode (erase+write, erase only, write only)
 * enableInterrupt  : Enable ready interrupt
 */
typedef struct
{
    EEPROM_Mode_t mode;
    uint8 programmingMode;
    uint8 enableInterrupt;
} EEPROM_Config_t;

/*---------------------------------------------------------------------------------*
 *                              FUNCTION PROTOTYPES                                *
 *---------------------------------------------------------------------------------*/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_init
 *
 * [FUNCTION DESCRIPTION]: Initialize EEPROM module with specified configuration
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: const EEPROM_Config_t* config - pointer to configuration
 *           [out]: none
 *
 * [return]: none
 *
 *---------------------------------------------------------------------------------*/
void EEPROM_init(const EEPROM_Config_t* config);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeByte
 *
 * [FUNCTION DESCRIPTION]: Write a single byte to EEPROM at specified address
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address (0-1023)
 *                 uint8 data - byte to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeByte(uint16 address, uint8 data);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readByte
 *
 * [FUNCTION DESCRIPTION]: Read a single byte from EEPROM at specified address
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address (0-1023)
 *           [out]: none
 *
 * [return]: uint8 - byte read from EEPROM
 *
 *---------------------------------------------------------------------------------*/
uint8 EEPROM_readByte(uint16 address);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeBlock
 *
 * [FUNCTION DESCRIPTION]: Write a block of data to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 const uint8* data - pointer to data buffer
 *                 uint16 length - number of bytes to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeBlock(uint16 startAddress, const uint8* data, uint16 length);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readBlock
 *
 * [FUNCTION DESCRIPTION]: Read a block of data from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 uint16 length - number of bytes to read
 *           [out]: uint8* data - pointer to buffer for read data
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_readBlock(uint16 startAddress, uint8* data, uint16 length);

/*[6]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeString
 *
 * [FUNCTION DESCRIPTION]: Write a null-terminated string to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 const char* str - pointer to string
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeString(uint16 startAddress, const char* str);

/*[7]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readString
 *
 * [FUNCTION DESCRIPTION]: Read a null-terminated string from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 startAddress - starting EEPROM address
 *                 uint16 maxLength - maximum string length (including null)
 *           [out]: char* str - pointer to buffer for string
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_readString(uint16 startAddress, char* str, uint16 maxLength);

/*[8]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeInteger
 *
 * [FUNCTION DESCRIPTION]: Write an integer (16-bit or 32-bit) to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 uint32 value - integer value to write
 *                 uint8 size - size in bytes (2 or 4)
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeInteger(uint16 address, uint32 value, uint8 size);

/*[9]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readInteger
 *
 * [FUNCTION DESCRIPTION]: Read an integer (16-bit or 32-bit) from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 uint8 size - size in bytes (2 or 4)
 *           [out]: none
 *
 * [return]: uint32 - integer value read
 *
 *---------------------------------------------------------------------------------*/
uint32 EEPROM_readInteger(uint16 address, uint8 size);

/*[10]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_erase
 *
 * [FUNCTION DESCRIPTION]: Erase EEPROM by writing 0xFF to all locations
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_erase(void);

/*[11]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_isReady
 *
 * [FUNCTION DESCRIPTION]: Check if EEPROM is ready for operation
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - 1 if ready, 0 if busy
 *
 *---------------------------------------------------------------------------------*/
uint8 EEPROM_isReady(void);

/*[12]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_getStatus
 *
 * [FUNCTION DESCRIPTION]: Get current EEPROM status
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: EEPROM_Status_t - current status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Status_t EEPROM_getStatus(void);

/*[13]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_getLastError
 *
 * [FUNCTION DESCRIPTION]: Get last error that occurred
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - last error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_getLastError(void);

/*[14]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_clearError
 *
 * [FUNCTION DESCRIPTION]: Clear error flag
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
void EEPROM_clearError(void);

/*[15]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_validateAddress
 *
 * [FUNCTION DESCRIPTION]: Validate if address is within EEPROM range
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - address to validate
 *           [out]: none
 *
 * [return]: uint8 - 1 if valid, 0 if invalid
 *
 *---------------------------------------------------------------------------------*/
uint8 EEPROM_validateAddress(uint16 address);
/*[16]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeFloat
 *
 * [FUNCTION DESCRIPTION]: Write a float (32-bit) to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 float value - float value to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeFloat(uint16 address, float value);

/*[17]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readFloat
 *
 * [FUNCTION DESCRIPTION]: Read a float (32-bit) from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *           [out]: none
 *
 * [return]: float - float value read
 *
 *---------------------------------------------------------------------------------*/
float EEPROM_readFloat(uint16 address);

/*[18]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_writeDouble
 *
 * [FUNCTION DESCRIPTION]: Write a double (64-bit) to EEPROM
 *
 * [SYNCHRONIZATION]: sync (polling) / async (interrupt)
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *                 double value - double value to write
 *           [out]: none
 *
 * [return]: EEPROM_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
EEPROM_Error_t EEPROM_writeDouble(uint16 address, double value);

/*[19]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: EEPROM_readDouble
 *
 * [FUNCTION DESCRIPTION]: Read a double (64-bit) from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint16 address - EEPROM address
 *           [out]: none
 *
 * [return]: double - double value read
 *
 *---------------------------------------------------------------------------------*/
double EEPROM_readDouble(uint16 address);

#endif /* EEPROM_H_ */
>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
