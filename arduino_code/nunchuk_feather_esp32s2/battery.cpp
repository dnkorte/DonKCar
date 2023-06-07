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
#include "status.h"


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

float curEbattvolts, curEcellvolts, curMbattvolts, curMcellvolts, curCbattvolts, currCcellvolts;
char  curEcolorcode, curMcolorcode, curCcolorcode;   // 'R', 'O', 'Y', 'G', '?'
int   curEcolorRGB, curMcolorRGB, curCcolorRGB;
bool  battEdata_is_new, battMdata_is_new;

/*
 * private function templates
 */
float read_my_batt();

/*
 * public functions
 */
 

void batt_init() {
  /*  
   * setup battery monitor chip(s) - note per the reverse TFT guide, the board
   * may come with either of 2 different chips, depending upon supply.
   * we provide for both of them here
   * https://learn.adafruit.com/esp32-s2-reverse-tft-feather/power-management
   */
  if (!maxlipo.begin()) {
    // if no lc709203f..
    if (!lc.begin()) {
      while (1) delay(10);
    }
    // found lc709203f!
    else {
      addr0x36 = false;
      lc.setThermistorB(3950);
      lc.setPackSize(LC709203F_APA_500MAH);
      lc.setAlarmVoltage(3.8);
    }
  // found max17048!
  } else {
    addr0x36 = true;
  }
  battEdata_is_new = false;
  battMdata_is_new = false;
}

void batt_store_batt_volts(char battcode, float battvolts, float cellvolts, char colorcode) {
  if (battcode == 'M') {
    curMbattvolts = battvolts;
    curMcellvolts = cellvolts;
    curMcolorcode = colorcode;
    battMdata_is_new = true;
  } else {
    curEbattvolts = battvolts;
    curEcellvolts = cellvolts;
    curEcolorcode = colorcode;
    battEdata_is_new = true;
  }
}

void display_my_mybatt() {  
  float batt_volts;
  
  batt_volts = read_my_batt();
  if (batt_volts > 3.75) {
    curCcolorcode = 'G';
  } else if (batt_volts > 3.5) {
    curCcolorcode = 'Y';
  } else if (batt_volts > 3.25) {
    curCcolorcode = 'O';
  } else {
    curCcolorcode = 'R';
  }
  status_disp_batt_volts('C', batt_volts, batt_volts, curCcolorcode);
}



void display_remote_mybatts() {  
  char tmpBuf[12];

  if (battEdata_is_new) {
    status_disp_batt_volts('E', curEbattvolts, curEcellvolts, curEcolorcode);    
    battEdata_is_new = false;
  }
  
  if (battMdata_is_new) {
    status_disp_batt_volts('M', curMbattvolts, curMcellvolts, curMcolorcode); 
    battMdata_is_new = false;
  }
}

bool batt_has_new_data_ready(char battcode) {
  if (battcode == 'E') {
    return battEdata_is_new;
  }
  if (battcode == 'M') {
    return battMdata_is_new;
  }
  return false;
}


/*
 * private functions
 */
 
void lc709203f() {
  //Serial.print("Batt_Voltage:");
  //Serial.print(lc.cellVoltage(), 3);
  //Serial.print("\t");
  //Serial.print("Batt_Percent:");
  //Serial.print(lc.cellPercent(), 1);
  //Serial.print("\t");
  //Serial.print("Batt_Temp:");
  //Serial.println(lc.getCellTemperature(), 1);
}

void max17048() {
  //Serial.print(F("Batt Voltage: ")); Serial.print(maxlipo.cellVoltage(), 3); Serial.println(" V");
  //Serial.print(F("Batt Percent: ")); Serial.print(maxlipo.cellPercent(), 1); Serial.println(" %");
  //Serial.println();
}

float read_my_batt() { 
  if (addr0x36 == true) {
    //max17048();
    return maxlipo.cellVoltage();
  }
  else {
    //lc709203f();
    return lc.cellVoltage();
  }
}
