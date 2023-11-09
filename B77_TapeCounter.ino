/*
  __ __|                     ___|                       |                    __ ) ___  | ___  |
    |   _` |  __ \    _ \  |       _ \   |   |  __ \   __|   _ \   __|      __ \     /      /
    |  (   |  |   |   __/  |      (   |  |   |  |   |  |     __/  |         |   |   /      /
   _| \__,_|  .__/  \___| \____| \___/  \__,_| _|  _| \__| \___| _|        ____/  _/     _/
             _|

   ARDUINO OLED TAPE COUNTER FOR REVOX B77 REEL-TO-REEL MACHINES
   based on
   Copyright (C) 2020 by DIYLAB, v0.99-rc1, 30.05.2020
   Version 2.0, October 2023 modified by Marc Stähli
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   This Software is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This Software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Arduino SSD1306Ascii Library.  If not, see
   <http://www.gnu.org/licenses/>.

   ################################################################################################
   # Many thanks for the great interest of the colleagues from the                                #
   # 'Old Fidelity Forum' https://old-fidelity-forum.de/thread-38940.html                         #
   # and https://old-fidelity-forum.de/thread-38716.html                                          #
   # and especially for the fantastic help of the user 'gogosch'!                                 #
   ################################################################################################

   Used libraries
   --------------
   Arduino OneButton Library: <https://github.com/mathertel/OneButton>
   SSD1306Ascii: <https://github.com/greiman/SSD1306Ascii>

   ###############################################################################################

   Update & Modifications in October 23 for B77:

   All modifications done especially for a B77 MKI - HiFi Labor rebuild
   hifilabor.ch, Marc Stähli - Vielen Dank für die tolle und solide Arbeit!

   Using an Arduino Nano with igital pins on 5V (!ESP pins are 3.3V!)
   Using two HW-006 V.1.3 Line Tracker Sensor Modules and two white segmens on right spooling
   motor feeding Pin 2 and 3 at the Nano for interrupts
   Using one 360 Degree Rotary Encoder EC12 RE1 for button push and turning

   - slimmed down code for single use of B77 (MKI) and only one font
   - Transistor Q1 on Capstan Speed Control PCP (Collector@Q1) delivers 12 V at 7.5 ips and
     0 V at 3.75 ips, fed into SPEED_Pin via voltage divider R1 10 KOhm, R2 6.8 KOhm = 4.8 V
   - added display offset for x and y for fitting purposes
   - added rotary encoder/switch combination
      - knob rotation lets you switch between counter, meter and seconds
      - double click shows total running time
   - found a compromize with "real-time" settings measuring at the right reel
   - hooked Nano Pin (9) to Pause PCB from Mauro200id - ebay (PIC foot 6  --> is low on pause)fj
   - added rewind2zero functionality  - 2 relais hooked to Nano pin 10 and 11 controlling
     the connection of 24V from the remote control pins to pin 3 and 6 in order to do rewind and 
     stop.
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
// USER CONFIG SECTION (please only edit here!)                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////

// Software configuration:
#define SW_VERSION 2.0       // Current version of the software
#define HELLO_LOGO true      // Show logo picture.
#define HELLO_TEXT false     // Show welcoming text (search for 'welcome helloText').
#define HELLO_TIMEOUT 3000   // The display duration of the greeting in ms.
#define BRIGHTNESS 128       // Display brightness (0 to 255).
#define FLIPDISPLAY true     // Set the display to normal (false) oder 180 degree mode (true).
#define INVERTDISPLAY false  // Set inverted display (true) or normal dispalay (false).
#define UNSIGN false         // Minus sign (true: sign not shown, false: sign shown).
#define DIGITS_MINUTES 3     // Digits for the minute display in realtime mode (2 or 3).
#define DIGITS_COUNTER 5     // Number of digits for normal counter (FontSize1: 4, 5 or 6 - FontSize2: 4 or 5).
#define PULSES_COUNTER 2     // Number of pulses for one count in counter mode (1 to 255).
#define OPHOURSOFFSET 0      // Offset for operating hours counter in seconds. Example: 1261440000 = 40 YEARS ;o)
#define OLED_X_OFFSET 2      // Offset for shifting the OLED output in X direction (e.g. + 1 = shift 1 pixel to the right)
#define OLED_Y_OFFSET 0      // Offset for shifting the OLED output in Y direction (e.g. -1 = shift 8 pixels up)

// Meter and realtime preferences by gogosch ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SPEEDHIGH 381.0    // Highest belt speed 15" in mm/s
#define SPEEDMIDDLE 190.5  // Middle  belt speed 7,5" in mm/s
#define SPEEDLOW 95.25     // Lowest  belt speed 3,75" in mm/s
#define NUMSEGS 4          // Segments on the tape reel
#define SCOPE 580          // Ment to be circumference, but more a trimming value

// Further adjustments (only  change if you know what you are doing!).~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SENSOR_A 3            // INPUT PIN (Sensor-A) 'INT0'.
#define SENSOR_B 2            // INPUT PIN (Sensor-B)	'INT1'.
#define BUTTON 5              // INPUT PIN (RESET-Button).
#define ROTARY_A 6            // Rotary Switch A signal in
#define ROTARY_B 7            // Rotary Switch B signal in
#define SPEED_PIN 8           // Speed Input pin after Voltage Divider (high = 7.5 ips, low = 3.75 ips)
#define LED 13                // Onboard LED.
#define EEPROM_MODE 10        // EEPROM address for mode.
#define EEPROM_BRIGHTNESS 20  // EEPROM address for brightness.
#define EEPROM_COUNTER 100    // EEPROM address for counter data.
#define EEPROM_METER 200      // EEPROM address for meter data.
#define EEPROM_REALTIME 300   // EEPROM address for realtime data.
#define EEPROM_OPHOURS 400    // EEPROM address for operating hour counter data.
#define I2C_ADDRESS 0x3C      // I2C display address (0x3C/0x3D, depending on display type).
#define PAUSE_PIN 9           // Pin goes low when PAUSE is engaged
#define REW_PIN 10            // Triggers Relais to Rewind
#define STOP_PIN 11           // Triggers Relais to Stop

//////////////////////////////////////////////////////////////////////////////////////////////////
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <OneButton.h>
#include <Encoder.h>
#include <EEPROM.h>
#include <SSD1306Ascii.h>
#include "SSD1306AsciiWire.h"
#include "tapecounter_16x24.h"
#include "tapecounter_21x32.h"

#if HELLO_LOGO
#include "logo_mst1.h"
#endif

// Time constants.
#define SECS_PER_MIN (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY (SECS_PER_HOUR * 24L)

// Macros for getting elapsed time.
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfMinutesEx(_time_) (_time_ / SECS_PER_MIN)
#define numberOfHours(_time_) ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) (_time_ / SECS_PER_DAY)

OneButton btn(BUTTON, true, true);
Encoder rotaryEncoder(ROTARY_A, ROTARY_B);

SSD1306AsciiWire oled;

// Struct for welcome message.
typedef struct
{
  char* msg;
  byte col;
  byte row;
} welcome;

/**************************************************************************/
/*!
	Here you define the welcome text. A maximum of 2 lines is possible.
	Syntax: Message, column (0 to 128), row (1 or 2).
	If a line is not used, it is commented out.
*/
/**************************************************************************/
welcome helloText[2] = {
  /*Line1*/ { "HiFiLabor.ch", 52 + OLED_X_OFFSET, 1 },
  /*Line2*/ { "COUNTER 2.0", 52 + OLED_X_OFFSET, 2 },
};

