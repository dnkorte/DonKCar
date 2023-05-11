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
 */
#include "nunchuk.h"
#include "mode_mgr.h"

int cmdC, cmdZ, cmdX, cmdY;
bool flagC, flagZ, flagX, flagY;

void nunchuk_init() {
  flagC = false;
  flagZ = false;
  flagX = false;
  flagY = false;
}

void process_nunchuk1_from_espnow(char msgType, int intData, float floatData) {
  
  /*
   * handle requests generated in nunchuk module
   * these cannot be completed here because this is called from 
   * the wifi callback for ESPNOW. the callback is part of the
   * nterrupt handler and some events (priincipally those
   * that generate PWM) get confused when called from an ISR
   */      
  if (msgType == 'Y') { 
    cmdY = intData;
    flagY = true;
  }
     
  if (msgType == 'X') { 
    cmdX = intData;
    flagX = true;
  }

  if (msgType == 'Z') {
    cmdZ = intData;
    flagZ = true;
  }
  
  if (msgType == 'C') {
    cmdC = intData;
    flagC = true;
  }  

  /*
   * this one can be directly handled here because it 
   * just sets a variable
   */
  if (msgType == 'H') {
    mode_notice_heartbeat();
  }
}

void process_nunchuk1_from_web(String header) {
  int startOfCmd = header.indexOf("GET /nunchuk1");
  String cmd = header.substring(startOfCmd + 4, (startOfCmd+36));
  int startOfParamName = cmd.indexOf("?") + 1;
  int startOfValue = cmd.indexOf("=") + 1;
  int endOfValue = cmd.indexOf(" ");
  String ParamName = cmd.substring(startOfParamName, startOfValue-1);
  String myValueS = cmd.substring(startOfValue, endOfValue);
  int myValue = myValueS.toInt();
      
  if (ParamName == "joyY") { 
    mode_joyY_event(myValue);
  }
  
  if (ParamName == "joyX") { 
    mode_joyX_event(myValue);
  }

  if (ParamName == "zbutn") {
    mode_z_button_event(myValue);
  }
  
  if (ParamName == "cbutn") {
    mode_c_button_event(myValue);
  }  

  if (ParamName == "imalive") {
    mode_notice_heartbeat();
  }
}
