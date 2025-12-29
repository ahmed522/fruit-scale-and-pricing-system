/***********************************************************************************
 *                                                                                 *
 * [MODULE]: Application Data Manager                                             *
 *                                                                                 *
 * [FILE NAME]: appdata.c                                                          *
 *                                                                                 *
 ***********************************************************************************/

#include "app_data.h"
#include <string.h>

/*---------------------------------------------------------------------------------*
 *                              GLOBAL VARIABLES                                   *
 *---------------------------------------------------------------------------------*/
static const float32 g_defaultItemsPrices[APPDATA_NUM_ITEMS] = {APPDATA_DEFAULT_ITEM1_PRICE,
																APPDATA_DEFAULT_ITEM2_PRICE,
																APPDATA_DEFAULT_ITEM3_PRICE,
																APPDATA_DEFAULT_ITEM4_PRICE,
																APPDATA_DEFAULT_ITEM5_PRICE};

static AppData_Error_t g_lastError = APPDATA_NO_ERROR;

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION PROTOTYPES                           *
 *---------------------------------------------------------------------------------*/

/*[1]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_convertEepromError
 *
 * [FUNCTION DESCRIPTION]: Convert EEPROM HAL layer error to Application Data layer error
 *                         Updates global error flag and returns mapped error code
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Non-Reentrant
 *
 * [Params]: [in]: EEPROM_Error_t eepromError - EEPROM layer error code
 *           [out]: none
 *
 * [return]: AppData_Error_t - corresponding application data error code
 *
 *---------------------------------------------------------------------------------*/
static AppData_Error_t AppData_convertEepromError(EEPROM_Error_t eepromError);

/*[2]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_getItemAddress
 *
 * [FUNCTION DESCRIPTION]: Get EEPROM address for a specific item index (1-5)
 *                         Maps item number to corresponding EEPROM memory address
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint8 itemIndex - item index (1-5)
 *           [out]: none
 *
 * [return]: uint16 - EEPROM address for item, or 0 if invalid index
 *
 *---------------------------------------------------------------------------------*/
static uint16 AppData_getItemAddress(uint8 itemIndex);

/*[3]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_getItemNameAddress
 *
 * [FUNCTION DESCRIPTION]: Get EEPROM address for a specific item name (1-5)
 *                         Maps item number to corresponding item name storage address
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: uint8 itemIndex - item index (1-5)
 *           [out]: none
 *
 * [return]: uint16 - EEPROM address for item name, or 0 if invalid index
 *
 *---------------------------------------------------------------------------------*/
static uint16 AppData_getItemNameAddress(uint8 itemIndex);

/*[4]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_loadPassword
 *
 * [FUNCTION DESCRIPTION]: Load password string from EEPROM
 *
 * [SYNCHRONIZATION]: sync
 *
 * [REENTARNCY]: Reentrant
 *
 * [Params]: [in]: none
 *           [out]: char* password - buffer for password (min 16 bytes)
 *
 * [return]: AppData_Error_t - error status
 *
 *---------------------------------------------------------------------------------*/
static AppData_Error_t AppData_loadPassword(char* password);

/*[5]-------------------------------------------------------------------------------
 *
 * [FUNCTION NAME]: AppData_initializeDefaults
 *
 * [FUNCTION DESCRIPTION]: Initialize all application data with default values
 *                         Password = "0000", All prices = 0.0, Income = 0.0
 *                         Use on first-time system startup
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
static AppData_Error_t AppData_initializeDefaults(void);

/*---------------------------------------------------------------------------------*
 *                              FUNCTION DEFINITIONS                               *
 *---------------------------------------------------------------------------------*/

