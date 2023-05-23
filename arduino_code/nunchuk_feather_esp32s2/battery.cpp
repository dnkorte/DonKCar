/*
 * Summary: remote controller for robots/racers uses nunchuk as actuator
 *    communicates to robot/racer using ESP-NOW protocol; this version
 *    runs on an Adafruit QTPy ESP32-S2 and has an oled display
 * 
 * Author(s):  Don Korte
 * Repository: https://github.com/dnkorte/DonKCar
 *
 * MIT License
 * Copyright (c) 2023 Don Korte
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * 0000000
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. * 
 */
#include "battery.h"
#include "screen_tft.h"


/*
 * note per reverse TFT guide the board may come with either of 2 different
 * battery monitor chips, depending on supply.  we provide for both of them
 * https://learn.adafruit.com/esp32-s2-reverse-tft-feather/power-management
 */
#include "Adafruit_MAX1704X.h"
#include "Adafruit_LC709203F.h"
Adafruit_MAX17048 maxlipo;
Adafruit_LC709203F lc;
bool addr0x36 = true;     // indicate we have a 17048 chip


void batt_init() {
  /*  
   * setup battery monitor chip(s) - note per the reverse TFT guide, the board
   * may come with either of 2 different chips, depending upon supply.
   * we provide for both of them here
   * https://learn.adafruit.com/esp32-s2-reverse-tft-feather/power-management
   */
  if (!maxlipo.begin()) {
    DEBUG_PRINTLN(F("Couldnt find Adafruit MAX17048, looking for LC709203F.."));
    // if no lc709203f..
    if (!lc.begin()) {
      DEBUG_PRINTLN(F("Couldnt find Adafruit MAX17048 or LC709203F."));
      while (1) delay(10);
    }
    // found lc709203f!
    else {
      addr0x36 = false;
      DEBUG_PRINTLN(F("Found LC709203F"));
      lc.setThermistorB(3950);
      lc.setPackSize(LC709203F_APA_500MAH);
      lc.setAlarmVoltage(3.8);
    }
  // found max17048!
  } else {
    addr0x36 = true;
    DEBUG_PRINTLN(F("Found MAX17048"));
  }
}

float read_batt() { 
  if (addr0x36 == true) {
    //max17048();
    return maxlipo.cellVoltage();
  }
  else {
    //lc709203f();
    return lc.cellVoltage();
  }

}


/*
 * private functions
 */
 
void lc709203f() {
  Serial.print("Batt_Voltage:");
  Serial.print(lc.cellVoltage(), 3);
  Serial.print("\t");
  Serial.print("Batt_Percent:");
  Serial.print(lc.cellPercent(), 1);
  Serial.print("\t");
  Serial.print("Batt_Temp:");
  Serial.println(lc.getCellTemperature(), 1);
}

void max17048() {
  Serial.print(F("Batt Voltage: ")); Serial.print(maxlipo.cellVoltage(), 3); Serial.println(" V");
  Serial.print(F("Batt Percent: ")); Serial.print(maxlipo.cellPercent(), 1); Serial.println(" %");
  Serial.println();
}

void display_mybatt() {  
  float batt_volts;
  char tmpBuf[12];
  int myColor = COLOR_WHITE;

  batt_volts = read_batt();
  if (batt_volts > 3.75) {
    myColor = COLOR_GREEN;
  } else if (batt_volts > 3.5) {
    myColor = COLOR_YELLOW;
  } else if (batt_volts > 3.25) {
    myColor = COLOR_ORANGE;
  } else {
    myColor = COLOR_RED;
  }
  
  dtostrf(batt_volts, 4, 1, tmpBuf);
  screen_writeText_colrow(COL_M_BATT_LAB_N+2, ROW_M_BATT1, 5, tmpBuf, myColor);
  //screen_writeText_colrow(COL_M_BATT_LAB_N+5, ROW_M_BATT1, 1, "v", myColor);
  
  dtostrf(batt_volts, 4, 1, tmpBuf);
  screen_writeText_colrow(COL_M_BATT_LAB_N+2, ROW_M_BATT2, 5, tmpBuf, myColor);
  //screen_writeText_colrow(COL_M_BATT_LAB_N+5, ROW_M_BATT2, 1, "v", myColor);
  screen_show();
}
