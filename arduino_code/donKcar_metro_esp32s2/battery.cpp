/*
 * Summary: openMV + esp32 based autonomous racer
 * 
 * Author(s):  Don Korte
 * Repository: https://github.com/dnkorte/DonKCar
 *
 * MIT License
 * Copyright (c) 2020 Don Korte
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. * 
 * 
 * **********************************************************************************************************************
 * read battery voltage.  note that battery is divided by resistive 
 * divider with 10k and 1k, such that nominal 7.4v (LiPo 2S) should
 * yield approx 0.67v at pin A2 (BATT_E) or A3 (BATT_M)
 * https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32s2/api-reference/peripherals/adc.html
 * https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32s2/api-reference/peripherals/adc.html#_CPPv414adc1_channel_t
 * attenuation ADC_ATTEN_DB_0 allows to read 0 - 750 mV
 * Pin A2 (GPIO1) is on ADC1 which is the one that is ok to use even if using WiFi
 * Pin A3 (GPIO2) is on ADC1 which is the one that is ok to use even if using WiFi
 * **********************************************************************************************************************
 *
 */
#include "battery.h"
#include "nunchuk.h"
#include "screen.h"
#include "status.h"
#include "util.h"     // needed for a/d average

float batt_volts_E, cell_volts_E;   // batt for electronics
float batt_volts_M, cell_volts_M;   // batt for motors
char  colorcode_E, colorcode_M;     // 'R', 'O', 'Y', 'G', '?'
int   num_red_E, num_red_M;         // if 3 checks in a row are red then we go to MODE_ERROR_BATT 

  int  battColor;

void batt_init() {  
  batt_volts_E = 0.0;
  batt_volts_M = 0.0;
  cell_volts_E = 0.0;
  cell_volts_M = 0.0;
  num_red_E = 0;
  num_red_M = 0;
  colorcode_E = 'Z';
  colorcode_E = 'Z';
  
  analogReadResolution(12);
}

/*
 * note that batt_read returns boolean value indicating
 * whether robot shutdown is required due to critical
 * battery voltage level.   
 * 
 * each "reading" does 5 checks in quick succession to average
 * the current voltage (noise reduction), then returns the average
 * value.     
 * 
 * if 3 successive calls to this function indicate RED status
 * then the shutdown indication is returned 
 * 
 * returns true if shutdown required, false if ok
 * 
 * note that the value is read and stored by this function but
 * is not "returned" to the caller, and is not displayed
 */
bool batt_read(char battcode) {
  int  vbat_raw;

  if ((battcode == 'E') && config.batt_E_used) {
    vbat_raw = readAnalog5xAveraged(PIN_VBAT_E_CHK);
    batt_volts_E = (config.batt_E_scale * vbat_raw) / 1000.0;  
    cell_volts_E = batt_volts_E / config.batt_E_numcells;
    
    if (cell_volts_E > config.batt_E_min_green) {
      colorcode_E = 'G';
    } else if (cell_volts_E > config.batt_E_min_yellow) {
      colorcode_E = 'Y';
    } else if (cell_volts_E > config.batt_E_min_orange) {
      colorcode_E = 'O';
    } else {
      colorcode_E = 'R';
    }   
          
    if ((colorcode_E == 'R') && config.batt_E_shutdown_on_red) {
      num_red_E++;
      if (num_red_E > 3) {
        return true;    // indicate time to perform shutdown due to critically low batt
      }
    } else {
      num_red_E = 0;
    }
  }  

  if ((battcode == 'M') && config.batt_M_used) {
    vbat_raw = readAnalog5xAveraged(PIN_VBAT_M_CHK);
    batt_volts_M = (config.batt_M_scale * vbat_raw) / 1000.0;  
    cell_volts_M = batt_volts_M / config.batt_M_numcells;
    
    if (cell_volts_M > config.batt_M_min_green) {
      colorcode_M = 'G';
    } else if (cell_volts_M > config.batt_M_min_yellow) {
      colorcode_M = 'Y';
    } else if (cell_volts_M > config.batt_M_min_orange) {
      colorcode_M = 'O';
    } else {
      colorcode_M = 'R';
    }   
          
    if ((colorcode_M == 'R') && config.batt_M_shutdown_on_red) {
      num_red_M++;
      if (num_red_M > 3) {
        return true;    // indicate time to perform shutdown due to critically low batt
      }
    } else {
      num_red_M = 0;
    }
  }
  
  return false;     // no battery shuddown needed
}

void batt_display(char battcode) {  
  if (battcode == 'E') {  
    status_disp_batt_volts('E', batt_volts_E, cell_volts_E, colorcode_E);
  }  
  if (battcode == 'M') {  
    status_disp_batt_volts('M', batt_volts_M, cell_volts_M, colorcode_M);
  }
  screen_show();  
}

/* 
 *  this function sends current battery voltage and colorcode information
 *  to the remote controller device using ESPNOW
 */
void batt_send(char battcode) {
  if (battcode == 'E') { 
    nunchuk_send_batt('E', colorcode_E, batt_volts_E, cell_volts_E);  
  }
  
  if (battcode == 'M') { 
    nunchuk_send_batt('M', colorcode_M, batt_volts_M, cell_volts_M);  
  } 
}
