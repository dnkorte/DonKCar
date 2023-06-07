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
#include <WiFi.h>
#include "wifi.h"
#include "webap_core.h"
#include "webap_pages_steer_rc.h"
#include "webap_pages_throt_rc.h"
#include "webap_pages_batt.h"
#include "webap_pages_util.h"
#include "mode_mgr.h"
#include "status.h"


/*
 * note this code is based upon examples at
 * https://randomnerdtutorials.com/esp32-web-server-arduino-ide/ 
 * https://randomnerdtutorials.com/esp32-access-point-ap-web-server/     +++
 * Note when ESP32 board is selected, the Arduino IDE selects the ESP32 version of Wifi.h automatically
 * https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
 * 
 * https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/wifi.html 
 * 
 * https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/ 
 * https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library/ 
 * https://randomnerdtutorials.com/esp32-websocket-server-arduino/
 * https://randomnerdtutorials.com/esp32-web-server-slider-pwm/
 * https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/
 */

// Set web server port number to 80

WiFiServer serverAP(80);
WiFiClient clientAP;

String pageBuf;   // global variable that page builders put new pages in

/*
 * *********************************************
 * private data
 * *********************************************
 */

// Variable to store the HTTP request
String headerX;
bool   webModeActive, webModeEndRequest;

// Current time
unsigned long currentTime= millis();
// Previous time
unsigned long previousTime= 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime= 2000;

/*
 * templates for private functions
 */

String webap_process_API(String header);
void webap_build_web_page(String header);

/*
 * *********************************************
 * public functions
 * *********************************************
 */

//char* ssid = "racer";
char* password = "123456789";

void webap_init(void) {  
  char tmpbuf[9];
   
  // Remove the password parameter, if you want the AP (Access Point) to be open 
  WiFi.mode(WIFI_AP);
  delay(1000);
  WiFi.softAP(config.robot_name, password);
  IPAddress IP = WiFi.softAPIP();
  String IPs = String() + IP[0] + "." + IP[1] + "." + IP[2] + "." + IP[3];
  status_disp_IP_or_MAC(IPs, 'I');
  status_disp_info_msgs("Connect to ^^^ IP", "on SSID "+String(config.robot_name), "", 'O');

  wifi_deinit();
  webModeActive = true;
  webModeEndRequest = false;
  serverAP.begin();           // start http server
}

void webap_deinit(int reason) {
  webModeActive = false;
  webModeEndRequest = false;
  wifi_init();    // this re-initializes ESP-NOW mode
  delay(1000);
  void status_clear_status_area();
  if (reason == 0) {
    status_disp_info_msgs("", "Web Config Ended", "by user request", 'C');
  }
  if (reason == 1) {
    status_disp_info_msgs("", "Web Config Ended", "due to lost signal", 'O');
  }
  mode_set_mode(MODE_IDLE);
}

bool webap_getWebMode() {
  return webModeActive;
}

bool webap_getWebEndRequest() {
  return webModeEndRequest;
}

void webap_adjustWebEndRequest(bool newValue) {
  webModeEndRequest = newValue;
}

/*
 * function webap_process() checks for incoming clients
 * then if found, connects, views requests, and calls
 * helper functions to either build a page or perform
 * API services
 */