void AppData_init(void)
{
    /* Clear error status */
    g_lastError = APPDATA_NO_ERROR;

    /* Note: EEPROM_init() should already be called by application */

    /* First-time initialization */
    if(AppData_isFirstTime())
    {
        /* Initialize with default values */
    	AppData_initializeDefaults();
    }
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_savePassword(const char* password)
{
    EEPROM_Error_t eepromStatus;

    /* Validate parameter */
    if(password == NULL)
    {
        g_lastError = APPDATA_NULL_POINTER;
        return APPDATA_NULL_POINTER;
    }

    /* Validate length */
    if(strlen(password) > APPDATA_MAX_PASSWORD_LENGTH)
    {
        g_lastError = APPDATA_STRING_TOO_LONG;
        return APPDATA_STRING_TOO_LONG;
    }

    /* Write to EEPROM using HAL layer */
    eepromStatus = EEPROM_writeString(APPDATA_PASSWORD_ADDRESS, password);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/


uint8 AppData_verifyPassword(const char* enteredPassword)
{
    char storedPassword[APPDATA_PASSWORD_SIZE];

    /* Validate parameter */
    if(enteredPassword == NULL)
    {
        return 0;
    }

    /* Load stored password */
    if(AppData_loadPassword(storedPassword) != APPDATA_NO_ERROR)
    {
        return 0;
    }

    /* Compare passwords */
    return (strcmp(enteredPassword, storedPassword) == 0) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_saveItemPrice(uint8 itemIndex, float price)
{
    uint16 address;
    EEPROM_Error_t eepromStatus;

    /* Validate index */
    if(itemIndex < 1 || itemIndex > APPDATA_NUM_ITEMS)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return APPDATA_INVALID_INDEX;
    }

    /* Validate price range */
    if(price < APPDATA_MIN_PRICE || price > APPDATA_MAX_PRICE)
    {
        g_lastError = APPDATA_INVALID_PRICE;
        return APPDATA_INVALID_PRICE;
    }

    /* Get item address */
    address = AppData_getItemAddress(itemIndex);

    /* Validate address was found */
    if(address == 0)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return APPDATA_INVALID_INDEX;
    }

    /* Write to EEPROM using HAL layer */
    eepromStatus = EEPROM_writeFloat(address, price);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/

float AppData_loadItemPrice(uint8 itemIndex)
{
    uint16 address;

    /* Validate index */
    if(itemIndex < 1 || itemIndex > APPDATA_NUM_ITEMS)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return 0.0f;
    }

    /* Get item address */
    address = AppData_getItemAddress(itemIndex);

    /* Validate address was found */
    if(address == 0)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return 0.0f;
    }

    /* Read from EEPROM using HAL layer */
    return EEPROM_readFloat(address);
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_saveTotalIncome(double totalIncome)
{
    EEPROM_Error_t eepromStatus;

    /* Validate total income (must be non-negative) */
    if(totalIncome < 0.0)
    {
        g_lastError = APPDATA_INVALID_PRICE;
        return APPDATA_INVALID_PRICE;
    }

    /* Write to EEPROM using HAL layer */
    eepromStatus = EEPROM_writeDouble(APPDATA_TOTAL_INCOME_ADDRESS, totalIncome);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/

double AppData_loadTotalIncome(void)
{
    /* Read from EEPROM using HAL layer */
    return EEPROM_readDouble(APPDATA_TOTAL_INCOME_ADDRESS);
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_addToTotalIncome(double amount)
{
    double currentIncome;
    double newIncome;

    /* Validate amount (can be positive or negative for refunds) */
    /* Optional: Add validation if you want to restrict negative values */

    /* Read current total income */
    currentIncome = AppData_loadTotalIncome();

    /* Calculate new total income */
    newIncome = currentIncome + amount;

    /* Validate result is non-negative */
    if(newIncome < 0.0)
    {
        g_lastError = APPDATA_INVALID_PRICE;
        return APPDATA_INVALID_PRICE;
    }

    /* Save new total income */
    return AppData_saveTotalIncome(newIncome);
}
/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_getLastError(void)
{
    return g_lastError;
}

/*---------------------------------------------------------------------------------*/

void AppData_clearError(void)
{
    g_lastError = APPDATA_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_saveItemName(uint8 itemIndex, const char* itemName)
{
    uint16 address;
    EEPROM_Error_t eepromStatus;

    /* Validate index */
    if(itemIndex < 1 || itemIndex > APPDATA_NUM_ITEMS)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return APPDATA_INVALID_INDEX;
    }

    /* Validate parameter */
    if(itemName == NULL)
    {
        g_lastError = APPDATA_NULL_POINTER;
        return APPDATA_NULL_POINTER;
    }

    /* Validate length */
    if(strlen(itemName) > APPDATA_MAX_ITEM_NAME_LENGTH)
    {
        g_lastError = APPDATA_STRING_TOO_LONG;
        return APPDATA_STRING_TOO_LONG;
    }

    /* Get item name address */
    address = AppData_getItemNameAddress(itemIndex);

    if(address == 0)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return APPDATA_INVALID_INDEX;
    }

    /* Write to EEPROM using HAL layer */
    eepromStatus = EEPROM_writeString(address, itemName);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_loadItemName(uint8 itemIndex, char* itemName)
{
    uint16 address;
    EEPROM_Error_t eepromStatus;

    /* Validate index */
    if(itemIndex < 1 || itemIndex > APPDATA_NUM_ITEMS)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return APPDATA_INVALID_INDEX;
    }

    /* Validate parameter */
    if(itemName == NULL)
    {
        g_lastError = APPDATA_NULL_POINTER;
        return APPDATA_NULL_POINTER;
    }

    /* Get item name address */
    address = AppData_getItemNameAddress(itemIndex);

    if(address == 0)
    {
        g_lastError = APPDATA_INVALID_INDEX;
        return APPDATA_INVALID_INDEX;
    }

    /* Read from EEPROM using HAL layer */
    eepromStatus = EEPROM_readString(address, itemName, APPDATA_ITEM_NAME_SIZE);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/

uint8 AppData_isFirstTime(void)
{
    uint8 flag;

    /* Read first-time flag from EEPROM */
    flag = EEPROM_readByte(APPDATA_FIRST_TIME_FLAG_ADDRESS);

    /* Check if EEPROM is in erased state (0xFF) or uninitialized */
    if(flag != APPDATA_INITIALIZED_VALUE)
    {
        return 1;  /* First time - needs initialization */
    }
    else
    {
        return 0;  /* Already initialized */
    }
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_markAsInitialized(void)
{
    EEPROM_Error_t eepromStatus;

    /* Write initialized flag to EEPROM */
    eepromStatus = EEPROM_writeByte(APPDATA_FIRST_TIME_FLAG_ADDRESS,
                                     APPDATA_INITIALIZED_VALUE);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------
 * 							HX711 CALIBRATION FUNCTIONS
 *---------------------------------------------------------------------------------*/

AppData_Error_t AppData_saveCalibration(double scale, int32_t offset)
{
    EEPROM_Error_t eepromStatus;

    /* Save scale factor */
    eepromStatus = EEPROM_writeDouble(APPDATA_HX711_SCALE_ADDRESS, scale);
    if(eepromStatus != EEPROM_NO_ERROR) {
        return AppData_convertEepromError(eepromStatus);
    }

    /* Save offset */
    eepromStatus = EEPROM_writeInteger(APPDATA_HX711_OFFSET_ADDRESS, offset, APPDATA_HX711_OFFSET_SIZE);
    if(eepromStatus != EEPROM_NO_ERROR) {
        return AppData_convertEepromError(eepromStatus);
    }

    return AppData_markAsCalibrated();
;
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_loadCalibration(double* scale, int32_t* offset)
{
    /* Validate parameters */
    if(scale == NULL || offset == NULL) {
        g_lastError = APPDATA_NULL_POINTER;
        return APPDATA_NULL_POINTER;
    }

    /* Load scale factor */
    *scale = EEPROM_readDouble(APPDATA_HX711_SCALE_ADDRESS);

    /* Load offset */
    *offset = EEPROM_readInteger(APPDATA_HX711_OFFSET_ADDRESS, APPDATA_HX711_OFFSET_SIZE);

    return APPDATA_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_saveHX711Scale(double scale)
{
    EEPROM_Error_t eepromStatus;

    /* Write scale to EEPROM using HAL layer */
    eepromStatus = EEPROM_writeDouble(APPDATA_HX711_SCALE_ADDRESS, scale);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/

double AppData_loadHX711Scale(void)
{
    /* Read scale from EEPROM using HAL layer */
    return EEPROM_readDouble(APPDATA_HX711_SCALE_ADDRESS);
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_saveHX711Offset(int32_t offset)
{
    EEPROM_Error_t eepromStatus;

    /* Write offset to EEPROM using HAL layer */
    eepromStatus = EEPROM_writeInteger(APPDATA_HX711_OFFSET_ADDRESS, offset,APPDATA_HX711_OFFSET_SIZE);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/

int32_t AppData_loadHX711Offset(void)
{
    /* Read offset from EEPROM using HAL layer */
    return EEPROM_readInteger(APPDATA_HX711_OFFSET_ADDRESS, APPDATA_HX711_OFFSET_SIZE);
}

/*---------------------------------------------------------------------------------*/

uint8 AppData_isCalibrated(void)
{
    uint8 flag;

    /* Read calibration flag from EEPROM */
    flag = EEPROM_readByte(APPDATA_HX711_CALIBRATED_FLAG_ADDRESS);

    /* Check if HX711 has been calibrated */
    if(flag == APPDATA_HX711_CALIBRATED_VALUE) {
        return 1;  /* Calibrated - valid calibration data exists */
    }
    else {
        return 0;  /* Not calibrated - needs calibration */
    }
}

/*---------------------------------------------------------------------------------*/

AppData_Error_t AppData_markAsCalibrated(void)
{
    EEPROM_Error_t eepromStatus;

    /* Write calibrated flag to EEPROM */
    eepromStatus = EEPROM_writeByte(APPDATA_HX711_CALIBRATED_FLAG_ADDRESS,
                                     APPDATA_HX711_CALIBRATED_VALUE);

    return AppData_convertEepromError(eepromStatus);
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*
 *                           PRIVATE FUNCTION DEFINITIONS                          *
 *---------------------------------------------------------------------------------*/

static AppData_Error_t AppData_convertEepromError(EEPROM_Error_t eepromError)
{
    /* Convert EEPROM layer error to application data layer error */
    if(eepromError == EEPROM_NO_ERROR)
    {
        g_lastError = APPDATA_NO_ERROR;
        return APPDATA_NO_ERROR;
    }
    else
    {
        /* Any EEPROM error maps to generic EEPROM error */
        g_lastError = APPDATA_EEPROM_ERROR;
        return APPDATA_EEPROM_ERROR;
    }
}

/*---------------------------------------------------------------------------------*/

static uint16 AppData_getItemAddress(uint8 itemIndex)
{
    /* Return EEPROM address based on item index */
    switch(itemIndex)
    {
        case 1:
            return APPDATA_ITEM1_ADDRESS;
        case 2:
            return APPDATA_ITEM2_ADDRESS;
        case 3:
            return APPDATA_ITEM3_ADDRESS;
        case 4:
            return APPDATA_ITEM4_ADDRESS;
        case 5:
            return APPDATA_ITEM5_ADDRESS;
        default:
            return 0;  /* Invalid index */
    }
}

/*---------------------------------------------------------------------------------*/

static uint16 AppData_getItemNameAddress(uint8 itemIndex)
{
    switch(itemIndex)
    {
        case 1: return APPDATA_ITEM1_NAME_ADDRESS;
        case 2: return APPDATA_ITEM2_NAME_ADDRESS;
        case 3: return APPDATA_ITEM3_NAME_ADDRESS;
        case 4: return APPDATA_ITEM4_NAME_ADDRESS;
        case 5: return APPDATA_ITEM5_NAME_ADDRESS;
        default: return 0;
    }
}

/*---------------------------------------------------------------------------------*/

static AppData_Error_t AppData_loadPassword(char* password)
{
    EEPROM_Error_t eepromStatus;

    /* Validate parameter */
    if(password == NULL)
    {
        g_lastError = APPDATA_NULL_POINTER;
        return APPDATA_NULL_POINTER;
    }

    /* Read from EEPROM using HAL layer */
    eepromStatus = EEPROM_readString(APPDATA_PASSWORD_ADDRESS, password, APPDATA_PASSWORD_SIZE);

    return AppData_convertEepromError(eepromStatus);
}


/*---------------------------------------------------------------------------------*/

static AppData_Error_t AppData_initializeDefaults(void)
{
    uint8 i;
    AppData_Error_t status;

    /* Set default password "0000" */
    status = AppData_savePassword(APPDATA_DEFAULT_PASSWORD);
    if(status != APPDATA_NO_ERROR)
    {
        return status;
    }

    /* Set all item prices to 0.0 */
    for(i = 0; i < APPDATA_NUM_ITEMS; i++)
    {
        status = AppData_saveItemPrice(i, g_defaultItemsPrices[i]);
        if(status != APPDATA_NO_ERROR)
        {
            return status;
        }
    }

    /* Set total income to 0.0 */
    status = AppData_saveTotalIncome(0.0);
    if(status != APPDATA_NO_ERROR)
    {
        return status;
    }

    /* Set default item names */
    status = AppData_saveItemName(1, APPDATA_DEFAULT_ITEM1_NAME);
    if(status != APPDATA_NO_ERROR) return status;

    status = AppData_saveItemName(2, APPDATA_DEFAULT_ITEM2_NAME);
    if(status != APPDATA_NO_ERROR) return status;

    status = AppData_saveItemName(3, APPDATA_DEFAULT_ITEM3_NAME);
    if(status != APPDATA_NO_ERROR) return status;

    status = AppData_saveItemName(4, APPDATA_DEFAULT_ITEM4_NAME);
    if(status != APPDATA_NO_ERROR) return status;

    status = AppData_saveItemName(5, APPDATA_DEFAULT_ITEM5_NAME);
    if(status != APPDATA_NO_ERROR) return status;

    status = AppData_markAsInitialized();	    /* Mark as initialized */
    if(status != APPDATA_NO_ERROR) return status;

    return APPDATA_NO_ERROR;
}

/*---------------------------------------------------------------------------------*/

