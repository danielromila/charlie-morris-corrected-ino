# charlie-morris-corrected-ino

This file was made and publicly posted by Charlie Morris, ZL2CTM. More details in his video at:
https://www.youtube.com/watch?v=XpxEs2_0ygg&feature=em-comments As he posted it, it did not work for me and for others. So I corrected it. You can also see pictures with it in function, also in this folder, and with the outputs verified on my dogital oscilooscope, also connected to the computer.

The two required libraries:

#include <LiquidCrystal_I2C.h> //fmalpartida library, from https://github.com/fmalpartida/New-LiquidCrystal , updated November 24th 2020
                               // if you have already a library LiquidCrystal_I2C in your library folder you need to delete it (or move it), 
                               //                    otherwise will interfere with the useful library
#include <si5351.h> // Adafruit Si5351 library, version 1.2.0

Some corrections I made:

// Instantiate the Objects
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7);    //I2C Address 0x27 (or 0x3F)
Si5351 si5351(0x60); //I2C Address 0x60

 // Initialize the display
 lcd.begin (16,2); // for 16 x 2 LCD module
 lcd.setBacklightPin(3,POSITIVE);
 lcd.setBacklight(HIGH);