volatile uint32_t operatingHoursCounter, saveCounter, intervalA, intervalB, intervalC, intervalD, intervalE, intervalF, intervalM;
volatile bool ledTrigger, speedTrigger, pauseTrigger, lockDisplay, pastPinA, pinA;
volatile byte mode, loopCounter;
volatile byte brightness = BRIGHTNESS;
/////////////////////////////// Rew2Zero VARS //////////////////////////////
String movement;
bool rewindToZero = false;
long counterStart, prevCounter;
/////////////////////////////// COUNTER VARS ///////////////////////////////
long counter, pastCounter, longCounterSeconds, pastLongCounterSeconds;
float counterMeters, pastCounterMeters, counterSeconds, pastCounterSeconds, secsPerPulse, mmPerPulse, speedActual, pastSpeedActual;
float numSegs = NUMSEGS;
float scope = SCOPE;
////////////////////////////////////////////////////////////////////////////

/**************************************************************************/
/*!
   @brief   Setup
*/
/**************************************************************************/
void setup() {
  Serial.begin(9600);
  // Set inputs.
  pinMode(SENSOR_A, INPUT);
  pinMode(SENSOR_B, INPUT);
  pinMode(SPEED_PIN, INPUT);
  pinMode(PAUSE_PIN, INPUT);
  // Set outputs.
  pinMode(LED, OUTPUT);
  pinMode(REW_PIN, OUTPUT);
  pinMode(STOP_PIN, OUTPUT);

  // Set pinchange interrupts.
  attachInterrupt(0, CheckState, CHANGE);
  attachInterrupt(1, CheckState, CHANGE);
  // Link the button functions.
  btn.attachClick(ButtonClick);
  btn.attachDoubleClick(ButtonDoubleClick);
  btn.attachLongPressStart(ButtonLongPressStart);
  btn.attachLongPressStop(ButtonLongPressStop);
  btn.attachDuringLongPress(ButtonLongPress);
  // Initialize display.
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  // Set display brightness.
  oled.setContrast(BRIGHTNESS);
  // Set display to normal (false) oder 180 degree mode (true).
  oled.displayRemap(FLIPDISPLAY);
  // Set inverted display (true) or normal dispalay (false).
  oled.invertDisplay(INVERTDISPLAY);
  // Clear the display and set the cursor to (0,0).
  oled.clear();

#if HELLO_LOGO
  // For a 128x32 image.
  byte r = 0;  // Start row
  byte c = 0;  // Start col
  int a = 0;   // Position in array

  for (byte b = 0; b < 4; b++) {
    oled.setCursor(c, (r + b));
    for (byte i = 0; i < 128; i++) {
      oled.ssd1306WriteRam(pgm_read_byte(&logo[a]));
      a++;
    }
  }
#endif

#if HELLO_TEXT
  // Set message font, cursor and text.
  oled.setFont(utf8font10x16);
  for (size_t i = 0; i < (sizeof(helloText) / sizeof(helloText[0])); i++) {
    oled.setCursor(helloText[i].col + OLED_X_OFFSET, (helloText[i].row == 1) ? 0 : 2);
    oled.print(helloText[i].msg);
  }
#endif

#if HELLO_LOGO || HELLO_TEXT
  // Display specific time.
  delay(HELLO_TIMEOUT);

  // Clear the display and set the cursor to (0,0).
  oled.clear();
#endif

  // Get last realtime data from the EEPROM.
  EEPROM.get(EEPROM_OPHOURS, operatingHoursCounter);

  // Show the operating hours.
  if (digitalRead(BUTTON) == LOW) {
    char opHoursBuffer[48];
    opHoursSecondsToString(opHoursBuffer, operatingHoursCounter + OPHOURSOFFSET);
    oled.setFont(utf8font10x16);
    oled.setCursor(0 + OLED_X_OFFSET, 0 + OLED_Y_OFFSET);
    oled.print(opHoursBuffer);
    // Waiting for release the button.
    while (digitalRead(BUTTON) == LOW) {}
    oled.clear();
  }
  // Initialize font size
  oled.setFont(tapecounter_16x24);
  // Initialization of the virgin EEPROM at the very first start.
  if (EEPROM.read(1000) == 255 && EEPROM.read(1001) == 255) {
    // Successful deflowered ;o)
    EEPROM.write(1000, 0);
    EEPROM.write(1001, 0);
    // Initialize EEPROM.
    EEPROM.write(EEPROM_MODE, 0);
    EEPROM.write(EEPROM_BRIGHTNESS, BRIGHTNESS);
    SaveCounter();
  }

  // Get last mode from the EEPROM.
  mode = EEPROM.read(EEPROM_MODE);
  // Get last brightness from the EEPROM.
  brightness = EEPROM.read(EEPROM_BRIGHTNESS);
  // Get last counter data from the EEPROM.
  EEPROM.get(EEPROM_COUNTER, counter);
  // Get last meter data from the EEPROM.
  EEPROM.get(EEPROM_METER, counterMeters);
  // Get last realtime data from the EEPROM.
  EEPROM.get(EEPROM_REALTIME, counterSeconds);
  // Counter overflow calculation.
  OverflowCalculation();
  // Calculation of the display parameters for realtime and counter.
  CalculatingDisplayParameters();
  // Initialize timer1 for one second pulses.
  setupTimer1();
}

