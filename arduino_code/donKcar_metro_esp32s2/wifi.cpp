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
 * *******************************************************
 * info about ESP-NOW communication
 * https://dronebotworkshop.com/esp-now/  * 
 * *******************************************************
 * 
 */
#include "wifi.h"
#include "status.h"
#include "nunchuk.h"

#include <WiFi.h>
#include <esp_now.h>



/*
 * *************************************************
 * private data 
 * *************************************************
*/

// MAC Address of current controller (once its connected)
uint8_t controllerMAC[6];
bool controllerMACknown;

// peer info
esp_now_peer_info_t peerInfo;

bool  esp_now_available;

/*
 * *************************************************
 * private function templates
 * *************************************************
*/
void setup_wifi_espnow();
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

/*
 * *************************************************
 * public functions 
 * *************************************************
*/

void wifi_init() {  
  String myMACs;
  char myMACc[20];
  char tmpBuf[24];
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    status_disp_simple_msg("Cannot Init ESPNOW", 'R');
    delay(2000);
    return;
  }
    
  // Register callback function for receiving
  esp_now_register_recv_cb(OnDataRecv);
  
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  controllerMACknown = false;    

  myMACs = WiFi.macAddress(); 
  status_disp_IP_or_MAC(myMACs, 'M');
  
  esp_now_available = true;
}

void wifi_deinit() {
  esp_now_available = false;
}

bool wifi_is_espnow_avail() {
  return esp_now_available;
}

/* 
 *  sends a message to controller, mstType is:
 */
void wifi_send_message( uint8_t *databuffer, int len) {

if (esp_now_available) {
    if (controllerMACknown) {  
      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(controllerMAC, databuffer, len);
       
      if (result == ESP_OK) {
        //DEBUG_PRINTLN("Sending confirmed");
      }
      else {
        //DEBUG_PRINTLN("Sending error");
      }
    }
  }
}


/*
 * *************************************************
 * private functions 
 * *************************************************
*/

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //DEBUG_PRINT("\r\nLast Packet Send Status:\t");
  //DEBUG_PRINTLN(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback function executed when data is received
// note this is processed during ISR (interrupt service) so 
// it should be fast, and should not do any millis() checks
// or anything that would disrupt an ISR

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (!controllerMACknown) {
    controllerMACknown = true;
    for (int i=0; i<6; i++) {
      controllerMAC[i] = mac[i];
    } 
    // Register peer
    memcpy(peerInfo.peer_addr, controllerMAC, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;    
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      //DEBUG_PRINTLN("Failed to add peer");
      return;
    }     
  }   
  nunchuk_process_cmd_from_remote(incomingData, len);
}