void webap_process(void) {
  String actionResponseStatus;
  String myPageBuf;
  
  clientAP = serverAP.available();   // Listen for incoming clients
  headerX = "";

  if (clientAP) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                // make a String to hold incoming data from the 
    actionResponseStatus = "OK";
    while (clientAP.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (clientAP.available()) {             // if there's bytes to read from the client,
        char c = clientAP.read();             // read a byte, then
        headerX += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so we need to send a response
          // but must evaluate the page first (if its a display page) to see if any user errors
          
          if (currentLine.length() == 0) {     
                   
            if (headerX.indexOf("/wcmd/") >= 0) {
              actionResponseStatus = webap_process_API(headerX);                  
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              clientAP.println("HTTP/1.1 200 " +  actionResponseStatus + " ");   
              clientAP.println("Content-type:text/html");
              clientAP.println("Connection: close");
              clientAP.println();
              // The HTTP response ends with another blank line
              clientAP.println();
              
            } else {  
              // if this is a request to display a new web page, do it       
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              clientAP.println("HTTP/1.1 200 OK");
              clientAP.println("Content-type:text/html");
              clientAP.println("Connection: close");
              clientAP.println();
              // The HTTP response ends with another blank line
              clientAP.println();
              webap_build_web_page(headerX);
              clientAP.println(pageBuf);
              actionResponseStatus = true;                              
            } 
            
            // Break out of the while loop          
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    headerX = "";
    // Close the connection
    clientAP.stop();
  }
}

/*
 * public functions -- helpers for page build
 * (for use by webap_XXX page builders)
 */

String webap_start_local_js(void) {
  String obuf = "";
  obuf = obuf + "<script type='text/javascript'>\n";
  obuf = obuf + "const Http = new XMLHttpRequest();\n";
  obuf = obuf + "const urlBase='http://192.168.4.1./wcmd/';\n";  
  return obuf;
}

String webap_end_local_js(void) {
  return "</script>\n";
}

String webap_commonJS(void) {
  String obuf = "";
  obuf = obuf + "<script type='text/javascript'>\n";
      obuf = obuf + "const urlheart=urlBase+'heartbeat';\n";
      obuf = obuf + "setInterval(function() {Http.open('GET', urlheart); Http.send();}, 5000);\n";

      obuf = obuf + "Http.onreadystatechange = function() {\n";
      obuf = obuf + "  if (this.readyState == 4 && this.status == 200) {\n";
      obuf = obuf + "    parts = this.statusText.split(' ');\n";

      obuf = obuf + "    if (parts[0] == 'OK') { return; }\n";
      obuf = obuf + "    if ((parts[0] == 'ERROR') || (parts[0] == 'SUCCESS')) { alert( this.statusText); return; }\n";
      obuf = obuf + "    if (parts[0] == 'VALUE') { local_api_process(parts[1], parts[2]); return; }\n";
      obuf = obuf + "    alert('ERROR Something wierd happened; page returned unexpected value');\n";
      obuf = obuf + "  }\n";
      obuf = obuf + "};\n";
 
  obuf = obuf + "</script>";  
  return obuf;
}

String webap_start_page() {  
  String obuf = "";
    obuf = obuf + "<!DOCTYPE html>\n<html>\n";
    obuf = obuf + "<head>\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    obuf = obuf + "<link rel=\"icon\" href=\"data:,\">\n";   // this disables the request for page icon
  return obuf;
}

String webap_end_page() {  
  return "</body>\n</html>\n";
}


/*
 * functions that build page sections that could be common
 * to all pages
 */

String show_menu(void) {
  String pageBuf;

  pageBuf = "";  
  pageBuf = pageBuf + "<table>\n";
  pageBuf = pageBuf + "<tr>\n";
    pageBuf = pageBuf + "<td class='menu tan'><a href=\"/getcfg.html\">Show Config JSON</a></td>\n";
    pageBuf = pageBuf + "<td class='menu green'><a href=\"/steer.html\">Steering Config</a></td>\n";
    pageBuf = pageBuf + "<td class='menu ltblue'><a href=\"/steer_sweeps.html\">Steering Sweeps</a></td>\n";
  pageBuf = pageBuf + "</tr>\n";

  
  
  pageBuf = pageBuf + "<tr>";
    pageBuf = pageBuf + "<td class='menu green'><a href=\"/battE.html\">Batt E Config</a></td>\n";
    pageBuf = pageBuf + "<td class='menu green'><a href=\"/battM.html\">Batt M Config</a></td>\n";
    pageBuf = pageBuf + "<td class='menu green'><a href=\"/identity.html\">Identity</a></td>\n";
  pageBuf = pageBuf + "</tr>\n";
  
  pageBuf = pageBuf + "<tr>";
    pageBuf = pageBuf + "<td class='menu green'><a href=\"/throt.html\">Throttle Config</a></td>\n";
    pageBuf = pageBuf + "<td class='menu green'><a href=\"/esc.html\">ESC Config</a></td>\n";
    pageBuf = pageBuf + "<td class='menu green'>&nbsp;</td>\n";
  pageBuf = pageBuf + "</tr>\n";
    
  pageBuf = pageBuf + "<tr>\n";
    pageBuf = pageBuf + "<td class='menu pink' colspan='2'><a href=\"/bye.html\">EXIT CONFIGURATOR</a></td>\n";
    pageBuf = pageBuf + "<td class='menu gold'><a href=\"/index.html\">HOME</a></td>\n";
  pageBuf = pageBuf + "</tr>\n";
  pageBuf = pageBuf + "</table>\n";

  return pageBuf;
}

/*
 * this stylesheet modelled upon 
 * https://www.w3schools.com/howto/howto_css_animate_buttons.asp
 * https://www.w3schools.com/howto/tryit.asp?filename=tryhow_css_buttons_animate3
 * https://htmlcolorcodes.com/color-chart/
 */
String show_mycss(void) {  
  String pageBuf;

  pageBuf = "";  
  pageBuf = pageBuf + "<style>\n";
  pageBuf = pageBuf + "html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: left;}\n";
  pageBuf = pageBuf + ".button {\n";
  //pageBuf = pageBuf + "  padding: 15px 25px;\n";
  pageBuf = pageBuf + "  padding: 12px 12px;\n";
  pageBuf = pageBuf + "  font-size: 24px;\n";
  pageBuf = pageBuf + "  text-align: center;\n";
  pageBuf = pageBuf + "  cursor: pointer;\n";
  pageBuf = pageBuf + "  outline: none;\n";
  pageBuf = pageBuf + "  color: #fff;\n";
  pageBuf = pageBuf + "  border: none;\n";
  pageBuf = pageBuf + "  border-radius: 15px;\n";
  pageBuf = pageBuf + "  box-shadow: 0 9px #999;\n";
  pageBuf = pageBuf + "}\n";
    
  pageBuf = pageBuf + ".button:active {";
  pageBuf = pageBuf + "  box-shadow: 0 5px #666;";
  pageBuf = pageBuf + "  transform: translateY(4px);";
  pageBuf = pageBuf + "}\n";  

  
  pageBuf = pageBuf + ".btnsmall {\n";
  pageBuf = pageBuf + "  padding: 4px 8px;\n";
  pageBuf = pageBuf + "  font-size: 16px;\n";
  pageBuf = pageBuf + "  text-align: center;\n";
  pageBuf = pageBuf + "  cursor: pointer;\n";
  pageBuf = pageBuf + "  outline: none;\n";
  pageBuf = pageBuf + "  color: #fff;\n";
  pageBuf = pageBuf + "  border: none;\n";
  pageBuf = pageBuf + "  border-radius: 6px;\n";
  pageBuf = pageBuf + "  box-shadow: 0 4px #999;\n";
  pageBuf = pageBuf + "}\n";
    
  pageBuf = pageBuf + ".btnsmall:active {";
  pageBuf = pageBuf + "  box-shadow: 0 5px #666;";
  pageBuf = pageBuf + "  transform: translateY(4px);";
  pageBuf = pageBuf + "}\n";  

  pageBuf = pageBuf + ".btnGreen { background-color:#006633;}\n";
  pageBuf = pageBuf + ".btnRed { background-color:#CC0033;}\n";
  pageBuf = pageBuf + ".btnBlue { background-color:#3300cc;}\n";
  pageBuf = pageBuf + ".btnGold { background-color:#ffd700; color:#000; }\n";
  pageBuf = pageBuf + ".btnLtBlue { background-color:#87cefa; color:#000; }\n";
  pageBuf = pageBuf + ".btnLtGreen { background-color:#7fffd4; color:#000; }\n";
  pageBuf = pageBuf + ".btnLtViolet { background-color:#dda0dd; color:#000; }\n";
  
  pageBuf = pageBuf + "table { margin-top: 12px;}\n";
  pageBuf = pageBuf + "td.menu { border: 1px solid; padding: 6px; text-align:center; }\n";
  pageBuf = pageBuf + "td.matrix { border: 0px solid; padding: 6px; text-align:center;}\n";
  pageBuf = pageBuf + "td.green { background-color:#7fffd4; border: 1px solid; }\n";
  pageBuf = pageBuf + "td.pink { background-color:#ffb6c1; border: 1px solid; }\n";
  pageBuf = pageBuf + "td.gold { background-color:#ffd700; border: 1px solid; }\n";

  pageBuf = pageBuf + "td.ltblue { background-color:#87cefa; border: 1px solid; }\n";
  pageBuf = pageBuf + "td.seagreen { background-color:#2e8b57; border: 1px solid; }\n";
  pageBuf = pageBuf + "td.tan { background-color:#d2b48c; border: 1px solid; }\n";
  
  pageBuf = pageBuf + "a { display:block; text-decoration:none; }\n";
  pageBuf = pageBuf + "p { width:480px; max-width:480px; }\n";

  pageBuf = pageBuf + "</style>\n";
  return pageBuf;
}

/*
 * ***********************************************************
 * private functions
 * ***********************************************************
 */
 
/*
 * note this should not display anything to website
 * it returns a status code to caller though, which MIGHT display an alert or might use a returned value
 * 
 * returns  "OK":  if success; note calling page just continues with no acknowledgement
 *          "SUCCESS message":   calling page does alert showing this string
 *          "ERROR message":     calling page does alert showing this string
 *          "VALUE paramid value" this returns a value that calling page will process (no alert)
 *                                (note the value can be a string but if so no spaces allowed)
 */
String webap_process_API(String header) {  
  String api_response;
  
  api_response = webap_process_API_steer(header);
  if (api_response != "NOMATCH") {
    return api_response;
  } 
   
  api_response = webap_process_API_batt(header);
  if (api_response != "NOMATCH") {
    return api_response;
  } 
   
  api_response = webap_process_API_throt(header);
  if (api_response != "NOMATCH") {
    return api_response;
  } 

  /*
   * API function for processing Web Browser Heartbeat signal
   */   
  if (header.indexOf("/wcmd/heartbeat") >= 0) {
    mode_notice_webap_heartbeat();    
  }
  return "OK";
}

void webap_build_web_page(String header) {
  char   tmpBuf[36];  // for simple numeric conversions
  pageBuf = "";

  /*
   * check for pages by category
   */
   if (webap_build_steer(header)) {
    return;
   }
   if (webap_build_batt(header)) {
    return;
   }
   if (webap_build_throt(header)) {
    return;
   }
  
  /* 
   * if none of the above match, then check for basic utility pages
   * note that this function ALWAYS displays a page, even if it is
   * only the default index.html
   */  
    webap_build_util(header);
}