/**************************************************************************/
/*!
   @brief   MainLoop
*/
/**************************************************************************/

long encPos = rotaryEncoder.read();

void loop() {
  // Watching the push button.
  btn.tick();

  // Watching the encoder
  long newPos = rotaryEncoder.read();
  if (newPos != encPos) {
    if (newPos % 4 == 0) {
      if (newPos > encPos) {
        EncoderRotated(1);  // cw
        encPos = newPos;
      }
      if (newPos < encPos) {
        EncoderRotated(0);  // ccw
        encPos = newPos;
      }
    }
  }

  // Watching the pause button
  if (digitalRead(PAUSE_PIN) == LOW && pauseTrigger == false) {
    oled.clear();
    CenterStringAndWrite("PAUSE", 1);
    pauseTrigger = true;
  } else if (digitalRead(PAUSE_PIN) == HIGH && !lockDisplay) {
    RefreshDisplay(true);
    pauseTrigger = false;
  }

  // Detect movement
  if (millis() > intervalM + 500) {
    intervalM = millis();
    if (prevCounter == counter) {
      movement = "still";
    } else {
      movement = "runs";
    }
    prevCounter = counter;
  }

  // Rewind to zero procedure
  if (rewindToZero && counter > 0) {
    if (movement == "still") {
      counterStart = counter;
      doRewind();
    }
    if (counter == 100 && counterStart != counter) doStop();
    if (counter == 40 && counterStart != counter) doStop();
    if (counter == 10 && counterStart != counter) doStop();
    if (counter == 1) {
      doStop();
      rewindToZero = false;
    }
  }

  // This section is executed every 30ms.
  if (millis() > intervalA + 30) {
    intervalA = millis();

    // Calculation of the display parameters for realtime and counter.
    CalculatingDisplayParameters();

    // Refresh display if no locked.
    if (!lockDisplay) RefreshDisplay(false);
  }

  // Turn onboard LED off.
  if (ledTrigger && (millis() - intervalC > 500)) {
    ledTrigger = false;
    digitalWrite(LED, LOW);
  }

  // Reset speeddisplay.
  if (speedTrigger && (millis() - intervalD > 2000)) {
    speedTrigger = false;

    // Was the display locked from the previous speed display?
    if (lockDisplay) {
      // Clear the display and set the cursor to (0,0).
      oled.clear();
      // Releases the display.
      lockDisplay = false;
      // Reload the pause display monoflop
      pauseTrigger = false;
    }
  }

  // Save current counter data.
  if (saveCounter == 4) {
    saveCounter++;
    // Trigger the onboard LED monoflop.
    ledTrigger = true;
    intervalC = millis();
    // Turn onboard LED on.
    digitalWrite(LED, HIGH);
    // Save current counter data.
    SaveCounter();
  }
}
/**************************************************************************/
/*!
   @brief   Helper function: Trigger the STOP button
*/
/**************************************************************************/
void doStop() {
  if (millis() > intervalF + 250) {  // avoiding double-triggering
    intervalF = millis();
    digitalWrite(STOP_PIN, HIGH);
    delay(10);
    digitalWrite(STOP_PIN, LOW);
  }
}

