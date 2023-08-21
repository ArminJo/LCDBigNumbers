/*
 *  BigNumbersDemo.cpp
 *
 *  Shows all big numbers on a 1602 or 2004 LCD.
 *  https://wokwi.com/projects/346534078384702034
 *
 *  Copyright (C) 2022-2023  Armin Joachimsmeyer
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

//#define USE_PARALLEL_2004_LCD // Is default
//#define USE_PARALLEL_1602_LCD
//#define USE_SERIAL_2004_LCD
//#define USE_SERIAL_1602_LCD
#include "LCDBigNumbers.hpp" // Include sources for LCD big number generation

#if defined(USE_PARALLEL_LCD)
#include <LiquidCrystal.h>
LiquidCrystal myLCD(2, 3, 4, 5, 6, 7); // Depends on your actual connections
//LiquidCrystal myLCD(4, 5, 6, 7, 8, 9); // Sample connections starting at pin 4
#else
#define LCD_I2C_ADDRESS 0x27    // Default LCD address is 0x27 for a 20 chars and 4 line / 2004 display
#include "LiquidCrystal_I2C.h"  // Use an up to date library version which has the init method
LiquidCrystal_I2C myLCD(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS); // LCD_COLUMNS and LCD_ROWS are set by LCDBigNumbers.hpp depending on the defined display
#endif

void setup() {
    Serial.begin(115200);

#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__));

#if defined(USE_PARALLEL_LCD)
    myLCD.begin(LCD_COLUMNS, LCD_ROWS); // LCD_COLUMNS and LCD_ROWS are set by LCDBigNumbers.hpp depending on the defined display
#else
    myLCD.init();
    myLCD.clear();
    myLCD.backlight(); // Switch backlight LED on
#endif
    myLCD.print(F("Big numbers demo"));
    myLCD.setCursor(0, 1);
    myLCD.print(__DATE__);
    delay(2000);

}

void loop() {
    /*
     * Show all big number fonts: BIG_NUMBERS_FONT_1_COLUMN_2_ROWS_VARIANT_1, BIG_NUMBERS_FONT_2_COLUMN_2_ROWS_VARIANT_1,
     * BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_1, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_2, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_3,
     * BIG_NUMBERS_FONT_2_COLUMN_3_ROWS_VARIANT_1, BIG_NUMBERS_FONT_2_COLUMN_3_ROWS_VARIANT_2, BIG_NUMBERS_FONT_3_COLUMN_3_ROWS_VARIANT_1,
     * BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_1, BIG_NUMBERS_FONT_3_COLUMN_4_ROWS_VARIANT_2
     */
    testBigNumbers(&myLCD);
}
