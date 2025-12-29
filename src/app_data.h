/***********************************************************************************
 *                                                                                 *
 * [MODULE]: Application Data Manager                                             *
 *                                                                                 *
 * [FILE NAME]: appdata.h                                                          *
 *                                                                                 *
 * [AUTHOR]: Ahmed Abdelaal                                                        *
 *                                                                                 *
 * [DATE]: 25/12/2025                                                              *
 *                                                                                 *
 * [DESCRIPTION]: Application-specific data management layer for EEPROM           *
 *                Handles password, item prices, and total income storage         *
 *                This is the Service Layer / ECU Abstraction Layer               *
 *                                                                                 *
 ***********************************************************************************/

#ifndef APPDATA_H_
#define APPDATA_H_

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "std_types.h"
#include "EEPROM.h"

/*---------------------------------------------------------------------------------*
 *                          APPLICATION MEMORY MAP                                 *
 *---------------------------------------------------------------------------------*/

/*
 * EEPROM Memory Layout for Application Data
 *
 * Address Range    |  Size      | Description
 * ---------------- | ---------- | ---------------------------------
 * 0x0000 - 0x000F  |  16 bytes  | Password (max 15 chars + null)
 * 0x0010 - 0x0013  |  4 bytes   | Item 1 Price (float)
 * 0x0014 - 0x0017  |  4 bytes   | Item 2 Price (float)
 * 0x0018 - 0x001B  |  4 bytes   | Item 3 Price (float)
 * 0x001C - 0x001F  |  4 bytes   | Item 4 Price (float)
 * 0x0020 - 0x0023  |  4 bytes   | Item 5 Price (float)
 * 0x0024 - 0x002B  |  8 bytes   | Total Income (double)
 * 0x002C - 0x003B  |  16 bytes  | Item 1 Name (max 15 chars + null)
 * 0x003C - 0x004B  |  16 bytes  | Item 2 Name (max 15 chars + null)
 * 0x004C - 0x005B  |  16 bytes  | Item 3 Name (max 15 chars + null)
 * 0x005C - 0x006B  |  16 bytes  | Item 4 Name (max 15 chars + null)
 * 0x006C - 0x007B  |  16 bytes  | Item 5 Name (max 15 chars + null)
 * 0x007C - 0x007C  |  1 byte    | First Time Flag (0xAA=initialized)
 * 0x007D - 0x0084  |  8 bytes   | HX711 Scale Factor (double)
 * 0x0085 - 0x0088  |  4 bytes   | HX711 Offset (int32_t)
 *b0x0089 - 0x0089  |  1 byte    | HX711 Calibrated Flag (0x55=calibrated)
 * 0x008A - 0x03FF  |  886 bytes | Reserved for future use
 */

/* Application Memory Addresses */
#define APPDATA_PASSWORD_ADDRESS        0x0000
#define APPDATA_ITEM1_ADDRESS           0x0010
#define APPDATA_ITEM2_ADDRESS           0x0014
#define APPDATA_ITEM3_ADDRESS           0x0018
#define APPDATA_ITEM4_ADDRESS           0x001C
#define APPDATA_ITEM5_ADDRESS           0x0020
#define APPDATA_TOTAL_INCOME_ADDRESS    0x0024
#define APPDATA_ITEM1_NAME_ADDRESS      0x002C
#define APPDATA_ITEM2_NAME_ADDRESS      0x003C
#define APPDATA_ITEM3_NAME_ADDRESS      0x004C
#define APPDATA_ITEM4_NAME_ADDRESS      0x005C
#define APPDATA_ITEM5_NAME_ADDRESS      0x006C
#define APPDATA_FIRST_TIME_FLAG_ADDRESS 0x007C
#define APPDATA_HX711_SCALE_ADDRESS     0x007D
#define APPDATA_HX711_OFFSET_ADDRESS    0x0085
#define APPDATA_HX711_CALIBRATED_FLAG_ADDRESS  0x0089
/* Application Data Sizes */
#define APPDATA_PASSWORD_SIZE           16      /* bytes */
#define APPDATA_ITEM_PRICE_SIZE         4       /* bytes (float) */
#define APPDATA_TOTAL_INCOME_SIZE       8       /* bytes (double) */
#define APPDATA_ITEM_NAME_SIZE          16      /* bytes (string) */
#define APPDATA_HX711_SCALE_SIZE        8     /* bytes (double) */
#define APPDATA_HX711_OFFSET_SIZE       4     /* bytes (int32_t) */

