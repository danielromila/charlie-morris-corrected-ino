# charlie-morris-corrected-ino

This file was made and publicly posted by Charlie Morris, ZL2CTM.

The two required libraries:

#include <LiquidCrystal_I2C.h> //fmalpartida library, from https://github.com/fmalpartida/New-LiquidCrystal , updated November 24th 2020
#include <si5351.h> // Adafruit Si5351 library, version 1.2.0

Some corrections I made:

// Instantiate the Objects
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7);    //I2C Address 0x27 (or 0x3F)
Si5351 si5351(0x60); //I2C Address 0x60

 // Initialize the display
 lcd.begin (16,2); // for 16 x 2 LCD module
 lcd.setBacklightPin(3,POSITIVE);
 lcd.setBacklight(HIGH);