/**************************************************************************/
/*!
   @brief   Helper function: Trigger the REWIND button
*/
/**************************************************************************/
void doRewind() {
  if (millis() > intervalB + 1500) {  // avoiding double-triggering
    intervalB = millis();
    digitalWrite(REW_PIN, HIGH);
    delay(10);
    digitalWrite(REW_PIN, LOW);
  }
}

/**************************************************************************/
/*!
   @brief   Save counter data to EEPROM.
*/
/**************************************************************************/
void SaveCounter() {
  // Write counter data to EEPROM.
  EEPROM.put(EEPROM_COUNTER, counter);
  // Write meter data to EEPROM.
  EEPROM.put(EEPROM_METER, counterMeters);
  // Write realtime data to EEPROM.
  EEPROM.put(EEPROM_REALTIME, counterSeconds);
  // Write operating hours data to EEPROM.
  EEPROM.put(EEPROM_OPHOURS, operatingHoursCounter);
}

/**************************************************************************/
/*!
   @brief   This function will be called when encoder turned
            sense_up is true
*/
/**************************************************************************/
void EncoderRotated(bool upsense) {
  if (upsense) {
    mode++;
    if (mode > 2)
      mode = 0;
  }
  if (!upsense) {
    mode--;
    if (mode == 255)  // because mode is defined as a byte
      mode = 2;
  }
  // Save current mode to the EEPROM.
  EEPROM.update(EEPROM_MODE, mode);

  oled.clear();
  // Refresh display .
  RefreshDisplay(true);
}