/* Application Default Data */
#define APPDATA_DEFAULT_PASSWORD        "0000"
#define APPDATA_DEFAULT_ITEM1_PRICE     10.0f
#define APPDATA_DEFAULT_ITEM2_PRICE     20.0f
#define APPDATA_DEFAULT_ITEM3_PRICE     35.0f
#define APPDATA_DEFAULT_ITEM4_PRICE     50.0f
#define APPDATA_DEFAULT_ITEM5_PRICE     70.0f
#define APPDATA_INITIALIZED_VALUE       0xAA	 /* System has been initialized */
#define APPDATA_DEFAULT_HX711_SCALE     10000.0  /* Must be calibrated */
#define APPDATA_DEFAULT_HX711_OFFSET    8000000  /* Must be calibrated */
#define APPDATA_HX711_CALIBRATED_VALUE  0x55     /* HX711 has been calibrated */

/* Default Item Names */
#define APPDATA_DEFAULT_ITEM1_NAME      "Apple"
#define APPDATA_DEFAULT_ITEM2_NAME      "Orange"
#define APPDATA_DEFAULT_ITEM3_NAME      "Mango"
#define APPDATA_DEFAULT_ITEM4_NAME      "Strawberry"
#define APPDATA_DEFAULT_ITEM5_NAME      "Banana"

/* First Free Address after application data */
#define APPDATA_END_ADDRESS                0x008A
#define APPDATA_USER_FREE_START            0x008A

/* Validation Constants */
#define APPDATA_MAX_PASSWORD_LENGTH     15
#define APPDATA_NUM_ITEMS               5
#define APPDATA_MAX_PRICE               999999.99f  /* Maximum price value */
#define APPDATA_MIN_PRICE               0.0f        /* Minimum price value */
#define APPDATA_MAX_ITEM_NAME_LENGTH    15


/*---------------------------------------------------------------------------------*
 *                                     ENUMS                                       *
 *---------------------------------------------------------------------------------*/

/*
 * Description: Enumeration for application data error types
 *
 * APPDATA_NO_ERROR              : No error occurred - operation successful
 * APPDATA_INVALID_INDEX         : Item index out of range (must be 1-5)
 * APPDATA_STRING_TOO_LONG       : String exceeds maximum length
 * APPDATA_NULL_POINTER          : Null pointer passed as parameter
 * APPDATA_EEPROM_ERROR          : Underlying EEPROM operation failed
 * APPDATA_INVALID_PRICE         : Price value out of valid range
 * APPDATA_INVALID_INCOME        : Income value is negative or invalid
 * APPDATA_INVALID_NAME          : Item name is invalid or empty
 * APPDATA_PASSWORD_TOO_SHORT    : Password is less than minimum length
 * APPDATA_PASSWORD_INVALID_CHAR : Password contains invalid characters
 * APPDATA_INVALID_SCALE         : HX711 scale factor is invalid or zero
 * APPDATA_INVALID_OFFSET        : HX711 offset is out of reasonable range
 * APPDATA_NOT_INITIALIZED       : System not initialized - call AppData_init()
 * APPDATA_NOT_CALIBRATED        : HX711 not calibrated - calibration required
 * APPDATA_CALIBRATION_FAILED    : HX711 calibration process failed
 * APPDATA_READ_ERROR            : Error reading from EEPROM
 * APPDATA_WRITE_ERROR           : Error writing to EEPROM
 * APPDATA_VERIFICATION_FAILED   : Data verification failed after write
 * APPDATA_BUFFER_OVERFLOW       : Buffer size insufficient for operation
 * APPDATA_ADDRESS_OUT_OF_RANGE  : EEPROM address is out of valid range
 * APPDATA_TIMEOUT               : Operation timed out
 * APPDATA_BUSY                  : EEPROM is busy, operation cannot proceed
 * APPDATA_UNKNOWN_ERROR         : Unknown or undefined error occurred
 */
