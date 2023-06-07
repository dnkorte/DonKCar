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
#include "wifi.h"
#include "mode_mgr.h"

#define MSG_VOLTS_E   0
#define MSG_VOLTS_M   1
#define MSG_STAT1     2
#define MSG_STAT2     3
#define MSG_STAT3     4
#define MSG_MENUITEM  5
#define MSG_STAT_CLR  6

int cmdC, cmdZ, cmdX, cmdY;
bool flagC, flagZ, flagX, flagY;

/*
 * colorcode char for messages is as follows:
 *  G - Green
 *  Y - Yellow
 *  O - Orange
 *  R - Red
 *  W - White
 *  X - Black (background)
 *  B - Blue
 *  C - Cyan
 *  P - Purple
 */
 
typedef struct {
  char msgtype;   // 'C', 'Z', 'X', 'Y', 'V', 'x', 'y', '0', '1', '2'
  int  intdata;
} MessageCtoR;    // this type (structure) holds a message that goes from Controller to Robot

typedef struct {
  uint8_t messagetype;
  char    colorcode;
  float   battvolts;
  float   cellvolts;
} MessageRtoC_batt;    // this type (structure) holds a message that goes from Robot to Controller

typedef struct {
  uint8_t messagetype;
  char    colorcode;
  char    text[22];
} MessageRtoC_text;    // this type (structure) holds a message that goes from Robot to Controller

typedef struct {
  uint8_t messagetype;
  char    colorcode;
} MessageRtoC_simple;    // this type (structure) holds a message that goes from Robot to Controller

// create a structured object
MessageRtoC_batt mySendBatt;
MessageRtoC_text mySendText;
MessageRtoC_simple mySendSimple;
MessageCtoR myRcvdData;

void nunchuk_init() {
  flagC = false;
  flagZ = false;
  flagX = false;
  flagY = false;
}

bool nunchuk_is_available() {
  return wifi_is_espnow_avail();
}

void nunchuk_process_cmd_from_remote(const uint8_t *incomingData, int len) {
  /*
   * handle requests generated in nunchuk module
   * these cannot be completed here because this is called from 
   * the wifi callback for ESPNOW. the callback is part of the
   * nterrupt handler and some events (priincipally those
   * that generate PWM) get confused when called from an ISR
   */  
  
  memcpy(&myRcvdData, incomingData, len);  
      
  if (myRcvdData.msgtype == 'Y') { 
    cmdY = myRcvdData.intdata;
    flagY = true;
  }
     
  if (myRcvdData.msgtype == 'X') { 
    cmdX = myRcvdData.intdata;
    flagX = true;
  }

  if (myRcvdData.msgtype == 'Z') {
    cmdZ = myRcvdData.intdata;
    flagZ = true;
  }
  
  if (myRcvdData.msgtype == 'C') {
    cmdC = myRcvdData.intdata;
    flagC = true;
  }  

  /*
   * this one can be directly handled here because it 
   * just sets a variable
   */
  if (myRcvdData.msgtype == 'H') {
    mode_notice_heartbeat();
  }
}

void nunchuk_send_batt(char battcode, char colorcode, float battvolts, float cellvolts) { 
  if (battcode == 'M') {
    mySendBatt.messagetype = MSG_VOLTS_M;
  } else {
    mySendBatt.messagetype = MSG_VOLTS_E;
  }
  mySendBatt.colorcode = colorcode;
  mySendBatt.battvolts = battvolts;
  mySendBatt.cellvolts = cellvolts;

  // Send message via ESP-NOW
  wifi_send_message( (uint8_t *) &mySendBatt, sizeof(mySendBatt));
}

/*
 * rownumber may be 1,2,3 for status lines, or 0 for menuline
 * message must be a null terminated array of characters
 */
void nunchuk_send_text(int rownumber, char colorcode, char * message) { 
  switch (rownumber) {
    case 1:
      mySendText.messagetype = MSG_STAT1;
      break;
    case 2:
      mySendText.messagetype = MSG_STAT2;
      break;
    case 3:
      mySendText.messagetype = MSG_STAT3;
      break;
    case 0:
      mySendText.messagetype = MSG_MENUITEM;
      break;
    default:
      return;
  }    
  mySendText.colorcode = colorcode;  
  strcpy(mySendText.text, message);

  // Send message via ESP-NOW
  wifi_send_message( (uint8_t *) &mySendText, sizeof(mySendText));
}