/**************************************************************************/
/*!
   @brief   This function will be called when the button was pressed 1 time.
*/
/**************************************************************************/
void ButtonClick() {
  // Resetting all counters.
  counter = 0;
  counterMeters = 0;
  counterSeconds = 0;
  // Save current counter data.
  SaveCounter();
  // Clear the display and set the cursor to (0,0).
  oled.clear();
  // Refresh display .
  RefreshDisplay(true);
}

/**************************************************************************/
/*!
   @brief   This function will be called when the button
			was pressed 2 times in a short timeframe. Initiates the rewind to zero
      procedure in setting the flag to true
*/
/**************************************************************************/
void ButtonDoubleClick() {
  if (counter > 0) {
    rewindToZero = true;
    movement = "still";  // trick the procedure to start rewinding
  }
}
/**************************************************************************/
/*!
   @brief   This function will be called often,
			while the button is pressed for a long time.
*/
/**************************************************************************/
void ButtonLongPress() {
  // This section is executed every 300ms.
  if (millis() > intervalE + 300) {
    intervalE = millis();

    // The brightness is adjusted in 16 steps.
    brightness++;
    if (brightness > 16)
      brightness = 0;

    // Charbuffer for indicator.
    char buffer[16];
    // Set cursor to right side in line 2.
    oled.setCursor(75 + OLED_X_OFFSET, 2 + OLED_Y_OFFSET);

    // Brightness indicator left and right side.
    for (size_t i = 0; i < brightness; i++) {
      // Left side: set indicator.
      buffer[i] = 133;  // Indicator char.
    }
    for (size_t i = brightness; i < 16; i++) {
      // Right side: clear indicator.
      buffer[i] = 32;  // Space char.
    }

    // Print the buffer on display.
    oled.print(buffer);
    // Setting the brightness * 16 (0 to 256).
    oled.setContrast(brightness * 16);
  }
}

/**************************************************************************/
/*!
   @brief   This function will be called once, when the button is
			pressed for a long time.
*/
/**************************************************************************/
void ButtonLongPressStart() {
  // Lock the display for further output.
  lockDisplay = true;
  // Set font for indicator.
  oled.setFont(utf8font10x16);
  // Clear the display and set the cursor to (0,0).
  oled.clear();
  // Set cursor to left side in line 2.
  oled.setCursor(0 + OLED_X_OFFSET, 2 + OLED_Y_OFFSET);
  // Write message on left side.
  oled.println("BRIGHTNESS:");
}

