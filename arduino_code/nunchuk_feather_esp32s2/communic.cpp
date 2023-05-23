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
#include "screen_tft.h"

#include <WiFi.h>
#include <Arduino_JSON.h>
#include <esp_now.h>

/*
 * info about ESP-NOW communication
 * https://dronebotworkshop.com/esp-now/ 
 */

/*
 * 
 * To access the esp32 by name instead of ip address
 *
 *   #include <ESPmDNS.h>
 *   then after WiFi.begin() add:
 *   
 *   if(!MDNS.begin("esp32")){
 *    Serial.println("Error starting mDNS");
 *   return;
 *   }
 *   
 *   Then your browser will see http://racer2.local (or whatever the ROBOT_NAME is)
 *   This note from user "Epi" on oct 24 2022 in replies to
 *   https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/
 */

/*
 * *************************************************
 * private data 
 * *************************************************
*/

// Replace the next variables with your SSID/Password combination
//char ssid[] = "";
//char password[] = "";


// MAC Address of responder - this is for (red) racer3
uint8_t broadcastAddress[] = {0x7c, 0xdf, 0xa1, 0x44, 0x41, 0x76};

typedef struct struct_message {
  char msgtype;   // 'C', 'Z', 'B', 'X', 'Y', 'V'
  int  intdata;
  float floatdata;
} Message;

// create a structured object
Message myData;

// peer info
esp_now_peer_info_t peerInfo;

/*
 * *************************************************
 * private functions 
 * *************************************************
*/

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  DEBUG_PRINT("\r\nLast Packet Send Status:\t");
  DEBUG_PRINTLN(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup_wifi_espnow() {   // Set ESP32 as a Wi-Fi Station
  String myMACs;
  char myMACc[20];
  char tmpBuf[24];
  
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    DEBUG_PRINTLN("Error initializing ESP-NOW");
    return;
  }
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    DEBUG_PRINTLN("Failed to add peer");
    return;
  }  
 
  //screen_writeText_colrow(COL_LEFTEDGE, ROW_MAC, WIDTH_HEADER, "MC:", COLOR_FOREGROUND );
  //myMACs = WiFi.macAddress(); 
  //myMACs.toCharArray(myMACc, 20);
  //screen_writeText_colrow(COL_LEFTEDGE+4, ROW_MAC, WIDTH_DATA, myMACc, COLOR_FOREGROUND );
}



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
void communic_send_message(char msgType, int intData, float floatData) {
  
  myData.msgtype = msgType;
  myData.intdata = intData;
  myData.floatdata = floatData;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    DEBUG_PRINTLN("Sending confirmed");
  }
  else {
    DEBUG_PRINTLN("Sending error");
  }
}