typedef enum
{
    /* Success */
    APPDATA_NO_ERROR = 0,              /* Operation successful */

    /* Parameter Validation Errors (1-9) */
    APPDATA_INVALID_INDEX,             /* Item index out of range (1-5) */
    APPDATA_NULL_POINTER,              /* Null pointer passed */
    APPDATA_BUFFER_OVERFLOW,           /* Buffer too small */
    APPDATA_ADDRESS_OUT_OF_RANGE,      /* Invalid EEPROM address */

    /* String Validation Errors (10-14) */
    APPDATA_STRING_TOO_LONG,           /* String exceeds max length */
    APPDATA_INVALID_NAME,              /* Invalid item name */

    /* Password Errors (15-19) */
    APPDATA_PASSWORD_TOO_SHORT,        /* Password < 4 characters */
    APPDATA_PASSWORD_INVALID_CHAR,     /* Non-numeric character in password */
    APPDATA_VERIFICATION_FAILED,       /* Password verification failed */

    /* Value Range Errors (20-29) */
    APPDATA_INVALID_PRICE,             /* Price out of range */
    APPDATA_INVALID_INCOME,            /* Income is negative */
    APPDATA_INVALID_SCALE,             /* HX711 scale invalid/zero */
    APPDATA_INVALID_OFFSET,            /* HX711 offset out of range */

    /* State Errors (30-39) */
    APPDATA_NOT_INITIALIZED,           /* AppData_init() not called */
    APPDATA_NOT_CALIBRATED,            /* HX711 not calibrated */
    APPDATA_CALIBRATION_FAILED,        /* Calibration process failed */

    /* EEPROM Operation Errors (40-49) */
    APPDATA_EEPROM_ERROR,              /* Generic EEPROM error */
    APPDATA_READ_ERROR,                /* EEPROM read failed */
    APPDATA_WRITE_ERROR,               /* EEPROM write failed */
    APPDATA_TIMEOUT,                   /* Operation timeout */
    APPDATA_BUSY,                      /* EEPROM busy */

    /* General Errors (50+) */
    APPDATA_UNKNOWN_ERROR              /* Unknown error */

} AppData_Error_t;
/*---------------------------------------------------------------------------------*
 *                              FUNCTION PROTOTYPES                                *
 *---------------------------------------------------------------------------------*/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_init
 *
 * [FUNCTION DESCRIPTION]: Initialize application data layer
 *                         Must be called after EEPROM_init()
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
void AppData_init(void);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_savePassword
 *
 * [FUNCTION DESCRIPTION]: Save password string to EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: const char* password - password string (max 15 chars)
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_savePassword(const char* password);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_verifyPassword
 *
 * [FUNCTION DESCRIPTION]: Verify entered password against stored password
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: const char* enteredPassword - password to verify
 *           [out]: none
 *
 * [return]: uint8 - 1 if password matches, 0 if not
 *
 *---------------------------------------------------------------------------------*/
uint8 AppData_verifyPassword(const char* enteredPassword);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_saveItemPrice
 *
 * [FUNCTION DESCRIPTION]: Save item price to EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint8 itemIndex - item index (1-5)
 *                 float price - item price value (0.0 to 999999.99)
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_saveItemPrice(uint8 itemIndex, float price);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_loadItemPrice
 *
 * [FUNCTION DESCRIPTION]: Load item price from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint8 itemIndex - item index (1-5)
 *           [out]: none
 *
 * [return]: float - item price value (0.0 if error)
 *
 *---------------------------------------------------------------------------------*/
float AppData_loadItemPrice(uint8 itemIndex);

/*[6]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_saveTotalIncome
 *
 * [FUNCTION DESCRIPTION]: Save total income to EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: double totalIncome - total income value
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_saveTotalIncome(double totalIncome);

/*[7]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_loadTotalIncome
 *
 * [FUNCTION DESCRIPTION]: Load total income from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: double - total income value
 *
 *---------------------------------------------------------------------------------*/
