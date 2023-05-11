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
#include "webap_pages_steer_rc.h"
#include "webap_core.h"
#include "util.h"
#include "drivetrain.h"
#include "servo.h"

extern String pageBuf;

/*
 * this checks for pages related to steering
 * (for RC-steered vehicles)
 * 
 * if an appropriate page request is present in (header) then
 * it builds the page in pageBuf and returns true
 * otherwise it returns false and does not modify pageBuf
 */
 
bool webap_build_steer(String header) {
  char   tmpBuf[36];  // for simple numeric conversions
  
  bool processed_a_page = false;
  
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page steer.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  if (header.indexOf("GET /steer.html") >=0) {
    
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Steering Setup</h1>\n";
      pageBuf = pageBuf + "<table>\n";
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td colspan='2' class='matrix'><button class=\"button btnRed\" onClick=\"sendSteerGoto(\'L\');\">Goto LEFT</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"button btnGold\" onClick=\"adjSteerConfig(\'L\');\">Save uS L</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";      
          pageBuf = pageBuf + "<td colspan='2' class='matrix'><button class=\"button btnBlue\" onClick=\"sendSteerGoto(\'C\')\">Goto CENTER</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"button btnGold\" onClick=\"adjSteerConfig(\'C\');\">Save uS C</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";      
          pageBuf = pageBuf + "<td colspan='2' class='matrix'><button class=\"button btnGreen\" onClick=\"sendSteerGoto(\'R\')\">Goto RIGHT</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"button btnGold\" onClick=\"adjSteerConfig(\'R\');\">Save uS R</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
  
        pageBuf = pageBuf + "<tr>\n";    
            pageBuf = pageBuf + "<td colspan='2' class='matrix'><button class=\"button btnLtBlue\" onClick=\"sendSteerUS()\">Goto uS</button></td>\n";
            pageBuf = pageBuf + "<td class='matrix'><b>uS</b><br /><button onClick=\"adjUS(-5);\">&lt&lt</button><input style='width:48px; max-width:48px;' type='text' id='steerUS' /><button onClick=\"adjUS(+5);\">&gt&gt</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
    pageBuf = pageBuf + "</table>\n";

    pageBuf = pageBuf + "<p>Click Red, Blue, or Green buttons to move wheels to specified position (L, C, R). ";
    pageBuf = pageBuf + "Or type a value (" + SERVO_MIN_MICROS + "-" + SERVO_MAX_MICROS + ") into the text box ";
    pageBuf = pageBuf + "and click the Light Blue button to move wheels to position matching entered number of ";
    pageBuf = pageBuf + "microSeconds.</p>\n";
    
    pageBuf = pageBuf + "<p>Click a gold button to adjust configuration file value for appropriate (L, C, R) ";
    pageBuf = pageBuf + "position to match the value entered in text box.</p>\n";
    
    pageBuf = pageBuf + "<p>Currently configured (saved) uS values are: L (<span id='usL'>" + config.steer_left_us + "</span>), C (<span id='usC'>" + config.steer_center_us + "</span>), R (<span id='usR'>" + config.steer_right_us + "</span>).</p>\n";
      
    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();
          
      pageBuf = pageBuf + "function sendSteerGoto(cmd) {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'steer/goto'+cmd);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";   
         
      pageBuf = pageBuf + "function sendSteerUS() {\n";
      pageBuf = pageBuf + "  myUS = document.getElementById('steerUS').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'steer/gotoUS?value='+myUS);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function adjUS(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('steerUS').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 1500; }\n";
      pageBuf = pageBuf + "  document.getElementById('steerUS').value = initialval + amt;\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function local_api_process(paramid, myvalue) {\n";
      pageBuf = pageBuf + "  if (paramid == 'USLEFT') { document.getElementById('usL').textContent = myvalue; document.getElementById('steerUS').value = myvalue; }\n";
      pageBuf = pageBuf + "  if (paramid == 'USCENTER') { document.getElementById('usC').textContent = myvalue; document.getElementById('steerUS').value = myvalue; }\n";
      pageBuf = pageBuf + "  if (paramid == 'USRIGHT') { document.getElementById('usR').textContent = myvalue; document.getElementById('steerUS').value = myvalue; }\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function adjSteerConfig(cmd) {\n";
      pageBuf = pageBuf + "  myUS = document.getElementById('steerUS').value;\n";
      pageBuf = pageBuf + "  if (cmd == 'L') { document.getElementById('usL').textContent = myUS; }\n";
      pageBuf = pageBuf + "  if (cmd == 'C') { document.getElementById('usC').textContent = myUS; }\n";
      pageBuf = pageBuf + "  if (cmd == 'R') { document.getElementById('usR').textContent = myUS; }\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'steer/adjcfg'+cmd+'?value='+myUS);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();

    processed_a_page = true;
  } 
  return processed_a_page;
}
  



    
/*
 * process commands that have been sent by a web button 
 * note this should not display anything to website
 * it returns a status code to caller though, which MIGHT display an alert or might use a returned value
 * 
 * returns  "OK":  if success; note calling page just continues with no acknowledgement
 *          "SUCCESS message":   calling page does alert showing this string
 *          "ERROR message":     calling page does alert showing this string
 *          "VALUE paramid value" this returns a value that calling page will process (no alert)
 *                                (note the value can be a string but if so no spaces allowed)
 *                                
 *          "NOMATCH"            didn't find any matching URLs                      
 */
