/*
 *  SimpleClock.cpp
 *
 *  Shows a (fast) running clock with big numbers on a 2004 LCD.
  * https://wokwi.com/projects/346661429974139474
 *
 *  Copyright (C) 2022  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of LCDBigNumbers https://github.com/ArminJo/LCDBigNumbers.
 *
 *  LCDBigNumbers is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
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

/*
 * Choose your display
 */
//#define USE_PARALLEL_2004_LCD // Is default
//#define USE_PARALLEL_1602_LCD
//#define USE_SERIAL_2004_LCD
//#define USE_SERIAL_1602_LCD
#include "LCDBigNumbers.hpp" // Include sources for LCD big number generation

#if defined(USE_PARALLEL_LCD)
LiquidCrystal myLCD(2, 3, 4, 5, 6, 7); // Depends on your actual connections
//LiquidCrystal myLCD(4, 5, 6, 7, 8, 9); // Sample connections starting at pin 4
#else
#define LCD_I2C_ADDRESS 0x27    // Default LCD address is 0x27 for a 20 chars and 4 line / 2004 display
LiquidCrystal_I2C myLCD(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS); // LCD_COLUMNS and LCD_ROWS are set by LCDBigNumbers.hpp depending on the defined display
#endif

/*
 * Available big number fonts are: BIG_NUMBERS_FONT_1_COLUMN_2_ROWS_VARIANT_1, BIG_NUMBERS_FONT_2_COLUMN_2_ROWS_VARIANT_1,
 * BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_1, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_2, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_3,
 * BIG_NUMBERS_FONT_2_COLUMN_3_ROWS_VARIANT_1, BIG_NUMBERS_FONT_2_COLUMN_3_ROWS_VARIANT_2, BIG_NUMBERS_FONT_3_COLUMN_3_ROWS_VARIANT_1,
 * BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_1, BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_2
 */
LCDBigNumbers ThreeLineNumbersLCD(&myLCD, BIG_NUMBERS_FONT_3_COLUMN_3_ROWS_VARIANT_1); // Use 3x3 numbers

void setup() {
    Serial.begin(115200);
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__));

#if defined(USE_PARALLEL_LCD)
    myLCD.begin(LCD_COLUMNS, LCD_ROWS); // LCD_COLUMNS and LCD_ROWS are set by LCDBigNumbers.hpp depending on the defined display
#else
    myLCD.init();
    myLCD.clear();
    myLCD.backlight(); // Switch backlight LED on
#endif
    ThreeLineNumbersLCD.begin(); // Creates custom character used for generating big numbers
    myLCD.setCursor(0, 0);
    myLCD.print(F("Clock demo"));
    myLCD.setCursor(0, 1);
    myLCD.print(__DATE__);
    delay(2000);
    myLCD.clear();
}

int Seconds = 0;
int Minutes = 48;
int Hours = 11;
unsigned long lastMillisOfClockUpdate = 0;

void loop() {
    if (millis() - lastMillisOfClockUpdate > 1000) { // Fast mode :-), do not wait 60 seconds
        lastMillisOfClockUpdate += 1000;
        /*
         * Print hours
         */
        ThreeLineNumbersLCD.setBigNumberCursor(2);
        if (Hours < 10) {
            ThreeLineNumbersLCD.print('0');
        }
        ThreeLineNumbersLCD.print(Hours);

        /*
         * Do blinking colon
         */
        if (Seconds % 2 == 1) {
            ThreeLineNumbersLCD.print(':');
        } else {
            ThreeLineNumbersLCD.print(ONE_COLUMN_SPACE_CHARACTER);
        }
        ThreeLineNumbersLCD.setBigNumberCursor(12);

        /*
         * Print minutes
         */
        if (Minutes < 10) {
            ThreeLineNumbersLCD.print('0');
        }
        ThreeLineNumbersLCD.print(Minutes);

        /*
         * Go to next minute
         */
        Seconds++;
        if (Seconds >= 2) { // Demo fast mode with blinking colon
//        if (Seconds >= 60) {
            Seconds = 0;
            Minutes++;
            if (Minutes >= 60) {
                Minutes = 0;
                Hours++;
                if (Hours >= 24) {
                    Hours = 0;
                }
            }
        }
    }
}
