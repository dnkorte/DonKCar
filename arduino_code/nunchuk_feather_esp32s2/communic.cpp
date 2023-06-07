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

#include "communic.h"
#include "status.h"
#include "battery.h"

#include <WiFi.h>
#include <Arduino_JSON.h>
#include <esp_now.h>

#define MSG_VOLTS_E   0
#define MSG_VOLTS_M   1
#define MSG_STAT1     2
#define MSG_STAT2     3
#define MSG_STAT3     4
#define MSG_MENUITEM  5
#define MSG_STAT_CLR  6

/*
 * info about ESP-NOW communication
 * https://dronebotworkshop.com/esp-now/ 
 */


/*
 * *************************************************
 * private data 
 * *************************************************
*/

// available target device names
String deviceNames[] = {
  "racer3", "notused", "notused"
};

// MAC Address of responder - this is for (red) racer3
uint8_t deviceAddresses[][6] = { 
  {0x7c, 0xdf, 0xa1, 0x44, 0x41, 0x76},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} 
};

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
MessageRtoC_batt myRcvdBatt;
MessageRtoC_text myRcvdText;
MessageRtoC_simple myRcvdSimple;
MessageCtoR mySendData;

// peer info
esp_now_peer_info_t peerInfo;

int selectedDeviceIndex;

/*
 * *************************************************
 * private function templates
 * *************************************************
 */

void setup_wifi_espnow();
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

/*
 * *************************************************
 * public functions 
 * *************************************************
*/

void communic_init() {
  setup_wifi_espnow();   // initialize wifi 
}

/* 
 *  sends a message to racer, mstType is:
 *  'C' (button C), 'Z' (button Z), 'B' (button B), 'X' (joy X), 'Y' (joy Y), 'V' (batt volts)', "h" (heartbeat)
 */
void communic_send_message(char msgType, int intData) {
  
  mySendData.msgtype = msgType;
  mySendData.intdata = intData;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(deviceAddresses[selectedDeviceIndex], (uint8_t *) &mySendData, sizeof(mySendData));
   
  if (result == ESP_OK) {
    // Sending confirmed
  }
  else {
    // Sending error
  }
}

String communic_connected_device() {
  return deviceNames[selectedDeviceIndex];
}

/*
 * *************************************************
 * private functions 
 * *************************************************
*/

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  
}

// Callback function executed when data is received
// note this is processed during ISR (interrupt service) so 
// it should be fast, and should not do any millis() checks
// or anything that would disrupt an ISR

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  switch(incomingData[0]) {    
    case MSG_VOLTS_E:
      memcpy(&myRcvdBatt, incomingData, len);
      batt_store_batt_volts('E', myRcvdBatt.battvolts, myRcvdBatt.cellvolts, myRcvdBatt.colorcode);
      break;
    case MSG_VOLTS_M:
      memcpy(&myRcvdBatt, incomingData, len);
      batt_store_batt_volts('M', myRcvdBatt.battvolts, myRcvdBatt.cellvolts, myRcvdBatt.colorcode);
      memcpy(&myRcvdBatt, incomingData, len);
      break;
    case MSG_STAT1:  
      memcpy(&myRcvdText, incomingData, len);
      status_save_msg(1, myRcvdText.text, myRcvdText.colorcode);
      break;
    case MSG_STAT2:  
      memcpy(&myRcvdText, incomingData, len);
      status_save_msg(2, myRcvdText.text, myRcvdText.colorcode);
      break;
    case MSG_STAT3:  
      memcpy(&myRcvdText, incomingData, len);
      status_save_msg(3, myRcvdText.text, myRcvdText.colorcode);
      break;
    case MSG_MENUITEM:  
      memcpy(&myRcvdText, incomingData, len);
      status_save_menu_msg(myRcvdText.text, myRcvdText.colorcode);
      break;
    default:
      memcpy(&myRcvdSimple, incomingData, len);
  }
   
  
}

void setup_wifi_espnow() {   // Set ESP32 as a Wi-Fi Station
  String myMACs;
  char myMACc[20];
  char tmpBuf[24];
  
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    //DEBUG_PRINTLN("Error initializing ESP-NOW");
    return;
  }

  selectedDeviceIndex = 0;
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register callback function for receiving
  esp_now_register_recv_cb(OnDataRecv);
  
  // Register peer
  memcpy(peerInfo.peer_addr, deviceAddresses[selectedDeviceIndex], 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  } 
}