/**************************************************************************/
/*!
   @brief   This function will be called once, when the button is
			released after beeing pressed for a long time.
*/
/**************************************************************************/
void ButtonLongPressStop() {
  // Save current brightness to the EEPROM.
  EEPROM.update(EEPROM_BRIGHTNESS, brightness);

  // BlingBling ;o)
  for (size_t i = 0; i < 2; i++) {
    oled.setCursor(75 + OLED_X_OFFSET, 2 + OLED_Y_OFFSET);
    oled.print("STORED            ");
    delay(250);
    oled.setCursor(75 + OLED_X_OFFSET, 2 + OLED_Y_OFFSET);
    oled.print("                  ");
    delay(250);
  }
  // Reset Font size
  oled.setFont(tapecounter_16x24);
  // Clear the display and set the cursor to (0,0).
  oled.clear();
  // Releases the display.
  lockDisplay = false;
  // Refresh display .
  RefreshDisplay(true);
}

/**************************************************************************/
/*!
   @brief   Refresh display by different value or forced.
*/
/**************************************************************************/
void RefreshDisplay(boolean force) {
  // Converts float seconds to long.
  longCounterSeconds = (long)counterSeconds;

  // Counter overflow calculation.
  OverflowCalculation();

  // The refresh of the display is forced.
  if (force) {
    // Counter
    if (mode == 0) {
      WriteOled(counter);
    }
    // Meter
    else if (mode == 1) {
      WriteOled(counterMeters);
    }
    // Realtime
    else if (mode == 2) {
      WriteOledRealTime(longCounterSeconds);
    }
  }
  // Only refresh if there is a different value.
  else {
    // Counter
    if (mode == 0 && counter != pastCounter) {
      pastCounter = counter;
      WriteOled(counter);
    }
    // Meter
    else if (mode == 1 && counterMeters != pastCounterMeters) {
      pastCounterMeters = counterMeters;
      WriteOled(counterMeters);
    }
    // Realtime
    else if (mode == 2 && longCounterSeconds != pastLongCounterSeconds) {
      pastLongCounterSeconds = longCounterSeconds;
      WriteOledRealTime(longCounterSeconds);
    }
  }
}

