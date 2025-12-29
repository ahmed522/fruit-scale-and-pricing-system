<<<<<<< HEAD
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: Application                                                           *
 *                                                                                 *
 * [FILE NAME]: main.c                                                             *
 *                                                                                 *
 * [DESCRIPTION]: Fruit weighing and pricing system with Admin/User roles         *
 *                                                                                 *
 ***********************************************************************************/

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "eeprom.h"
#include "app_data.h"
#include "keypad.h"
#include "lcd.h"
#include "hx711.h"

/*---------------------------------------------------------------------------------*
 *                              GLOBAL CONST MACROS                                *
 *---------------------------------------------------------------------------------*/
#define MAX_PASSWORD_LENGTH 6
#define MAX_PRICE_DIGITS 8
#define DECIMAL_PLACES 3

/*---------------------------------------------------------------------------------*
 *                                     ENUMS                                       *
 *---------------------------------------------------------------------------------*/

typedef enum
{
    STATE_ROLE_SELECT = 0,
    STATE_ADMIN_LOGIN,
    STATE_ADMIN_MENU,
    STATE_UPDATE_PRICE,
    STATE_UPDATE_PASSWORD,
    STATE_VIEW_INCOME,
    STATE_CALIBRATE_SCALE,
    STATE_USER_BROWSE_ITEMS,
    STATE_USER_WEIGH_ITEM,
    STATE_USER_CHECKOUT,
    STATE_LOGOUT
} AppState_t;

typedef enum
{
    ROLE_NONE = 0,
    ROLE_ADMIN,
    ROLE_USER
} UserRole_t;

/*---------------------------------------------------------------------------------*
 *                              GLOBAL VARIABLES                                   *
 *---------------------------------------------------------------------------------*/
static AppState_t g_currentState = STATE_ROLE_SELECT;
static UserRole_t g_currentRole = ROLE_NONE;
static uint8 g_isAuthenticated = 0;
static uint8 g_currentItemIndex = 1;
static double g_sessionTotal = 0.0;

/*---------------------------------------------------------------------------------*
 *                              FUNCTION PROTOTYPES                                *
 *---------------------------------------------------------------------------------*/

/* Initialization */
void App_init(void);
void App_displayWelcome(void);

/* Role Selection */
void App_handleRoleSelect(void);

/* Admin Functions */
void App_handleAdminLogin(void);
void App_displayAdminMenu(void);
void App_handleAdminMenu(void);
void App_handleUpdatePrice(void);
void App_handleUpdatePassword(void);
void App_handleViewIncome(void);
void performScaleCalibration(void);
void App_handleCalibrateScale(void);

/* User Functions */
void App_handleUserBrowseItems(void);
void App_handleUserWeighItem(void);
void App_handleUserCheckout(void);

/* Weight Measurement (Simulated) */
float getWeight(void);

/* Input Functions */
void App_getPasswordInput(char *password, uint8 maxLength);
void App_getNumericInput(char *buffer, uint8 maxLength);
float App_parsePrice(const char *priceString);

/* Display Helpers */
void App_displayFloat(float value);
void App_displayDouble(double value);
void App_showMessage(const char *line1, const char *line2, uint16 delayMs);
void App_showError(const char *message);
void App_showSuccess(const char *message);

/* Validation */
uint8 App_validatePrice(float price);
uint8 App_validatePassword(const char *password);
uint8 App_validateItemIndex(uint8 index);

/*---------------------------------------------------------------------------------*
 *                              FUNCTION DEFINITIONS                               *
 *---------------------------------------------------------------------------------*/