String webap_process_API_steer(String header) {  
  
  /*
   * API functions for Steering config
   */
 
  if (header.indexOf("/wcmd/steer/gotoL") >= 0) {
    drivetrain_enable();
    servo_set_steering_value(-255);
    return "VALUE USLEFT " + String(config.steer_left_us);
    
  } else if (header.indexOf("/wcmd/steer/gotoC") >= 0) {
    drivetrain_enable();
    servo_set_steering_value(0); 
    return "VALUE USCENTER " + String(config.steer_center_us);
    
  } else if (header.indexOf("/wcmd/steer/gotoR") >= 0) {
    drivetrain_enable();
    servo_set_steering_value(255);
    return "VALUE USRIGHT " + String(config.steer_right_us);
    
  } else if (header.indexOf("/wcmd/steer/gotoUS") >= 0) {
    drivetrain_enable();
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);   
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < SERVO_MIN_MICROS) {
        return "ERROR value too small, must be bigger than " + String(SERVO_MIN_MICROS);
      }
      if (myInt > SERVO_MAX_MICROS) {
        return "ERROR value too large, must be less than " + String(SERVO_MAX_MICROS);
      }
      servo_set_steering_uS(myInt);
      return "OK";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }

  } else if (header.indexOf("/wcmd/steer/adjcfgL") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < SERVO_MIN_MICROS) {
        return "ERROR value too small, must be bigger than " + String(SERVO_MIN_MICROS);
      }
      if (myInt > SERVO_MAX_MICROS) {
        return "ERROR value too large, must be less than " + String(SERVO_MAX_MICROS);
      }
      config.steer_left_us = myInt;
      cfg_save();
      return "SUCCESS Config file updated for LEFT (" + String(myInt) + ") uS";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/steer/adjcfgC") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < SERVO_MIN_MICROS) {
        return "ERROR value too small, must be bigger than " + String(SERVO_MIN_MICROS);
      }
      if (myInt > SERVO_MAX_MICROS) {
        return "ERROR value too large, must be less than " + String(SERVO_MAX_MICROS);
      }
      config.steer_center_us = myInt;
      cfg_save();
      return "SUCCESS Config file updated for CENTER (" + String(myInt) + ") uS";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/steer/adjcfgR") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < SERVO_MIN_MICROS) {
        return "ERROR value too small, must be bigger than " + String(SERVO_MIN_MICROS);
      }
      if (myInt > SERVO_MAX_MICROS) {
        return "ERROR value too large, must be less than " + String(SERVO_MAX_MICROS);
      }
      config.steer_right_us = myInt;
      cfg_save();
      return "SUCCESS Config file updated for RIGHT (" + String(myInt) + ") uS";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
  }
  return "NOMATCH";
}


    