double AppData_loadTotalIncome(void);

/*[8]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_getLastError
 *
 * [FUNCTION DESCRIPTION]: Get last error that occurred in application data layer
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: AppData_Error_t - last error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_getLastError(void);

/*[9]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_clearError
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
void AppData_clearError(void);

/*[10]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_addToTotalIncome
 *
 * [FUNCTION DESCRIPTION]: Add amount to current total income and save the result
 *                         Performs read-modify-write operation atomically
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: double amount - amount to add to total income
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_addToTotalIncome(double amount);

/*[11]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_saveItemName
 *
 * [FUNCTION DESCRIPTION]: Save item name string to EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: uint8 itemIndex - item index (1-5)
 *                 const char* itemName - item name string (max 15 chars)
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_saveItemName(uint8 itemIndex, const char* itemName);

/*[12]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_loadItemName
 *
 * [FUNCTION DESCRIPTION]: Load item name string from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint8 itemIndex - item index (1-5)
 *           [out]: char* itemName - buffer for item name (min 16 bytes)
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_loadItemName(uint8 itemIndex, char* itemName);

/*[13]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_isFirstTime
 *
 * [FUNCTION DESCRIPTION]: Check if this is the first time the system is running
 *                         Reads the first-time flag from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - 1 if first time (uninitialized), 0 if already initialized
 *
 *---------------------------------------------------------------------------------*/
uint8 AppData_isFirstTime(void);

/*[14]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_markAsInitialized
 *
 * [FUNCTION DESCRIPTION]: Mark the system as initialized by writing flag to EEPROM
 *                         Should be called after AppData_initializeDefaults()
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_markAsInitialized(void);

/*[15]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_saveCalibration
 *
 * [FUNCTION DESCRIPTION]: Save HX711 calibration data (scale and offset) to EEPROM
 *                         Used to persist scale calibration across power cycles
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: double scale - HX711 scale factor from calibration
 *                 int32_t offset - HX711 tare offset value
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_saveCalibration(double scale, int32_t offset);

/*[16]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_loadCalibration
 *
 * [FUNCTION DESCRIPTION]: Load HX711 calibration data from EEPROM
 *                         Call during initialization to restore calibration
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: double* scale - pointer to store scale factor
 *                  int32_t* offset - pointer to store offset value
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_loadCalibration(double* scale, int32_t* offset);

/*[17]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_saveHX711Scale
 *
 * [FUNCTION DESCRIPTION]: Save only HX711 scale factor to EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: double scale - HX711 scale factor
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_saveHX711Scale(double scale);

/*[18]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_loadHX711Scale
 *
 * [FUNCTION DESCRIPTION]: Load HX711 scale factor from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: double - scale factor value
 *
 *---------------------------------------------------------------------------------*/
double AppData_loadHX711Scale(void);

/*[1]9------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_saveHX711Offset
 *
 * [FUNCTION DESCRIPTION]: Save only HX711 offset to EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: int32_t offset - HX711 tare offset
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_saveHX711Offset(int32_t offset);

/*[20]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_loadHX711Offset
 *
 * [FUNCTION DESCRIPTION]: Load HX711 offset from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: int32_t - offset value
 *
 *---------------------------------------------------------------------------------*/
int32_t AppData_loadHX711Offset(void);

/*[21]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_isCalibrated
 *
 * [FUNCTION DESCRIPTION]: Check if HX711 has been calibrated
 *                         Reads calibration flag from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: uint8 - 1 if calibrated (valid calibration exists),
 *                   0 if not calibrated (needs calibration)
 *
 *---------------------------------------------------------------------------------*/
uint8 AppData_isCalibrated(void);

/*[22]------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_markAsCalibrated
 *
 * [FUNCTION DESCRIPTION]: Mark HX711 as calibrated by writing flag to EEPROM
 *                         Should be called after successful calibration
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: none
 *           [out]: none
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
AppData_Error_t AppData_markAsCalibrated(void);
#endif /* APPDATA_H_ */