int main(void)
{
    App_init();

    while (1)
    {
        switch (g_currentState)
        {
        case STATE_ROLE_SELECT:
            App_handleRoleSelect();
            break;

        case STATE_ADMIN_LOGIN:
            App_handleAdminLogin();
            break;

        case STATE_ADMIN_MENU:
            App_handleAdminMenu();
            break;

        case STATE_UPDATE_PRICE:
            App_handleUpdatePrice();
            break;

        case STATE_UPDATE_PASSWORD:
            App_handleUpdatePassword();
            break;

        case STATE_VIEW_INCOME:
            App_handleViewIncome();
            break;
        case STATE_CALIBRATE_SCALE:
            App_handleCalibrateScale();
            break;
        case STATE_USER_BROWSE_ITEMS:
            App_handleUserBrowseItems();
            break;

        case STATE_USER_WEIGH_ITEM:
            App_handleUserWeighItem();
            break;

        case STATE_USER_CHECKOUT:
            App_handleUserCheckout();
            break;

        case STATE_LOGOUT:
            g_isAuthenticated = 0;
            g_currentRole = ROLE_NONE;
            g_sessionTotal = 0.0;
            g_currentState = STATE_ROLE_SELECT;
            App_showMessage("Logged Out", "Thank you!", 2000);
            break;

        default:
            g_currentState = STATE_ROLE_SELECT;
            break;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------------*/

void App_init(void)
{
    double saved_scale;
    int32_t saved_offset;
    EEPROM_Config_t eepromConfig;

    LCD_init();
    KEYPAD_init();
    eepromConfig.mode = EEPROM_POLLING_MODE;
    eepromConfig.programmingMode = EEPROM_ERASE_AND_WRITE_MODE;
    eepromConfig.enableInterrupt = 0;
    EEPROM_init(&eepromConfig);

    AppData_init();
    App_displayWelcome();

    if (!AppData_isCalibrated())
    {
        performScaleCalibration();
    }

    if (AppData_loadCalibration(&saved_scale, &saved_offset) == APPDATA_NO_ERROR)
    {
        hx711_init(HX711_GAINCHANNELA128, saved_scale, saved_offset);
        /* Initialize HX711 with saved calibration */
        App_showMessage("Calibration", "Loaded!", 500);
    }
}

/*---------------------------------------------------------------------------------*/

void App_displayWelcome(void)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Fruit Weighing");
    LCD_displayStringRowColumn(1, 0, "   System v1.0");
    _delay_ms(2000);
}

/*---------------------------------------------------------------------------------*/

void App_handleRoleSelect(void)
{
    uint8 key;

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Select Role:");
    LCD_displayStringRowColumn(1, 0, "1:Admin  2:User");

    key = KEYPAD_getPressedKey();

    if (key == '1')
    {
        g_currentRole = ROLE_ADMIN;
        g_currentState = STATE_ADMIN_LOGIN;
    }
    else if (key == '2')
    {
        g_currentRole = ROLE_USER;
        g_currentItemIndex = 1;
        g_sessionTotal = 0.0;
        g_currentState = STATE_USER_BROWSE_ITEMS;
    }
    else
    {
        App_showError("Invalid Role!");
    }
}

/*---------------------------------------------------------------------------------*/

void App_handleAdminLogin(void)
{
    char enteredPassword[MAX_PASSWORD_LENGTH + 1];
    uint8 attempts = 3;

    while (attempts > 0 && !g_isAuthenticated)
    {
        LCD_clearScreen();
        LCD_displayStringRowColumn(0, 0, "Admin Login");
        LCD_displayStringRowColumn(1, 0, "Attempts:");
        LCD_displayInteger(attempts);
        _delay_ms(1500);

        LCD_clearScreen();
        LCD_displayStringRowColumn(0, 0, "Password:");
        LCD_goToRowColumn(1, 0);

        App_getPasswordInput(enteredPassword, MAX_PASSWORD_LENGTH);

        if (AppData_verifyPassword(enteredPassword))
        {
            g_isAuthenticated = 1;
            g_currentState = STATE_ADMIN_MENU;
            App_showSuccess("Access Granted!");
        }
        else
        {
            attempts--;
            if (attempts > 0)
            {
                App_showError("Wrong Password!");
            }
            else
            {
                App_showMessage("System Locked!", "Please wait...", 5000);
                g_currentState = STATE_ROLE_SELECT;
            }
        }
    }
}

/*---------------------------------------------------------------------------------*/

void App_displayAdminMenu(void)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "1:Price 2:Pass");
    LCD_displayStringRowColumn(1, 0, "3:$ 4:Cal 0:Out");
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
void App_handleAdminMenu(void)
{
    uint8 key;

    App_displayAdminMenu();

    key = KEYPAD_getPressedKey();

    switch (key)
    {
    case 'A':
    case 'a':
    case '1':
        g_currentState = STATE_UPDATE_PRICE;
        break;

    case 'B':
    case 'b':
    case '2':
        g_currentState = STATE_UPDATE_PASSWORD;
        break;

    case 'C':
    case 'c':
    case '3':
        g_currentState = STATE_VIEW_INCOME;
        break;

    case 'D':
    case 'd':
    case '4':
        g_currentState = STATE_CALIBRATE_SCALE; /* NEW */
        break;

    case '0':
        g_currentState = STATE_LOGOUT;
        break;

    default:
        App_showError("Invalid Option!");
        break;
    }
}

void App_handleUserBrowseItems(void)
{
    char itemName[17];
    float itemPrice;
    uint8 key;

    /* Load current item data */
    AppData_loadItemName(g_currentItemIndex, itemName);
    itemPrice = AppData_loadItemPrice(g_currentItemIndex);

    /* Display item */
    LCD_clearScreen();
    LCD_goToRowColumn(0, 0);
    LCD_displayInteger(g_currentItemIndex);
    LCD_displayStringRowColumn(0, 1, ". ");
    LCD_displayStringRowColumn(0, 3, itemName);
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayFloat(itemPrice);
    LCD_displayString("/KG");

    _delay_ms(500);

    /* Wait for user input */
    key = KEYPAD_getPressedKey();

    if (key == '#') /* Select this item */
    {
        g_currentState = STATE_USER_WEIGH_ITEM;
    }
    else if (key == 'A') /* Next item */
    {
        g_currentItemIndex++;
        if (g_currentItemIndex > APPDATA_NUM_ITEMS)
        {
            g_currentItemIndex = 1;
        }
    }
    else if (key == 'B') /* Previous item */
    {
        g_currentItemIndex--;
        if (g_currentItemIndex < 1)
        {
            g_currentItemIndex = APPDATA_NUM_ITEMS;
        }
    }
    else if (key == 'D') /* Checkout */
    {
        if (g_sessionTotal > 0.0)
        {
            g_currentState = STATE_USER_CHECKOUT;
        }
        else
        {
            App_showMessage("Cart Empty!", "Add items first", 2000);
        }
    }
    else if (key == 'C') /* Cancel/Exit */
    {
        if (g_sessionTotal > 0.0)
        {
            LCD_clearScreen();
            LCD_displayStringRowColumn(0, 0, "Cancel order?");
            LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

            key = KEYPAD_getPressedKey();
            if (key == '1')
            {
                g_sessionTotal = 0.0;
                g_currentState = STATE_ROLE_SELECT;
            }
        }
        else
        {
            g_currentState = STATE_ROLE_SELECT;
        }
    }
}

/*---------------------------------------------------------------------------------*/

void App_handleUserWeighItem(void)
{
    float weight;
    float unitPrice;
    float itemTotal;
    char itemName[17];
    uint8 key;

    /* Get item data */
    AppData_loadItemName(g_currentItemIndex, itemName);
    unitPrice = AppData_loadItemPrice(g_currentItemIndex);

    /* Step 3: Ask to place weight */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Place weight");
    LCD_displayStringRowColumn(1, 0, "then press #");
    _delay_ms(1000);

    /* Step 4: Show real-time weight */
    while (1)
    {
        weight = getWeight();
        _delay_ms(100);
        LCD_clearScreen();
        LCD_displayStringRowColumn(0, 0, "Weight:");
        LCD_goToRowColumn(1, 0);
        App_displayFloat(weight);
        LCD_displayString(" KG");

        /* Check for confirmation */
        key = KEYPAD_getPressedKeyNonBlocking();
        if (key == '#')
        {
            break;
        }
        else if (key == '*') /* Cancel */
        {
            g_currentState = STATE_USER_BROWSE_ITEMS;
            return;
        }
    }

    /* Step 5: Calculate price */
    itemTotal = weight * unitPrice;
    g_sessionTotal += (double)itemTotal;

    /* Display item total */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, itemName);
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayFloat(itemTotal);
    _delay_ms(2000);

    /* Step 6: Ask for another item */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Add another?");
    LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

    key = KEYPAD_getPressedKey();

    if (key == '1')
    {
        g_currentState = STATE_USER_BROWSE_ITEMS;
    }
    else
    {
        g_currentState = STATE_USER_CHECKOUT;
    }
}