/**************************************************************************/
/*!
   @brief   Write on OLED display (counter mode).
   @param   val   counter data as long
*/
/**************************************************************************/
void WriteOled(long val) {

#if DIGITS_COUNTER == 4
  // Char buffer.
  char buf[5];
  // Increase space between letters.
  oled.setLetterSpacing(4);
#if UNSIGN
  // Formatting the output.
  sprintf(buf, "%04ld", (0 > val) ? 10000 + val : val);
  // Set cursor (col, row).
  oled.setCursor(26 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#else
  // Formatting the output.
  sprintf(buf, (val < 0) ? "-%04ld" : " %04ld", (val > 0) ? val : -val);
  // Set cursor (col, row).
  oled.setCursor(15 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#endif

#elif DIGITS_COUNTER == 5
  // Char buffer.
  char buf[6];
  // Increase space between letters.
  oled.setLetterSpacing(4);
#if UNSIGN
  // Formatting the output.
  sprintf(buf, "%05ld", (0 > val) ? 100000 + val : val);
  // Set cursor (col, row).
  oled.setCursor(16 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#else
  // Formatting the output.
  sprintf(buf, (val < 0) ? "-%05ld" : " %05ld", (val > 0) ? val : -val);
  // Set cursor (col, row).
  oled.setCursor(4 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#endif

#else  // More of 5 digits or less than 4 digits.

#if UNSIGN
  // Char buffer.
  char buf[6];
  // Formatting the output.
  sprintf(buf, "%06ld", (0 > val) ? 1000000 + val : val);
  // Increase space between letters.
  oled.setLetterSpacing(4);
  // Set cursor (col, row).
  oled.setCursor(6 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#else
  // Char buffer.
  char buf[7];
  // Formatting the output.
  sprintf(buf, (val < 0) ? "-%06ld" : " %06ld", (val > 0) ? val : -val);
  // Increase space between letters.
  oled.setLetterSpacing(3);
  // Set cursor (col, row).
  oled.setCursor(0 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#endif
#endif  // END: DIGITS_COUNTER 4 & 5	& 6
  // Output on the display.
  oled.println(buf);
}

/**************************************************************************/
/*!
   @brief   Write on OLED display (meter mode), overloaded.
   @param   val   counter data as float
*/
/**************************************************************************/
void WriteOled(float val) {
  uint32_t intVal = fabsf(val) * 10;
  uint32_t pre = intVal / 10;
  uint32_t past = intVal % 10;

  // Buffer for digits.
  char buf[9];
  // Formatting the output.
  sprintf(buf, (val > -0.1f) ? "m %04lu.%01lu" : "m-%04lu.%01lu", pre, past);
  // Increase space between letters.
  oled.setLetterSpacing(2);
  // Set cursor (col, row).
  oled.setCursor(0 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
  // Output on the display.
  oled.println(buf);
}

/**************************************************************************/
/*!
   @brief   Write on OLED display (realtime mode), overloaded.
   @param   secs   seconds as long
*/
/**************************************************************************/
void WriteOledRealTime(long secs) {
  // Buffer for digits.
  char buf[8];
#if DIGITS_MINUTES == 3
  // Convert seconds to formatted output (000"00).
  (secs < 0) ? SecondsToString(buf, secs, "-%03d'%02d") : SecondsToString(buf, secs, " %03d'%02d");
  // Increase space between letters.
  oled.setLetterSpacing(4);
  // Set cursor (col, row).
  oled.setCursor(5 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#else
  // Convert seconds to formatted output (0.00"00).
  (secs < 0) ? SecondsToString(buf, secs, "-%01d:%02d'%02d") : SecondsToString(buf, secs, " %01d:%02d'%02d");
  // Increase space between letters.
  oled.setLetterSpacing(4);
  // Set cursor (col, row).
  oled.setCursor(1 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);
#endif
  // Output on the display.
  oled.println(buf);
}

/**************************************************************************/
/*!
   @brief   Converts seconds to a human readable time format.
   @param   buffer   string buffer
   @param   secs     seconds
   @param   format   formatting string
*/
/**************************************************************************/
void SecondsToString(char* buffer, long secs, char* format) {
  secs = (secs > 0) ? secs : -secs;
  int seconds = numberOfSeconds(secs);
#if DIGITS_MINUTES == 3
  int minutes = numberOfMinutesEx(secs);
  sprintf(buffer, format, minutes, seconds);
#else
  int minutes = numberOfMinutes(secs);
  int hours = numberOfHours(secs);
  sprintf(buffer, format, hours, minutes, seconds);
#endif
}

/**************************************************************************/
/*!
   @brief   Converts operating hours seconds to a human readable time format.
   @param   buffer   string buffer
   @param   secs     seconds
*/
/**************************************************************************/
void opHoursSecondsToString(char* buffer, uint32_t secs) {
  int days = elapsedDays(secs);
  int hours = numberOfHours(secs);
  int minutes = numberOfMinutes(secs);
  int seconds = numberOfSeconds(secs);
  sprintf(buffer, "WORKTIME: %d DAYS\n%02d HRS - %02d MIN - %02d SEC", days, hours, minutes, seconds);
}

/**************************************************************************/
/*!
   @brief   Initialize Timer1 for second pulses.
*/
/**************************************************************************/
void setupTimer1() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  // 1 Hz (16000000/((15624+1)*1024)).
  OCR1A = 15624;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 1024.
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // Output Compare Match A Interrupt Enable.
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

/**************************************************************************/
/*!
   @brief   Timer Overflow Entry Vector.
*/
/**************************************************************************/
ISR(TIMER1_COMPA_vect) {
  // Increase the operating hours counter.
  operatingHoursCounter++;
  // Increase the save counter.
  saveCounter++;

  // Write operating hours data to EEPROM every 10 minutes.
  if (operatingHoursCounter % 600 == 0)
    EEPROM.put(EEPROM_OPHOURS, operatingHoursCounter);
}

/**************************************************************************/
/*!
   @brief   Check sensor state after pinchange interrupt.
*/
/**************************************************************************/
void CheckState() {
  pinA = digitalRead(SENSOR_A);
  if ((pastPinA == LOW) && (pinA == HIGH)) {
    if (digitalRead(SENSOR_B) == LOW) {
      if (loopCounter == PULSES_COUNTER) {
        loopCounter = 0;
        counter++;
      };
      counterMeters += mmPerPulse;
      counterSeconds += secsPerPulse;
    } else {
      if (loopCounter == PULSES_COUNTER) {
        loopCounter = 0;
        counter--;
      };
      counterMeters -= mmPerPulse;
      counterSeconds -= secsPerPulse;
    }
    // Reset save counter.
    saveCounter = 0;
    // Increase the loop counter.
    loopCounter++;
  }
  // Hold pinA.
  pastPinA = pinA;
}

/**************************************************************************/
/*!
   @brief   Calculation of the counter overflow.
*/
/**************************************************************************/
void OverflowCalculation() {
  // Initiate normalcounter overflow depending on the number of digits.
  long tempCounter = abs(counter);
#if DIGITS_COUNTER == 4
  if (tempCounter > 9999)
    counter = 0;
#elif DIGITS_COUNTER == 5
  if (tempCounter > 99999)
    counter = 0;
#else
  if (tempCounter > 999999)
    counter = 0;
#endif
  // Initiate meter counter overflow
  if (counterMeters < -2000 || counterMeters > 2000)
    counterMeters = 0;

  // Initiate realtime counter overflow
  if (counterSeconds < -35999 - secsPerPulse || counterSeconds > 35999 + secsPerPulse)
    counterSeconds = 0;
}

/**************************************************************************/
/*!
   @brief   Calculation of the display parameters for realtime and counter.
*/
/**************************************************************************/
void CalculatingDisplayParameters() {
  // Polling speed inputs.
  PollingSpeedInputs();
  // Calculating for meter and realtime.
  CalculatingSpeed();
  // Only execute if the value 'speedActual' is changed.
  if (speedActual != pastSpeedActual) {
    // Trigger the speed display monoflop
    speedTrigger = true;
    intervalD = millis();
    // Hold speed.
    pastSpeedActual = speedActual;
    // Lock the display for further output.
    lockDisplay = true;
    // Clear the display and set the cursor to (0,0).
    oled.clear();
    // Increase space between letters.
    oled.setLetterSpacing(2);
    // Set cursor.
    oled.setCursor(0 + OLED_X_OFFSET, 1 + OLED_Y_OFFSET);

    if (speedActual == SPEEDLOW) {
      CenterStringAndWrite("3,75 LS", 1);
    } else if (speedActual == SPEEDMIDDLE) {
      CenterStringAndWrite("7,5 STD", 1);
    } else if (speedActual == SPEEDHIGH) {
      CenterStringAndWrite("38 HS", 1);
    } else {
      CenterStringAndWrite("SPDERR", 1);
    }
  }
}

/**************************************************************************/
/*!
   @brief   Center string and write to display.
   @param   msg   center message
   @param   row   display row
*/
/**************************************************************************/
void CenterStringAndWrite(char* msg, byte row) {
  size_t strWidth = oled.strWidth(msg);
  oled.setCursor(((128 - strWidth) / 2) + OLED_X_OFFSET, row + OLED_Y_OFFSET);
  oled.println(msg);
}

/**************************************************************************/
/*!
   @brief   Polling speed input. B77 Collector Q1 Capstan Speed Control
            PCP delivers 12 V at 7.5 ips and 0 V at 3.75 ips, fed into
            SPEED_Pin via voltage divider R1 10 KOhm, R2 6.8 KOhm = 4.8 V
*/
/**************************************************************************/
void PollingSpeedInputs() {
  // Default speed is the lowest available speed.
  //speedActual = SPEEDLOW;

  boolean a1 = digitalRead(SPEED_PIN);

  if (a1 == HIGH) speedActual = SPEEDMIDDLE;   // 7.5 ips
  else if (a1 == LOW) speedActual = SPEEDLOW;  // 3.75 ips
}

/**************************************************************************/
/*!
   @brief   Calculating for meter and realtime by gogosch
*/
/**************************************************************************/
void CalculatingSpeed() {

  // Seconds in internal counter always SPEEDLOW.
  secsPerPulse = scope / (SPEEDLOW * numSegs);
  secsPerPulse = secsPerPulse * SPEEDLOW / speedActual;  // fix by gogosch
  // Conversion to meters per pulse.
  mmPerPulse = secsPerPulse * speedActual / 1000;
}
