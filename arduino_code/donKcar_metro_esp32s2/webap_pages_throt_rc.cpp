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
#include "webap_pages_throt_rc.h"
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
 
bool webap_build_throt(String header) {
  char   tmpBuf[36];  // for simple numeric conversions
  
  bool processed_a_page = false;
  
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page steer.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  if (header.indexOf("GET /throt.html") >=0) {
    
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Throttle / Speeds Setup</h1>\n";
      pageBuf = pageBuf + "<table>\n";

      
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='4'>Manual Mode</td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Creep Speed</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManSpdCreep(-5);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valManSpdCreep' value='" + config.manual_speed_creep + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManSpdCreep(+5);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveManSpdCreep();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"goManSpdCreep();\">Go</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Normal Speed</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManSpdNormal(-5);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valManSpdNormal' value='" + config.manual_speed_normal + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManSpdNormal(+5);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveManSpdNormal();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"goManSpdNormal();\">Go</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Turning Speed</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManSpdTurn(-5);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valManSpdTurn' value='" + config.manual_speed_turns + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManSpdTurn(+5);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveManSpdTurn();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"goManSpdTurn();\">Go</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Turn Threshold</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManTurnThresh(-5);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valManTurnThresh' value='" + config.manual_turn_threshold + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjManTurnThresh(+5);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveManTurnThresh();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix green' colspan='4'>Auto Mode</td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Normal Speed</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjAutoSpdNormal(-5);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valAutoSpdNormal' value='" + config.auto_speed_normal + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjAutoSpdNormal(+5);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveAutoSpdNormal();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGreen\" onClick=\"goAutoSpdNormal();\">Go</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Turning Speed</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjAutoSpdTurn(-5);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valAutoSpdTurn' value='" + config.auto_speed_turns + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjAutoSpdTurn(+5);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveAutoSpdTurn();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGreen\" onClick=\"goAutoSpdTurn();\">Go</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix pink' colspan='4'>General</td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Accel Limit</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjAccelLimit(-2);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valAccelLimit' value='" + config.accel_limit + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjAccelLimit(+2);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveAccelLimit();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Startup PWM</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjThrotStartPWM(-5);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valThrotStartPWM' value='" + config.throt_start_pwm + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjThrotStartPWM(+5);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveThrotStartPWM();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGold\" onClick=\"goThrotStartPWM();\">Go</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";

        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Startup Duration</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>\n";
            pageBuf = pageBuf + "<button onClick=\"adjThrotStartDur(-1);\">&lt&lt</button>\n";
            pageBuf = pageBuf + "<input style='width:48px; max-width:48px;' type='text' id='valThrotStartDur' value='" + config.throt_start_duration + "'/>\n";
            pageBuf = pageBuf + "<button onClick=\"adjThrotStartDur(+1);\">&gt&gt</button>\n";
          pageBuf = pageBuf + "</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveThrotStartDur();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Stop</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
          pageBuf = pageBuf + "<td class='matrix' colspan='2'><button class=\"btnsmall btnLtBlue\" onClick=\"stop();\">Stop Motor</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";

    pageBuf = pageBuf + "</table>\n";    
        
    pageBuf = pageBuf + "<p>Throttle values are always specified in a range of 0 (full stop) to 255 (fastest forward).</p>\n";
    pageBuf = pageBuf + "<p>Accel limit specifies the maximum change in throttle that can occur during each drivetrain tick (0.1s).</p>\n";
    pageBuf = pageBuf + "<p>Startup pulse is a short pulse of higher PWM to get wheels started.  This is useful in overcoming inertia.  Duration is specified in drivetrain ticks (0.1s)</p>\n";
    pageBuf = pageBuf + "<p>Turn Threshold is the Turn Value (0-255) above which the Turning Speed is triggered.  Enter a positive value but it works both ways.</p>\n";
        
    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();
      
      pageBuf = pageBuf + "function adjManSpdCreep(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valManSpdCreep').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 128; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 0) { newval = 0; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valManSpdCreep').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function adjManSpdNormal(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valManSpdNormal').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 128; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 0) { newval = 0; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valManSpdNormal').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function adjManSpdTurn(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valManSpdTurn').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 128; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 0) { newval = 0; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valManSpdTurn').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function adjManTurnThresh(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valManTurnThresh').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 128; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 3) { newval = 3; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valManTurnThresh').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function adjAutoSpdNormal(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valAutoSpdNormal').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 128; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 0) { newval = 0; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valAutoSpdNormal').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function adjAutoSpdTurn(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valAutoSpdTurn').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 128; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 0) { newval = 0; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valAutoSpdTurn').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 

      pageBuf = pageBuf + "function adjThrotStartPWM(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valThrotStartPWM').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 128; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 0) { newval = 0; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valThrotStartPWM').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 

      pageBuf = pageBuf + "function adjThrotStartDur(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valThrotStartDur').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 2; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 0) { newval = 0; }\n  if (newval > 20) { newval = 20; }\n";
      pageBuf = pageBuf + "  document.getElementById('valThrotStartDur').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 
      
      pageBuf = pageBuf + "function adjAccelLimit(amt) {\n";      
      pageBuf = pageBuf + "  initialval=parseInt(document.getElementById('valAccelLimit').value);\n";
      pageBuf = pageBuf + "  if (isNaN(initialval)) { initialval = 25; }\n";
      pageBuf = pageBuf + "  newval = initialval + amt;\n  if (newval < 5) { newval = 5; }\n  if (newval > 255) { newval = 255; }\n";
      pageBuf = pageBuf + "  document.getElementById('valAccelLimit').value = newval;\n";
      pageBuf = pageBuf + "}\n"; 
      
         
      pageBuf = pageBuf + "function goManSpdCreep() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valManSpdCreep').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/go?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";   
      
      pageBuf = pageBuf + "function goManSpdNormal() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valManSpdNormal').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/go?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";   
      
      pageBuf = pageBuf + "function goManSpdTurn() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valManSpdTurn').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/go?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
      
      pageBuf = pageBuf + "function goAutoSpdNormal() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valAutoSpdNormal').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/go?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";     
      
      pageBuf = pageBuf + "function goAutoSpdTurn() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valAutoSpdTurn').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/go?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";   

      pageBuf = pageBuf + "function goThrotStartPWM() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valThrotStartPWM').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/go?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";      
      
      pageBuf = pageBuf + "function stop() {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/go?value=0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";


      
      pageBuf = pageBuf + "function saveManSpdCreep() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valManSpdCreep').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveManSpdCreep?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
      pageBuf = pageBuf + "function saveManSpdNormal() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valManSpdNormal').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveManSpdNormal?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
      pageBuf = pageBuf + "function saveManSpdTurn() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valManSpdTurn').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveManSpdTurn?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
      pageBuf = pageBuf + "function saveAutoSpdNormal() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valAutoSpdNormal').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveAutoSpdNormal?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
      pageBuf = pageBuf + "function saveAutoSpdTurn() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valAutoSpdTurn').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveAutoSpdTurn?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
      pageBuf = pageBuf + "function saveThrotStartPWM(md) {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valThrotStartPWM').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveThrotStartPWM?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
      pageBuf = pageBuf + "function saveManTurnThresh() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valManTurnThresh').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveManTurnThresh?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";      
      
      pageBuf = pageBuf + "function saveThrotStartDur() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valThrotStartDur').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveThrotStartDur?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
            
      pageBuf = pageBuf + "function saveAccelLimit() {\n";
      pageBuf = pageBuf + "  myThrot = document.getElementById('valAccelLimit').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'throt/saveAccelLimit?value='+myThrot);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";
      
      // no actions are required here because nothing sends a value that needs to be displayed on web page
      // but the function is required anyhow
      pageBuf = pageBuf + "function local_api_process(paramid, myvalue) {\n";
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
String webap_process_API_throt(String header) {  
  
  /*
   * API functions for Steering config
   */
 
  if (header.indexOf("/wcmd/throt/go") >= 0) {
    drivetrain_enable();
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);   
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR may not be negative";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      servo_set_throttle(myInt);
      if (myInt == 0) {
        drivetrain_disable();
      }
      return "OK";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }    

  } else if (header.indexOf("/wcmd/throt/saveManSpdCreep") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.manual_speed_creep = myInt;
      cfg_save();
      return "SUCCESS Config file updated for MANUAL CREEP SPEED (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }  

  } else if (header.indexOf("/wcmd/throt/saveManSpdNormal") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.manual_speed_normal = myInt;
      cfg_save();
      return "SUCCESS Config file updated for MANUAL NORMAL SPEED (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }

  } else if (header.indexOf("/wcmd/throt/saveManSpdTurn") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.manual_speed_turns = myInt;
      cfg_save();
      return "SUCCESS Config file updated for MANUAL TURN SPEED (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }

  } else if (header.indexOf("/wcmd/throt/saveAutoSpdNormal") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.auto_speed_normal = myInt;
      cfg_save();
      return "SUCCESS Config file updated for AUTO NORMAL SPEED (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/throt/saveAutoSpdTurn") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.auto_speed_turns = myInt;
      cfg_save();
      return "SUCCESS Config file updated for AUTO TURN SPEED (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/throt/saveThrotStartPWM") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.throt_start_pwm = myInt;
      cfg_save();
      return "SUCCESS Config file updated for THROTTLE STARTUP PWM (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }

  } else if (header.indexOf("/wcmd/throt/saveManTurnThresh") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 3) {
        return "ERROR value too small, must be bigger than 3";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.manual_turn_threshold = myInt;
      cfg_save();
      return "SUCCESS Config file updated for MANUAL TURN THRESHOLD (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/throt/saveThrotStartDur") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 20) {
        return "ERROR value too large, must be less than 20";
      }
      config.throt_start_duration = myInt;
      cfg_save();
      return "SUCCESS Config file updated for THROTTLE START DURATION (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/throt/saveAccelLimit") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isNumeric(myValue)) {
      int myInt = myValue.toInt();
      if (myInt < 5) {
        return "ERROR value too small, must be bigger than 5";
      }
      if (myInt > 255) {
        return "ERROR value too large, must be less than 255";
      }
      config.accel_limit = myInt;
      cfg_save();
      return "SUCCESS Config file updated for ACCELERATION LIMIT (" + String(myInt) + ")";   // success
    } else {
      return "ERROR only numbers allowed";   // non-numeric
    }
   
  }  
  return "NOMATCH";
}


    
