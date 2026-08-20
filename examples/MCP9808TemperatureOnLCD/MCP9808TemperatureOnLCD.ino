/*
 *  MCP9808TemperatureOnLCD.cpp
 *
 *  Read a MCP9808 sensor, and show value in BIG numbers on a 2004 LCD.
 *  With pin 2 you can toggle between 4 and 3 rows big numbers - for demonstration and optional additional information in bottom row.
 *  The MCP9808 chip requires 2.7 to 5.5 V and 200 to 400 uA.
 *
 *  Copyright (C) 2023-2026  Armin Joachimsmeyer
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>

#include "mcp9808.h"

// MCP9808 can run with 400 kHz, but here it runs with TWI default 100 kHz
MCP9808 TemperatureSensor;

#define VERSION_EXAMPLE "1.0"

/*
 * Choose your display
 */
//#define USE_PARALLEL_2004_LCD // one of USE_PARALLEL_2004_LCD (default) and USE_SERIAL_2004_LCD
//#define USE_SERIAL_2004_LCD
#include "LCDBigNumbers.hpp" // Include sources for LCD big number generation
#if defined(USE_PARALLEL_LCD)
LiquidCrystal myLCD(8, 9, 4, 5, 6, 7); // Depends on your actual connections
//LiquidCrystal myLCD(4, 5, 6, 7, 8, 9); // Sample connections starting at pin 4
#else
#define LCD_I2C_ADDRESS 0x27    // Default LCD address is 0x27 for a 20 chars and 4 line / 2004 display
LiquidCrystal_I2C myLCD(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS); // LCD_COLUMNS and LCD_ROWS are set by LCDBigNumbers.hpp depending on the defined display
#endif

LCDBigNumbers BigNumbersLCD(&myLCD, BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_1); // Use 3x4 numbers, 1. variant
#define LCD_STRING_BUFFER_SIZE 6
char sLCDStringBuffer[LCD_STRING_BUFFER_SIZE];

/*
 * Show battery value on LCD
 */
#define SHOW_VCC_ON_LCD // disable it, if you have it connected to an external power supply and VCC value is of no use
#if defined(SHOW_VCC_ON_LCD)
#include "ADCUtils.hpp"
uint16_t sVCCMillivolt;
#define VCC_LCD_COLUMN         16
#define VCC_LCD_ROW             3
void printVCCOnLCD();
#endif

/*
 * LCD page button
 */
#define USE_BUTTON_0
#include "EasyButtonAtInt01.hpp"
uint8_t sLCDPageNumber;
void handleLCDPageButtonPress(bool aButtonToggleState) {
    (void) aButtonToggleState;
    sLCDPageNumber++;
    myLCD.clear();

    if (sLCDPageNumber == 2) {
        BigNumbersLCD.init(BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_1);
        Serial.print('4');
        sLCDPageNumber = 0;
    } else {
        BigNumbersLCD.init(BIG_NUMBERS_FONT_3_COLUMN_3_ROWS_VARIANT_1);
        Serial.print('3');
    }
}
EasyButton StoreButtonAtPin2(&handleLCDPageButtonPress); // Button is connected to INT0 (pin2)

// Helper macro for getting a macro definition as string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

void setup() {

    Serial.begin(115200);
    Serial.println(F("START " __FILE__ "\r\nVersion " VERSION_EXAMPLE " from " __DATE__));

#if defined(USE_PARALLEL_LCD)
    myLCD.begin(LCD_COLUMNS, LCD_ROWS); // This also clears display
#else
    myLCD.init();
    myLCD.clear();
    myLCD.backlight();  // Switch backlight LED on
#endif
    BigNumbersLCD.begin(); // This also sets cursor to 0.0

    myLCD.print(F("MCP9808 Temp. " VERSION_EXAMPLE));
    myLCD.setCursor(0, 1);
    myLCD.print(F("Device "));

    // Set device address and check for connection
    if (TemperatureSensor.searchMCP9808OnI2C() == MCP9808_I2C_NOT_FOUND_ADDRESS) {
        Serial.print(F(" not found"));
    } else {
        Serial.print(F("Device found at address 0x"));
        Serial.print(TemperatureSensor._address,HEX);
        Serial.print(F(" with revision "));
        Serial.print(TemperatureSensor.getRevision()); // is 0
        Serial.print(F(" and manufacturer ID "));
        Serial.println(TemperatureSensor.getManufacturerID()); // is 0x54 / 84

        myLCD.print(F(" found at "));
        myLCD.print(TemperatureSensor._address, HEX);
    }

    myLCD.setCursor(0, 2);
    myLCD.print(F("Page toggle pin = 2"));
    Serial.println(F("Page toggle pin = 2"));
#if defined(SHOW_VCC_ON_LCD)
    myLCD.setCursor(VCC_LCD_COLUMN - 6, VCC_LCD_ROW);
    myLCD.print(F("VCC = "));
    Serial.println(F("VCC display is bottom right"));
    printVCCOnLCD();
#endif

    delay(4000);
    myLCD.clear();

}

void loop() {

    float tTemperature = ((float) (TemperatureSensor.getTemperatureTimes16()) / 16.0);
    /*
     * Print the 3 or 4 line big digits here
     */
    dtostrf(tTemperature, 4, 2, sLCDStringBuffer);
    BigNumbersLCD.setBigNumberCursor(0, 0);
    BigNumbersLCD.print(sLCDStringBuffer);
    if (tTemperature >= 0.0) {
        myLCD.setCursor(17, 1);
        myLCD.print(F("\xDF" "C "));
#if defined(SHOW_VCC_ON_LCD)
        printVCCOnLCD();
#endif
    } else {
        myLCD.setCursor(18, 1);
        myLCD.print(F("\xDF" "C"));
    }

    delay(1000); // at a 0.0625 resolution we have maximum 4 samples/s
}

#if defined(SHOW_VCC_ON_LCD)
/*
 * Uses 5 V ADC reference
 */
void printVCCOnLCD() {
    myLCD.setCursor(VCC_LCD_COLUMN, VCC_LCD_ROW);
    sVCCMillivolt = getVCCVoltageMillivolt();
    myLCD.print(sVCCMillivolt / 1000.0, 2);
//    myLCD.print(F(" V"));
}
#endif
