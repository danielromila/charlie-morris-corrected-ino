#include <LiquidCrystal_I2C.h> //fmalpartida library, from https://github.com/fmalpartida/New-LiquidCrystal , updated November 24th 2020
#include <si5351.h> // Adafruit Si5351 library, version 1.2.0

static const long bandStart = 3500000;          // start of VFO range
static const long bandEnd =   3900000;          // end of VFO range
static const long bandInit =  3690000;          // where to initially set the frequency
volatile long freq = 3690000;                   // the current freq
volatile long oldfreq = 0;                      // the previous freq
volatile long LSB_BFO_freq = 8997200;           // High side injection thus SB inversion.
volatile long USB_BFO_freq = 8998700;           // High side injection thus SB inversion.
volatile long radix = 1000;                     // How much to change the frequency by, clicking the Up Down switches
volatile long oldradix = 0;                     // the previous radix
volatile int mode = 0;                          // the current mode (0=LSB, 1=USB)
volatile int oldmode = 0;                       // the previous mode (0=LSB, 1=USB)
volatile int TX = 0;                            // 0=RX, 1=TX
volatile int oldTX = 1;                         // the old TX
unsigned int encoderA, encoderB, encoderC = 1;  // rotary encoder variables

// Rotary encoder pins and other inputs
static const int rotAPin = 2;
static const int rotBPin = 3;
static const int radixPin = 4;
static const int modePin = 5;
static const int PTTPin = 6;

// Instantiate the Objects
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7);    //I2C Address 0x27 (or 0x3F)
Si5351 si5351(0x60); //I2C Address 0x60


void setup()
{
  // Set up I/O pins
  pinMode(rotAPin, INPUT);
  digitalWrite(rotAPin, HIGH);                    // internal pull-up enabled
  pinMode(rotBPin, INPUT);
  digitalWrite(rotBPin, HIGH);                    // internal pull-up enabled
  pinMode(radixPin, INPUT);
  digitalWrite(radixPin, HIGH);                   // internal pull-up enabled
  pinMode(modePin, INPUT);
  digitalWrite(modePin, HIGH);                    // internal pull-up enabled
  pinMode(PTTPin, INPUT);
  digitalWrite(PTTPin, LOW);                      // internal pull-up disabled

  // Initialize the display
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);

  // Initialize the DDS
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 62100);         // 62100 is the specific calibration factor for this Si5351 board
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);
}

void loop()
{
  CheckEncoder();
  CheckRadixSwitch();
  CheckModeSwitch();
  CheckPTTPin();
}

void CheckEncoder()
{
  byte encoderA = digitalRead(rotAPin);
  byte encoderB = digitalRead(rotBPin);

  if ((encoderA == HIGH) && (encoderC == LOW))
  {
    if (encoderB == HIGH)
      // Decrease frequency
      freq = constrain(freq - radix, bandStart, bandEnd);
    else
      // Increase frequency
      freq = constrain(freq + radix, bandStart, bandEnd);
  }
  encoderC = encoderA;

  if (freq != oldfreq)
  {
    UpdateDisplay();
    SendFrequency();
    oldfreq = freq;
  }
}

void CheckRadixSwitch()
{
  if (digitalRead(radixPin) == 0)
  {
    radix = radix / 10;
    if (radix < 1)
      radix = 1000;
    delay(200);
  }

  if (radix != oldradix)
  {
    UpdateRadixDisplay();
    oldradix = radix;
  }
}

void CheckModeSwitch()
{
  if (digitalRead(modePin) == 0)
    mode = 0;                                 // 0=LSB
  if (digitalRead(modePin) == 1)
    mode = 1;                                 // 1=USB

  if (mode != oldmode)
  {
    UpdateDisplay();
    SendFrequency();
    oldmode = mode;
  }
}

void CheckPTTPin()
{
  TX = digitalRead(PTTPin);
  if (TX != oldTX)
  {
    UpdateDisplay();
    SendFrequency();
    oldTX = TX;
  }
}

void UpdateDisplay()
{
  // freq
  lcd.setCursor(0, 0);
  lcd.print(freq);
  // mode
  lcd.setCursor(0, 1);
  if (mode == 0)
    lcd.print("LSB");
  else
    lcd.print("USB");
  lcd.setCursor(10, 1);
  // PTT
  lcd.setCursor(4, 1);
  if (TX == 1)
    lcd.print("TX");
  if (TX == 0)
    lcd.print("RX");
  // callsign
  lcd.setCursor(10, 1);
  lcd.print("ZL2CTM");
}

void UpdateRadixDisplay()
{
  // radix
  if (radix == 1000)
  {
    lcd.setCursor(9, 0);
    lcd.print("      ");
    lcd.setCursor(9, 0);
    lcd.print(radix);
    lcd.setCursor(14, 0);
    lcd.print("Hz");
  }
  if (radix == 100)
  {
    lcd.setCursor(9, 0);
    lcd.print("      ");
    lcd.setCursor(10, 0);
    lcd.print(radix);
    lcd.setCursor(14, 0);
    lcd.print("Hz");
  }
  if (radix == 10)
  {
    lcd.setCursor(9, 0);
    lcd.print("      ");
    lcd.setCursor(11, 0);
    lcd.print(radix);
    lcd.setCursor(14, 0);
    lcd.print("Hz");
  }
  if (radix == 1)
  {
    lcd.setCursor(9, 0);
    lcd.print("      ");
    lcd.setCursor(12, 0);
    lcd.print(radix);
    lcd.setCursor(14, 0);
    lcd.print("Hz");
  }
}

void SendFrequency()
{
  if (mode == 0)                                                          // LSB
  {
    if (TX == 1)                                                          // Transmit
    {
      si5351.set_freq(((LSB_BFO_freq + freq) * 100ULL), SI5351_CLK2);     // VFO
      si5351.set_freq((LSB_BFO_freq * 100ULL), SI5351_CLK0);              // BFO
    }
    else                                                                  // Receive
    {
      si5351.set_freq(((LSB_BFO_freq + freq) * 100ULL), SI5351_CLK0);     // VFO
      si5351.set_freq((LSB_BFO_freq * 100ULL), SI5351_CLK2);              // BFO
    }
  }

  if (mode == 1)                                                          // USB
  {
    if (TX == 1)                                                          // Transmit
    {
      si5351.set_freq(((USB_BFO_freq + freq) * 100ULL), SI5351_CLK2);     // VFO
      si5351.set_freq((USB_BFO_freq * 100ULL), SI5351_CLK0);              // BFO
    }
    else                                                                  // Receive
    {
      si5351.set_freq(((USB_BFO_freq + freq) * 100ULL), SI5351_CLK0);     // VFO
      si5351.set_freq((USB_BFO_freq * 100ULL), SI5351_CLK2);              // BFO
    }
  }
}