/*---------------------------------------------------------------------------------*/

void App_handleUserCheckout(void)
{
    uint8 key;

    /* Display total */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Total Amount:");
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayDouble(g_sessionTotal);
    _delay_ms(3000);

    /* Ask for confirmation */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Confirm payment?");
    LCD_displayStringRowColumn(1, 0, "1:Yes  0:Cancel");

    key = KEYPAD_getPressedKey();

    if (key == '1')
    {
        /* Add to total income */
        if (AppData_addToTotalIncome(g_sessionTotal) == APPDATA_NO_ERROR)
        {
            App_showSuccess("Payment Done!");

            /* Display receipt */
            LCD_clearScreen();
            LCD_displayStringRowColumn(0, 0, "Thank you!");
            LCD_displayStringRowColumn(1, 0, "Have a nice day");
            _delay_ms(3000);

            /* Reset session */
            g_sessionTotal = 0.0;
            g_currentState = STATE_ROLE_SELECT;
        }
        else
        {
            App_showError("Payment Failed!");
            g_currentState = STATE_USER_BROWSE_ITEMS;
        }
    }
    else
    {
        App_showMessage("Cancelled", "Returning...", 1000);
        g_currentState = STATE_USER_BROWSE_ITEMS;
    }
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------
 * Weight Measurement Functions - HX711 Implementation
 *---------------------------------------------------------------------------------*/

float getWeight(void)
{
    /*
     * Read weight from HX711 in kilograms
     * Uses the library's built-in calibrated reading
     */

    double weight_kg;

    // Get weight using library function (returns calibrated value)
    weight_kg = hx711_getweight();

    // Ensure non-negative weight
    if (weight_kg < 0.0)
    {
        weight_kg = 0.0;
    }

    return (float)weight_kg;
}

/*---------------------------------------------------------------------------------*/

/* Keep all the Admin functions from previous version */
void App_handleUpdatePrice(void)
{
    char indexBuffer[2];
    char priceBuffer[MAX_PRICE_DIGITS + 1];
    uint8 itemIndex;
    float newPrice;
    float currentPrice;

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Update Price/KG");
    _delay_ms(1000);

    /* Get item index */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Item (1-5):");
    LCD_goToRowColumn(1, 0);

    indexBuffer[0] = KEYPAD_getPressedKey();
    indexBuffer[1] = '\0';
    LCD_displayCharacter(indexBuffer[0]);

    itemIndex = atoi(indexBuffer);

    if (!App_validateItemIndex(itemIndex))
    {
        App_showError("Invalid Index!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    /* Display current price */
    currentPrice = AppData_loadItemPrice(itemIndex);
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Current:$/KG");
    LCD_goToRowColumn(1, 0);
    App_displayFloat(currentPrice);
    _delay_ms(2000);

    /* Get new price */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "New Price/KG:");
    LCD_displayStringRowColumn(1, 0, "$");
    LCD_goToRowColumn(1, 1);

    App_getNumericInput(priceBuffer, MAX_PRICE_DIGITS);
    newPrice = App_parsePrice(priceBuffer);

    if (!App_validatePrice(newPrice))
    {
        App_showError("Invalid Price!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    /* Save new price */
    if (AppData_saveItemPrice(itemIndex, newPrice) == APPDATA_NO_ERROR)
    {
        App_showSuccess("Price Updated!");
    }
    else
    {
        App_showError("Update Failed!");
    }

    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/

void App_handleUpdatePassword(void)
{
    char currentPassword[MAX_PASSWORD_LENGTH + 1];
    char newPassword[MAX_PASSWORD_LENGTH + 1];
    char confirmPassword[MAX_PASSWORD_LENGTH + 1];

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Verify Identity");
    _delay_ms(1000);

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Password:");
    LCD_goToRowColumn(1, 0);

    App_getPasswordInput(currentPassword, MAX_PASSWORD_LENGTH);

    if (!AppData_verifyPassword(currentPassword))
    {
        App_showError("Wrong Password!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "New Password:");
    LCD_goToRowColumn(1, 0);

    App_getPasswordInput(newPassword, MAX_PASSWORD_LENGTH);

    if (!App_validatePassword(newPassword))
    {
        App_showError("Invalid Format!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Confirm:");
    LCD_goToRowColumn(1, 0);

    App_getPasswordInput(confirmPassword, MAX_PASSWORD_LENGTH);

    if (strcmp(newPassword, confirmPassword) != 0)
    {
        App_showError("Not Matching!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    if (AppData_savePassword(newPassword) == APPDATA_NO_ERROR)
    {
        App_showSuccess("Password Changed!");
    }
    else
    {
        App_showError("Update Failed!");
    }

    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/

void App_handleViewIncome(void)
{
    double totalIncome;
    uint8 key;

    totalIncome = AppData_loadTotalIncome();

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Total Income:");
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayDouble(totalIncome);
    _delay_ms(3000);

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Reset Income?");
    LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

    key = KEYPAD_getPressedKey();

    if (key == '1')
    {
        LCD_clearScreen();
        LCD_displayStringRowColumn(0, 0, "Are you sure?");
        LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

        key = KEYPAD_getPressedKey();

        if (key == '1')
        {
            if (AppData_saveTotalIncome(0.0) == APPDATA_NO_ERROR)
            {
                App_showSuccess("Income Reset!");
            }
            else
            {
                App_showError("Reset Failed!");
            }
        }
    }

    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/
void App_handleCalibrateScale(void)
{
    uint8 key;
    /* Display calibration intro */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Scale Calibrate");
    LCD_displayStringRowColumn(1, 0, "Press # to start");

    key = KEYPAD_getPressedKey();
    if (key != '#')
    {
        g_currentState = STATE_ADMIN_MENU;
        return;
    }
    performScaleCalibration();
    /* Return to admin menu */
    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/

/* Keep all helper functions from previous version */
void App_getPasswordInput(char *password, uint8 maxLength)
{
    uint8 index = 0;
    uint8 key;

    while (index < maxLength)
    {
        key = KEYPAD_getPressedKey();

        if (key == '#')
        {
            break;
        }
        else if (key == '*')
        {
            if (index > 0)
            {
                index--;
                LCD_goToRowColumn(1, index);
                LCD_displayCharacter(' ');
                LCD_goToRowColumn(1, index);
            }
        }
        else if (key >= '0' && key <= '9')
        {
            password[index] = key;
            LCD_displayCharacter('*');
            index++;
        }
    }

    password[index] = '\0';
}

/*---------------------------------------------------------------------------------*/

void App_getNumericInput(char *buffer, uint8 maxLength)
{
    uint8 index = 0;
    uint8 key;
    uint8 hasDecimal = 0;
    uint8 decimalCount = 0;

    while (index < maxLength)
    {
        key = KEYPAD_getPressedKey();

        if (key == '#')
        {
            break;
        }
        else if (key == '*')
        {
            if (index > 0)
            {
                index--;
                if (buffer[index] == '.')
                {
                    hasDecimal = 0;
                    decimalCount = 0;
                }
                else if (hasDecimal)
                {
                    decimalCount--;
                }
                LCD_goToRowColumn(1, index + 1);
                LCD_displayCharacter(' ');
                LCD_goToRowColumn(1, index + 1);
            }
        }
        else if (key >= '0' && key <= '9')
        {
            if (hasDecimal && decimalCount >= DECIMAL_PLACES)
            {
                continue;
            }

            buffer[index] = key;
            LCD_displayCharacter(key);
            index++;

            if (hasDecimal)
            {
                decimalCount++;
            }
        }
        else if (key == 'D' && !hasDecimal && index > 0)
        {
            buffer[index] = '.';
            LCD_displayCharacter('.');
            index++;
            hasDecimal = 1;
        }
    }

    buffer[index] = '\0';
}

/*---------------------------------------------------------------------------------*/

float App_parsePrice(const char *priceString)
{
    return atof(priceString);
}

/*---------------------------------------------------------------------------------*/

void App_displayFloat(float value)
{
    char buffer[16];
    uint8 i;

    dtostrf(value, 6, DECIMAL_PLACES, buffer);

    i = 0;
    while (buffer[i] == ' ')
    {
        i++;
    }

    LCD_displayString(&buffer[i]);
}

/*---------------------------------------------------------------------------------*/

void App_displayDouble(double value)
{
    char buffer[32];
    uint8 i;

    dtostrf(value, 10, DECIMAL_PLACES, buffer);

    i = 0;
    while (buffer[i] == ' ')
    {
        i++;
    }

    LCD_displayString(&buffer[i]);
}

/*---------------------------------------------------------------------------------*/

void App_showMessage(const char *line1, const char *line2, uint16 delayMs)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, line1);

    if (line2 != NULL && strlen(line2) > 0)
    {
        LCD_displayStringRowColumn(1, 0, line2);
    }

    _delay_ms(delayMs);
}

/*---------------------------------------------------------------------------------*/

void App_showError(const char *message)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "ERROR!");
    LCD_displayStringRowColumn(1, 0, message);
    _delay_ms(2000);
}

/*---------------------------------------------------------------------------------*/

void App_showSuccess(const char *message)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "SUCCESS!");
    LCD_displayStringRowColumn(1, 0, message);
    _delay_ms(2000);
}

/*---------------------------------------------------------------------------------*/

uint8 App_validatePrice(float price)
{
    return (price >= APPDATA_MIN_PRICE && price <= APPDATA_MAX_PRICE) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/

uint8 App_validatePassword(const char *password)
{
    uint8 length = strlen(password);
    uint8 i;

    if (length < 4 || length > MAX_PASSWORD_LENGTH)
    {
        return 0;
    }

    for (i = 0; i < length; i++)
    {
        if (password[i] < '0' || password[i] > '9')
        {
            return 0;
        }
    }

    return 1;
}

/*---------------------------------------------------------------------------------*/

uint8 App_validateItemIndex(uint8 index)
{
    return (index >= 1 && index <= APPDATA_NUM_ITEMS) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/
void performScaleCalibration(void)
{
    /*
     * Two-step calibration process for your HX711 load cell
     * This should be done ONCE during initial setup
     */
    double scale;
    int32_t offset;
    uint8_t key;
    double knownWeight = 1.0; // Use 1.000 kg calibration weight

    /* Step 1: Calibrate offset (tare) */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Remove weight");
    LCD_displayStringRowColumn(1, 0, "Press # to tare");

    key = KEYPAD_getPressedKey();
    if (key == '#')
    {
        // Use library's calibration function
        hx711_calibrate1setoffset();

        App_showSuccess("Tare Done!");
        _delay_ms(1000);
    }

    /* Step 2: Calibrate scale factor */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Place 1.000 KG");
    LCD_displayStringRowColumn(1, 0, "Press # to cal.");

    key = KEYPAD_getPressedKey();
    if (key == '#')
    {
        // Use library's calibration function with known weight
        hx711_calibrate2setscale(knownWeight);

        App_showSuccess("Scale Calibrated!");
        _delay_ms(1000);
        /* After calibration, save to EEPROM */
        scale = hx711_getscale();
        offset = hx711_getoffset();
        if (AppData_saveCalibration(scale, offset) == APPDATA_NO_ERROR)
        {
            App_showSuccess("Cal. Saved!");
        }
        else
        {
            App_showError("Save Failed!");
        }
    }
}
=======
/***********************************************************************************
 *                                                                                 *
 * [MODULE]: Application                                                           *
 *                                                                                 *
 * [FILE NAME]: main.c                                                             *
 *                                                                                 *
 * [DESCRIPTION]: Fruit weighing and pricing system with Admin/User roles         *
 *                                                                                 *
 ***********************************************************************************/

/*---------------------------------------------------------------------------------*
 *                                   INCLUDES                                      *
 *---------------------------------------------------------------------------------*/
#include "eeprom.h"
#include "app_data.h"
#include "keypad.h"
#include "lcd.h"
#include "hx711.h"


/*---------------------------------------------------------------------------------*
 *                              GLOBAL CONST MACROS                                *
 *---------------------------------------------------------------------------------*/
#define MAX_PASSWORD_LENGTH         6
#define MAX_PRICE_DIGITS            8
#define DECIMAL_PLACES              3

/*---------------------------------------------------------------------------------*
 *                                     ENUMS                                       *
 *---------------------------------------------------------------------------------*/

typedef enum
{
    STATE_ROLE_SELECT = 0,
    STATE_ADMIN_LOGIN,
    STATE_ADMIN_MENU,
    STATE_UPDATE_PRICE,
    STATE_UPDATE_PASSWORD,
    STATE_VIEW_INCOME,
    STATE_CALIBRATE_SCALE,
    STATE_USER_BROWSE_ITEMS,
    STATE_USER_WEIGH_ITEM,
    STATE_USER_CHECKOUT,
    STATE_LOGOUT
} AppState_t;


typedef enum
{
    ROLE_NONE = 0,
    ROLE_ADMIN,
    ROLE_USER
} UserRole_t;

/*---------------------------------------------------------------------------------*
 *                              GLOBAL VARIABLES                                   *
 *---------------------------------------------------------------------------------*/
static AppState_t g_currentState = STATE_ROLE_SELECT;
static UserRole_t g_currentRole = ROLE_NONE;
static uint8 g_isAuthenticated = 0;
static uint8 g_currentItemIndex = 1;
static double g_sessionTotal = 0.0;
 sint32 g_moffset = 0;         // Tare offset
 double g_mscale = 1.0;        // Scale factor

/*---------------------------------------------------------------------------------*
 *                              FUNCTION PROTOTYPES                                *
 *---------------------------------------------------------------------------------*/

/* Initialization */
void App_init(void);
void App_displayWelcome(void);

/* Role Selection */
void App_handleRoleSelect(void);

/* Admin Functions */
void App_handleAdminLogin(void);
void App_displayAdminMenu(void);
void App_handleAdminMenu(void);
void App_handleUpdatePrice(void);
void App_handleUpdatePassword(void);
void App_handleViewIncome(void);
void performScaleCalibration(void);
void App_handleCalibrateScale(void);

/* User Functions */
void App_handleUserBrowseItems(void);
void App_handleUserWeighItem(void);
void App_handleUserCheckout(void);

/* Weight Measurement (Simulated) */
float getWeight(void);

/* Input Functions */
void App_getPasswordInput(char* password, uint8 maxLength);
void App_getNumericInput(char* buffer, uint8 maxLength);
float App_parsePrice(const char* priceString);

/* Display Helpers */
void App_displayFloat(float value);
void App_displayDouble(double value);
void App_showMessage(const char* line1, const char* line2, uint16 delayMs);
void App_showError(const char* message);
void App_showSuccess(const char* message);

/* Validation */
uint8 App_validatePrice(float price);
uint8 App_validatePassword(const char* password);
uint8 App_validateItemIndex(uint8 index);

/*---------------------------------------------------------------------------------*
 *                              FUNCTION DEFINITIONS                               *
 *---------------------------------------------------------------------------------*/

int main(void)
{
    App_init();

    while(1)
    {
        switch(g_currentState)
        {
            case STATE_ROLE_SELECT:
                App_handleRoleSelect();
                break;

            case STATE_ADMIN_LOGIN:
                App_handleAdminLogin();
                break;

            case STATE_ADMIN_MENU:
                App_handleAdminMenu();
                break;

            case STATE_UPDATE_PRICE:
                App_handleUpdatePrice();
                break;

            case STATE_UPDATE_PASSWORD:
                App_handleUpdatePassword();
                break;

            case STATE_VIEW_INCOME:
                App_handleViewIncome();
                break;
            case STATE_CALIBRATE_SCALE:
                App_handleCalibrateScale();
                break;
            case STATE_USER_BROWSE_ITEMS:
                App_handleUserBrowseItems();
                break;

            case STATE_USER_WEIGH_ITEM:
                App_handleUserWeighItem();
                break;

            case STATE_USER_CHECKOUT:
                App_handleUserCheckout();
                break;

            case STATE_LOGOUT:
                g_isAuthenticated = 0;
                g_currentRole = ROLE_NONE;
                g_sessionTotal = 0.0;
                g_currentState = STATE_ROLE_SELECT;
                App_showMessage("Logged Out", "Thank you!", 2000);
                break;

            default:
                g_currentState = STATE_ROLE_SELECT;
                break;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------------*/

void App_init(void)
{
    double saved_scale;
    int32_t saved_offset;
    EEPROM_Config_t eepromConfig;

    LCD_init();
    KEYPAD_init();
    eepromConfig.mode = EEPROM_POLLING_MODE;
    eepromConfig.programmingMode = EEPROM_ERASE_AND_WRITE_MODE;
    eepromConfig.enableInterrupt = 0;
    EEPROM_init(&eepromConfig);

    AppData_init();
    App_displayWelcome();
    hx711_init(HX711_GAINCHANNELA128, g_mscale, g_moffset);
}




/*---------------------------------------------------------------------------------*/

void App_displayWelcome(void)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Fruit Weighing");
    LCD_displayStringRowColumn(1, 0, "   System v1.0");
    _delay_ms(2000);
}

/*---------------------------------------------------------------------------------*/

void App_handleRoleSelect(void)
{
    uint8 key;

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Select Role:");
    LCD_displayStringRowColumn(1, 0, "1:Admin  2:User");

    key = KEYPAD_getPressedKey();

    if(key == '1')
    {
        g_currentRole = ROLE_ADMIN;
        g_currentState = STATE_ADMIN_LOGIN;
    }
    else if(key == '2')
    {
        g_currentRole = ROLE_USER;
        g_currentItemIndex = 1;
        g_sessionTotal = 0.0;
        g_currentState = STATE_USER_BROWSE_ITEMS;
    }
    else
    {
        App_showError("Invalid Role!");
    }
}

/*---------------------------------------------------------------------------------*/

void App_handleAdminLogin(void)
{
    char enteredPassword[MAX_PASSWORD_LENGTH + 1];
    uint8 attempts = 3;

    while(attempts > 0 && !g_isAuthenticated)
    {
        LCD_clearScreen();
        LCD_displayStringRowColumn(0, 0, "Admin Login");
        LCD_displayStringRowColumn(1, 0, "Attempts:");
        LCD_displayInteger(attempts);
        _delay_ms(1500);

        LCD_clearScreen();
        LCD_displayStringRowColumn(0, 0, "Password:");
        LCD_goToRowColumn(1, 0);

        App_getPasswordInput(enteredPassword, MAX_PASSWORD_LENGTH);

        if(AppData_verifyPassword(enteredPassword))
        {
            g_isAuthenticated = 1;
            g_currentState = STATE_ADMIN_MENU;
            App_showSuccess("Access Granted!");
        }
        else
        {
            attempts--;
            if(attempts > 0)
            {
                App_showError("Wrong Password!");
            }
            else
            {
                App_showMessage("System Locked!", "Please wait...", 5000);
                g_currentState = STATE_ROLE_SELECT;
            }
        }
    }
}

/*---------------------------------------------------------------------------------*/

void App_displayAdminMenu(void)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "1:Price 2:Pass");
    LCD_displayStringRowColumn(1, 0, "3:$ 4:Cal 0:Out");
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
void App_handleAdminMenu(void)
{
    uint8 key;

    App_displayAdminMenu();

    key = KEYPAD_getPressedKey();

    switch(key)
    {
        case 'A':
        case 'a':
        case '1':
            g_currentState = STATE_UPDATE_PRICE;
            break;

        case 'B':
        case 'b':
        case '2':
            g_currentState = STATE_UPDATE_PASSWORD;
            break;

        case 'C':
        case 'c':
        case '3':
            g_currentState = STATE_VIEW_INCOME;
            break;

        case 'D':
        case 'd':
        case '4':
            g_currentState = STATE_CALIBRATE_SCALE;  /* NEW */
            break;

        case '0':
            g_currentState = STATE_LOGOUT;
            break;

        default:
            App_showError("Invalid Option!");
            break;
    }
}

void App_handleUserBrowseItems(void)
{
    char itemName[17];
    float itemPrice;
    uint8 key;

    /* Load current item data */
    AppData_loadItemName(g_currentItemIndex, itemName);
    itemPrice = AppData_loadItemPrice(g_currentItemIndex);

    /* Display item */
    LCD_clearScreen();
    LCD_goToRowColumn(0,0);
    LCD_displayInteger(g_currentItemIndex);
    LCD_displayStringRowColumn(0, 1, ". ");
    LCD_displayStringRowColumn(0, 3, itemName);
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayFloat(itemPrice);
    LCD_displayString("/KG");

    _delay_ms(500);

    /* Wait for user input */
    key = KEYPAD_getPressedKey();

    if(key == '#')  /* Select this item */
    {
        g_currentState = STATE_USER_WEIGH_ITEM;
    }
    else if(key == 'A')  /* Next item */
    {
        g_currentItemIndex++;
        if(g_currentItemIndex > APPDATA_NUM_ITEMS)
        {
            g_currentItemIndex = 1;
        }
    }
    else if(key == 'B')  /* Previous item */
    {
        g_currentItemIndex--;
        if(g_currentItemIndex < 1)
        {
            g_currentItemIndex = APPDATA_NUM_ITEMS;
        }
    }
    else if(key == 'D')  /* Checkout */
    {
        if(g_sessionTotal > 0.0)
        {
            g_currentState = STATE_USER_CHECKOUT;
        }
        else
        {
            App_showMessage("Cart Empty!", "Add items first", 2000);
        }
    }
    else if(key == 'C')  /* Cancel/Exit */
    {
        if(g_sessionTotal > 0.0)
        {
            LCD_clearScreen();
            LCD_displayStringRowColumn(0, 0, "Cancel order?");
            LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

            key = KEYPAD_getPressedKey();
            if(key == '1')
            {
                g_sessionTotal = 0.0;
                g_currentState = STATE_ROLE_SELECT;
            }
        }
        else
        {
            g_currentState = STATE_ROLE_SELECT;
        }
    }
}

/*---------------------------------------------------------------------------------*/

void App_handleUserWeighItem(void)
{
    float weight;
    float unitPrice;
    float itemTotal;
    char itemName[17];
    uint8 key;

    /* Get item data */
    AppData_loadItemName(g_currentItemIndex, itemName);
    unitPrice = AppData_loadItemPrice(g_currentItemIndex);


    /* Step 3: Ask to place weight */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Place weight");
    LCD_displayStringRowColumn(1, 0, "then press #");
    _delay_ms(500);
    hx711_startSimulationPattern();
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Weight:");
    /* Step 4: Show real-time weight */
    while(1)
    {
        weight = getWeight();
        _delay_ms(100);

        LCD_goToRowColumn(1, 0);
        App_displayFloat(weight);
        LCD_displayString(" kg");
        /* Check for confirmation */
        key = KEYPAD_getPressedKeyNonBlocking();
        if(key == '#')
        {
            break;
        }
        else if(key == '*')  /* Cancel */
        {
            g_currentState = STATE_ROLE_SELECT;
            return;
        }
    }

    /* Step 5: Calculate price */
    itemTotal = weight * unitPrice;
    g_sessionTotal += (double)itemTotal;

    /* Display item total */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, itemName);
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayFloat(itemTotal);
    _delay_ms(2000);

    /* Step 6: Ask for another item */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Add another?");
    LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

    key = KEYPAD_getPressedKey();

    if(key == '1')
    {
        g_currentState = STATE_USER_BROWSE_ITEMS;
    }
    else if(key == '0')
    {
        g_currentState = STATE_USER_CHECKOUT;
    }

}

/*---------------------------------------------------------------------------------*/

void App_handleUserCheckout(void)
{
    uint8 key;

    /* Display total */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Total Amount:");
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayDouble(g_sessionTotal);
    _delay_ms(3000);

    /* Ask for confirmation */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Confirm payment?");
    LCD_displayStringRowColumn(1, 0, "1:Yes  0:Cancel");

    key = KEYPAD_getPressedKey();

    if(key == '1')
    {
        /* Add to total income */
        if(AppData_addToTotalIncome(g_sessionTotal) == APPDATA_NO_ERROR)
        {
            App_showSuccess("Payment Done!");

            /* Display receipt */
            LCD_clearScreen();
            LCD_displayStringRowColumn(0, 0, "Thank you!");
            LCD_displayStringRowColumn(1, 0, "Have a nice day");
            _delay_ms(3000);

            /* Reset session */
            g_sessionTotal = 0.0;
            g_currentState = STATE_ROLE_SELECT;
        }
        else
        {
            App_showError("Payment Failed!");
            g_currentState = STATE_USER_BROWSE_ITEMS;
        }
    }
    else if(key==0)
    {
        App_showMessage("Cancelled", "Returning...", 1000);
        g_currentState = STATE_ROLE_SELECT;
    }
}

/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------
 * Weight Measurement Functions - HX711 Implementation
 *---------------------------------------------------------------------------------*/

float getWeight(void)
{
    /*
     * Read weight from HX711 in kilograms
     * Uses the library's built-in calibrated reading
     */

    double weight_kg;

    // Get weight using library function (returns calibrated value)
    weight_kg = hx711_getweight();

    // Ensure non-negative weight
    if(weight_kg < 0.000) {
        weight_kg = 0.000;
    }

    return (float)weight_kg;
}

/*---------------------------------------------------------------------------------*/

/* Keep all the Admin functions from previous version */
void App_handleUpdatePrice(void)
{
    char indexBuffer[2];
    char priceBuffer[MAX_PRICE_DIGITS + 1];
    uint8 itemIndex;
    float newPrice;
    float currentPrice;

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Update Price/KG");
    _delay_ms(1000);

    /* Get item index */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Item (1-5):");
    LCD_goToRowColumn(1, 0);

    indexBuffer[0] = KEYPAD_getPressedKey();
    indexBuffer[1] = '\0';
    LCD_displayCharacter(indexBuffer[0]);

    itemIndex = atoi(indexBuffer);

    if(!App_validateItemIndex(itemIndex))
    {
        App_showError("Invalid Index!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    /* Display current price */
    currentPrice = AppData_loadItemPrice(itemIndex);
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Current:$/KG");
    LCD_goToRowColumn(1, 0);
    App_displayFloat(currentPrice);
    _delay_ms(2000);

    /* Get new price */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "New Price/KG:");
    LCD_displayStringRowColumn(1, 0, "$");
    LCD_goToRowColumn(1, 1);

    App_getNumericInput(priceBuffer, MAX_PRICE_DIGITS);
    newPrice = App_parsePrice(priceBuffer);

    if(!App_validatePrice(newPrice))
    {
        App_showError("Invalid Price!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    /* Save new price */
    if(AppData_saveItemPrice(itemIndex, newPrice) == APPDATA_NO_ERROR)
    {
        App_showSuccess("Price Updated!");
    }
    else
    {
        App_showError("Update Failed!");
    }

    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/

void App_handleUpdatePassword(void)
{
    char currentPassword[MAX_PASSWORD_LENGTH + 1];
    char newPassword[MAX_PASSWORD_LENGTH + 1];
    char confirmPassword[MAX_PASSWORD_LENGTH + 1];

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Verify Identity");
    _delay_ms(1000);

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Password:");
    LCD_goToRowColumn(1, 0);

    App_getPasswordInput(currentPassword, MAX_PASSWORD_LENGTH);

    if(!AppData_verifyPassword(currentPassword))
    {
        App_showError("Wrong Password!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "New Password:");
    LCD_goToRowColumn(1, 0);

    App_getPasswordInput(newPassword, MAX_PASSWORD_LENGTH);

    if(!App_validatePassword(newPassword))
    {
        App_showError("Invalid Format!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Confirm:");
    LCD_goToRowColumn(1, 0);

    App_getPasswordInput(confirmPassword, MAX_PASSWORD_LENGTH);

    if(strcmp(newPassword, confirmPassword) != 0)
    {
        App_showError("Not Matching!");
        g_currentState = STATE_ADMIN_MENU;
        return;
    }

    if(AppData_savePassword(newPassword) == APPDATA_NO_ERROR)
    {
        App_showSuccess("Password Changed!");
    }
    else
    {
        App_showError("Update Failed!");
    }

    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/

void App_handleViewIncome(void)
{
    double totalIncome;
    uint8 key;

    totalIncome = AppData_loadTotalIncome();

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Total Income:");
    LCD_goToRowColumn(1, 0);
    LCD_displayString("$");
    App_displayDouble(totalIncome);
    _delay_ms(3000);

    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Reset Income?");
    LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

    key = KEYPAD_getPressedKey();

    if(key == '1')
    {
        LCD_clearScreen();
        LCD_displayStringRowColumn(0, 0, "Are you sure?");
        LCD_displayStringRowColumn(1, 0, "1:Yes  0:No");

        key = KEYPAD_getPressedKey();

        if(key == '1')
        {
            if(AppData_saveTotalIncome(0.0) == APPDATA_NO_ERROR)
            {
                App_showSuccess("Income Reset!");
            }
            else
            {
                App_showError("Reset Failed!");
            }
        }
    }

    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/
void App_handleCalibrateScale(void)
{
    uint8 key;
    /* Display calibration intro */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Scale Calibrate");
    LCD_displayStringRowColumn(1, 0, "Press # to start");

    key = KEYPAD_getPressedKey();
    if(key != '#') {
        g_currentState = STATE_ADMIN_MENU;
        return;
    }
    performScaleCalibration();
    /* Return to admin menu */
    g_currentState = STATE_ADMIN_MENU;
}

/*---------------------------------------------------------------------------------*/

/* Keep all helper functions from previous version */
void App_getPasswordInput(char* password, uint8 maxLength)
{
    uint8 index = 0;
    uint8 key;

    while(index < maxLength)
    {
        key = KEYPAD_getPressedKey();

        if(key == '#')
        {
            break;
        }
        else if(key == '*')
        {
            if(index > 0)
            {
                index--;
                LCD_goToRowColumn(1, index);
                LCD_displayCharacter(' ');
                LCD_goToRowColumn(1, index);
            }
        }
        else if(key >= '0' && key <= '9')
        {
            password[index] = key;
            LCD_displayCharacter('*');
            index++;
        }
    }

    password[index] = '\0';
}

/*---------------------------------------------------------------------------------*/

void App_getNumericInput(char* buffer, uint8 maxLength)
{
    uint8 index = 0;
    uint8 key;
    uint8 hasDecimal = 0;
    uint8 decimalCount = 0;

    while(index < maxLength)
    {
        key = KEYPAD_getPressedKey();

        if(key == '#')
        {
            break;
        }
        else if(key == '*')
        {
            if(index > 0)
            {
                index--;
                if(buffer[index] == '.')
                {
                    hasDecimal = 0;
                    decimalCount = 0;
                }
                else if(hasDecimal)
                {
                    decimalCount--;
                }
                LCD_goToRowColumn(1, index + 1);
                LCD_displayCharacter(' ');
                LCD_goToRowColumn(1, index + 1);
            }
        }
        else if(key >= '0' && key <= '9')
        {
            if(hasDecimal && decimalCount >= DECIMAL_PLACES)
            {
                continue;
            }

            buffer[index] = key;
            LCD_displayCharacter(key);
            index++;

            if(hasDecimal)
            {
                decimalCount++;
            }
        }
        else if(key == 'D' && !hasDecimal && index > 0)
        {
            buffer[index] = '.';
            LCD_displayCharacter('.');
            index++;
            hasDecimal = 1;
        }
    }

    buffer[index] = '\0';
}

/*---------------------------------------------------------------------------------*/

float App_parsePrice(const char* priceString)
{
    return atof(priceString);
}

/*---------------------------------------------------------------------------------*/

void App_displayFloat(float value)
{
    char buffer[16];
    uint8 i;

    dtostrf(value, 6, DECIMAL_PLACES, buffer);

    i = 0;
    while(buffer[i] == ' ')
    {
        i++;
    }

    LCD_displayString(&buffer[i]);
}

/*---------------------------------------------------------------------------------*/

void App_displayDouble(double value)
{
    char buffer[32];
    uint8 i;

    dtostrf(value, 10, DECIMAL_PLACES, buffer);

    i = 0;
    while(buffer[i] == ' ')
    {
        i++;
    }

    LCD_displayString(&buffer[i]);
}

/*---------------------------------------------------------------------------------*/

void App_showMessage(const char* line1, const char* line2, uint16 delayMs)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, line1);

    if(line2 != NULL && strlen(line2) > 0)
    {
        LCD_displayStringRowColumn(1, 0, line2);
    }

    _delay_ms(delayMs);
}

/*---------------------------------------------------------------------------------*/

void App_showError(const char* message)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "ERROR!");
    LCD_displayStringRowColumn(1, 0, message);
    _delay_ms(2000);
}

/*---------------------------------------------------------------------------------*/

void App_showSuccess(const char* message)
{
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "SUCCESS!");
    LCD_displayStringRowColumn(1, 0, message);
    _delay_ms(2000);
}

/*---------------------------------------------------------------------------------*/

uint8 App_validatePrice(float price)
{
    return (price >= APPDATA_MIN_PRICE && price <= APPDATA_MAX_PRICE) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/

uint8 App_validatePassword(const char* password)
{
    uint8 length = strlen(password);
    uint8 i;

    if(length < 4 || length > MAX_PASSWORD_LENGTH)
    {
        return 0;
    }

    for(i = 0; i < length; i++)
    {
        if(password[i] < '0' || password[i] > '9')
        {
            return 0;
        }
    }

    return 1;
}

/*---------------------------------------------------------------------------------*/

uint8 App_validateItemIndex(uint8 index)
{
    return (index >= 1 && index <= APPDATA_NUM_ITEMS) ? 1 : 0;
}

/*---------------------------------------------------------------------------------*/
void performScaleCalibration(void)
{
    /*
     * Two-step calibration process for your HX711 load cell
     * This should be done ONCE during initial setup
     */
	double scale;
	int32_t offset;
    uint8_t key;
    double knownWeight = 1.0;  // Use 1.000 kg calibration weight

    /* Step 1: Calibrate offset (tare) */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Remove weight");
    LCD_displayStringRowColumn(1, 0, "Press # to tare");

    key = KEYPAD_getPressedKey();
    if(key == '#') {
        // Use library's calibration function
        hx711_calibrate1setoffset();

        App_showSuccess("Tare Done!");
        _delay_ms(1000);
    }

    /* Step 2: Calibrate scale factor */
    LCD_clearScreen();
    LCD_displayStringRowColumn(0, 0, "Place 1.000 KG");
    LCD_displayStringRowColumn(1, 0, "Press # to cal.");

    key = KEYPAD_getPressedKey();
    if(key == '#') {
        // Use library's calibration function with known weight
        hx711_calibrate2setscale(knownWeight);

        App_showSuccess("Scale Calibrated!");
        _delay_ms(1000);
        /* After calibration, save to EEPROM */
        scale = hx711_getscale();
        offset = hx711_getoffset();
        g_moffset=offset;
        g_mscale=scale;
        if(AppData_saveCalibration(scale, offset) == APPDATA_NO_ERROR) {
            App_showSuccess("Cal. Saved!");
        } else {
           App_showError("Save Failed!");
        }
    }
}

>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548
