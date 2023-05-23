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
#include "wifi.h"
#include "screen.h"
#include "status.h"
#include "nunchuk.h"

#include <WiFi.h>
//#include <ESPmDNS.h>
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
//char ssid[] = "your_ssid_here";
//char password[] = "your_pw_here";

// MAC Address of responder - this is for (red) racer3
//uint8_t broadcastAddress[] = {0x7c, 0xdf, 0xa1, 0x44, 0x41, 0x76};

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

// Callback function executed when data is received
// note this is processed during ISR (interrupt service) so 
// it should be fast, and should not do any millis() checks
// or anything that would disrupt an ISR
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));  
  process_nunchuk1_from_espnow(myData.msgtype, myData.intdata, myData.floatdata);
}

void setup_wifi_espnow() {   
  String myMACs;
  char myMACc[20];
  char tmpBuf[24];
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    status_set_info_msg("Cannot Init ESPNOW", COLOR_RED);
    delay(2000);
    return;
  }
    
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
    
  screen_writeText_colrow(COL_LEFTEDGE, ROW_MAC, WIDTH_HEADER, "M:", COLOR_HEADER );
  myMACs = WiFi.macAddress(); 
  myMACs.toCharArray(myMACc, 20);
  screen_writeText_colrow(COL_LEFTEDGE+3, ROW_MAC, WIDTH_DATA, myMACc, COLOR_CYAN );
  screen_show();
}

/*
 * *************************************************
 * public functions 
 * *************************************************
*/

void wifi_init() {
  //setup_wifi_as_client();   // initialize wifi 
  setup_wifi_espnow();
}


/*
 * ************************************
 * keep old stuff for reference
 * ************************************
 */

#ifdef DONTUSETHIS

// note this is irrelevant for ESPNOW, but might be used later when HTML menus setup
void wifi_display_signal_strength() {
  long rssi;
  
  rssi = WiFi.RSSI();
  screen_writeText_colrow(COL_LEFTEDGE, ROW_RSSI, WIDTH_HEADER, "RSSI:", COLOR_HEADER);
  String temp = String(rssi);
  screen_writeString_colrow(COL_DATA, ROW_RSSI, 4, temp, COLOR_FOREGROUND ); 
  screen_writeString_colrow(COL_DATA+4, ROW_RSSI, 3, "db", COLOR_FOREGROUND );
  screen_show();
}

/*
 * setup_wifi_as_client initializes device as wifi client
 * i.e.: this is a device which looks for and connects to an existing wifi hotspot
 */
void setup_wifi_as_client() {
  String myIPs, myMACs;
  char myIPc[18], myMACc[20];
  char tmpBuf[24];
  
  delay(10);
  
  //screen_clear();
  screen_writeText_colrow(COL_LEFTEDGE, ROW_SSID, WIDTH_HEADER, "SSID:", COLOR_HEADER);
  screen_writeText_colrow(COL_DATA, ROW_SSID, WIDTH_DATA, ssid, COLOR_FOREGROUND ); 
  screen_writeText_colrow(COL_LEFTEDGE, ROW_IP, WIDTH_HEADER, "Stat:", COLOR_HEADER);
  screen_writeText_colrow(COL_DATA, ROW_IP, WIDTH_DATA, "connecting", COLOR_YELLOW ); 
  screen_show();
  
  WiFi.begin(ssid, password);
  
  //if (MDNS.begin(ROBOT_NAME)){
  //  mDNSavail = true;
  //} else {
  //  DEBUG_PRINTLN("Error starting mDNS");
  //}
  delay(1000); // give this time to settle before we ask for IP and RSSI

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    screen_dot_blink(screen_get_pixel_x(screen_get_last_col_index()-1), screen_get_pixel_y(ROW_IP), COLOR_FOREGROUND );
  }

  screen_writeText_colrow(COL_LEFTEDGE, ROW_IP, WIDTH_HEADER, "IP:", COLOR_HEADER );
  myIPs = WiFi.localIP().toString();
  myIPs.toCharArray(myIPc, 18);
  screen_writeText_colrow(COL_DATA, ROW_IP, WIDTH_DATA, myIPc, COLOR_FOREGROUND ); 
  
  screen_writeText_colrow(COL_LEFTEDGE, ROW_MAC, WIDTH_HEADER, "M:", COLOR_HEADER );
  myMACs = WiFi.macAddress(); 
  myMACs.toCharArray(myMACc, 20);
  screen_writeText_colrow(COL_LEFTEDGE+3, ROW_MAC, WIDTH_DATA, myMACc, COLOR_FOREGROUND );
  
  screen_writeText_colrow(COL_LEFTEDGE, ROW_URL, WIDTH_HEADER, "URL:", COLOR_HEADER );
  if (mDNSavail) {
    //strcpy(tmpBuf,"http://");
    strcpy(tmpBuf, ROBOT_NAME);
    strcat(tmpBuf, ".local");
    screen_writeText_colrow(COL_DATA, ROW_URL, WIDTH_DATA, tmpBuf, COLOR_GREEN );
  } else {
    strcpy(tmpBuf,"(no DNS)");
    screen_writeText_colrow(COL_DATA, ROW_URL, WIDTH_DATA, tmpBuf, COLOR_RED );
  }
  
  wifi_display_signal_strength();  
  screen_show();
}
#endif // DONTUSETHIS
