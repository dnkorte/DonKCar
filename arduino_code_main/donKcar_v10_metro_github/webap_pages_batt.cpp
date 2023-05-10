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
#include "webap_pages_batt.h"
#include "webap_core.h"
#include "util.h"

extern String pageBuf;

/*
 * this checks for pages related to battery configuration
 * 
 * if an appropriate page request is present in (header) then
 * it builds the page in pageBuf and returns true
 * otherwise it returns false and does not modify pageBuf
 */
 
bool webap_build_batt(String header) {
  char   tmpBuf[36];  // for simple numeric conversions
  
  bool processed_a_page = false;
  
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page battE.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  if (header.indexOf("GET /battE.html") >=0) {
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Battery E (Electronics)</h1>\n";
      pageBuf = pageBuf + "<table>\n";
      
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Battery Used</td>\n";          
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_beu' id='sel_beu'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.batt_E_used) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">No</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_E_used) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Yes</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                  
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGreen\" onClick=\"saveBattUsed();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
       
      
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Shutdown on Red?</td>\n";            
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_bes' id='sel_bes'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.batt_E_shutdown_on_red) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">No</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_E_shutdown_on_red) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Yes</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                            
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGreen\" onClick=\"saveShutdownOnRed();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
      
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Num of Cells</td>\n";            
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_benc' id='sel_benc'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (config.batt_E_numcells == 1) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + "> 1 cell </option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_E_numcells == 2) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + "> 2 cells </option>\n";       
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_E_numcells == 3) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + "> 3 cells </option>\n";            
          pageBuf = pageBuf + "</select>\n</td>\n";                            
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"saveNumCells();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='3'>Note Min V values are per CELL</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Min V GREEN</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_vgreen' value='" + config.batt_E_min_green + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveMinGreen();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Min V YELLOW</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_vyellow' value='" + config.batt_E_min_yellow + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveMinYellow();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Min V ORANGE</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_vorange' value='" + config.batt_E_min_orange + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveMinOrange();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='3'>Calibration (Batt Voltage, not Cell)</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";          
          int vbat_raw = analogRead(PIN_VBAT_E_CHK);
          float batt_volts = (config.batt_E_scale * vbat_raw) / 1000.0; 
          //dtostrf(batt_volts, 5, 2, tmpBuf);          
          pageBuf = pageBuf + "<td class='matrix'>Measured Volts</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><span id='disp_vmeas'>" + String(batt_volts, 2) + "</span></td>\n";
          //pageBuf = pageBuf + "<td class='matrix'>(per current calib)</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnLtViolet\" onClick=\"refreshAD();\">ReCheck A/D</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Voltmeter Says</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_actvolt' /></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGold\" onClick=\"recalibVolts();\">Re-Calibrate</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
      pageBuf = pageBuf + "</table>\n";        

    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();  
      
      pageBuf = pageBuf + "function local_api_process(paramid, myvalue) {\n";
      pageBuf = pageBuf + "  if (paramid == 'BATTVOLTS') { document.getElementById('disp_vmeas').textContent = myvalue; }\n";
      pageBuf = pageBuf + "}\n"; 
         
      pageBuf = pageBuf + "function saveMinGreen() {\n";
      pageBuf = pageBuf + "  newVolts = document.getElementById('inp_vgreen').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/setMinGreen?value='+newVolts);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function saveMinYellow() {\n";
      pageBuf = pageBuf + "  newVolts = document.getElementById('inp_vyellow').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/setMinYellow?value='+newVolts);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function saveMinOrange() {\n";
      pageBuf = pageBuf + "  newVolts = document.getElementById('inp_vorange').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/setMinOrange?value='+newVolts);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";

      
      pageBuf = pageBuf + "function saveBattUsed() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('sel_beu').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/setBattUsed?value='+myIndex);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function saveShutdownOnRed() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('sel_bes').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/setShutdownRed?value='+myIndex);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function saveNumCells() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('sel_benc').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/setNumCellsIndex?value='+myIndex);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
            
         
      pageBuf = pageBuf + "function recalibVolts() {\n";
      pageBuf = pageBuf + "  voltmeter_says = document.getElementById('inp_actvolt').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/recalibrate?value='+voltmeter_says);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
  
      pageBuf = pageBuf + "function refreshAD() {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battE/recheck');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";   
      
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();

    processed_a_page = true;

     
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page battM.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */

  } else if (header.indexOf("GET /battM.html") >=0) {
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Battery M (Motors/Servos)</h1>\n";
      pageBuf = pageBuf + "<table>\n";
      
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Battery Used</td>\n";          
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_bmu' id='sel_bmu'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.batt_M_used) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">No</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_M_used) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Yes</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                  
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGreen\" onClick=\"saveBattUsed();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
       
      
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Shutdown on Red?</td>\n";            
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_bms' id='sel_bms'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.batt_M_shutdown_on_red) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">No</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_M_shutdown_on_red) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Yes</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                            
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGreen\" onClick=\"saveShutdownOnRed();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
      
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Num of Cells</td>\n";            
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_bmnc' id='sel_bmnc'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (config.batt_M_numcells == 1) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + "> 1 cell </option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_M_numcells == 2) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + "> 2 cells </option>\n";       
            pageBuf = pageBuf + "<option value=1"; 
            if (config.batt_M_numcells == 3) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + "> 3 cells </option>\n";            
          pageBuf = pageBuf + "</select>\n</td>\n";                            
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"saveNumCells();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='3'>Note Min V values are per CELL</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Min V GREEN</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_vgreen' value='" + config.batt_M_min_green + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveMinGreen();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Min V YELLOW</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_vyellow' value='" + config.batt_M_min_yellow + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveMinYellow();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Min V ORANGE</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_vorange' value='" + config.batt_M_min_orange + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveMinOrange();\">Update</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='3'>Calibration (Batt Voltage, not Cell)</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";          
          int vbat_raw = analogRead(PIN_VBAT_M_CHK);
          float batt_volts = (config.batt_M_scale * vbat_raw) / 1000.0; 
          pageBuf = pageBuf + "<td class='matrix'>Measured Volts</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><span id='disp_vmeas'>" + String(batt_volts, 2) + "</span></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnLtViolet\" onClick=\"refreshAD();\">ReCheck A/D</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>Voltmeter Says</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_actvolt' /></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGold\" onClick=\"recalibVolts();\">Re-Calibrate</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
      pageBuf = pageBuf + "</table>\n";        

    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();  
      
      pageBuf = pageBuf + "function local_api_process(paramid, myvalue) {\n";
      pageBuf = pageBuf + "  if (paramid == 'BATTVOLTS') { document.getElementById('disp_vmeas').textContent = myvalue; }\n";
      pageBuf = pageBuf + "}\n"; 
         
      pageBuf = pageBuf + "function saveMinGreen() {\n";
      pageBuf = pageBuf + "  newVolts = document.getElementById('inp_vgreen').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/setMinGreen?value='+newVolts);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function saveMinYellow() {\n";
      pageBuf = pageBuf + "  newVolts = document.getElementById('inp_vyellow').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/setMinYellow?value='+newVolts);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function saveMinOrange() {\n";
      pageBuf = pageBuf + "  newVolts = document.getElementById('inp_vorange').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/setMinOrange?value='+newVolts);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";

      
      pageBuf = pageBuf + "function saveBattUsed() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('sel_bmu').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/setBattUsed?value='+myIndex);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function saveShutdownOnRed() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('sel_bms').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/setShutdownRed?value='+myIndex);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function saveNumCells() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('sel_bmnc').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/setNumCellsIndex?value='+myIndex);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
            
         
      pageBuf = pageBuf + "function recalibVolts() {\n";
      pageBuf = pageBuf + "  voltmeter_says = document.getElementById('inp_actvolt').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/recalibrate?value='+voltmeter_says);\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
  
      pageBuf = pageBuf + "function refreshAD() {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'battM/recheck');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "}\n";   
      
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();

    processed_a_page = true;
  } // if (header.indexOf("GET /battM.html") >=0)
  
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
String webap_process_API_batt(String header) {  
  
 /* 
  * ********************************************************************
  * API functions for Battery E config
  * ********************************************************************
  */

  if (header.indexOf("/wcmd/battE/setMinGreen") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 5.0) {
        return "ERROR value too large, must be less than 5.0";
      }
      config.batt_E_min_green = myFloat;
      cfg_save();
      return "SUCCESS Config file updated for MINIMUM GREEN VOLTS (" + String(myFloat) + ")";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/battE/setMinYellow") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 5.0) {
        return "ERROR value too large, must be less than 5.0";
      }
      config.batt_E_min_yellow = myFloat;
      cfg_save();
      return "SUCCESS Config file updated for MINIMUM YELLOW VOLTS (" + String(myFloat) + ")";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/battE/setMinOrange") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 5.0) {
        return "ERROR value too large, must be less than 5.0";
      }
      config.batt_E_min_orange = myFloat;
      cfg_save();
      return "SUCCESS Config file updated for MINIMUM ORANGE VOLTS (" + String(myFloat) + ")";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/battE/setBattUsed") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
    if (myValue == "0") {
      // index 0 indicates "No"
      config.batt_E_used = false;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt E NOT USED";
    } else if (myValue == "1") {
      // index 1 indicates "Yes"
      config.batt_E_used = true;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt E IS USED";
    } else {
      return "ERROR unexpected value sent";
    }
    
  } else if (header.indexOf("/wcmd/battE/setShutdownRed") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
    if (myValue == "0") {
      // index 0 indicates "No"
      config.batt_E_shutdown_on_red = false;
      cfg_save();
      return "SUCCESS Config file updated, Batt E does NOT shutdown on RED";
    } else if (myValue == "1") {
      // index 1 indicates "Yes"
      config.batt_E_shutdown_on_red = true;
      cfg_save();
      return "SUCCESS Config file updated, Batt E will shutdown on RED";
    } else {
      return "ERROR unexpected value sent";
    }
    
  } else if (header.indexOf("/wcmd/battE/setNumCellsIndex") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
    if (myValue == "0") {
      // index 0 indicates "1 cell"
      config.batt_E_numcells = 1;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt E 1-Cell";
    } else if (myValue == "1") {
      // index 1 indicates "2 cells"
      config.batt_E_numcells = 2;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt E 2-Cell";
    } else if (myValue == "2") {
      // index 2 indicates "3-cells"
      config.batt_E_numcells = 3;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt E 3-Cell";
    } else {
      return "ERROR unexpected value sent";
    }
    
  } else if (header.indexOf("/wcmd/battE/recalibrate") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    int vbat_raw;
    float newScale;
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 18.0) {
        return "ERROR value too large, must be less than 18.0";
      }
      //vbat_raw = analogRead(PIN_VBAT_E_CHK);
      vbat_raw = readAnalog5xAveraged(PIN_VBAT_E_CHK);
      newScale = (1000.0 * myFloat) / float (vbat_raw); 
      config.batt_E_scale = newScale; 
      cfg_save();
      return "SUCCESS Config file updated with new calibration coefficient";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 18.0)";   // non-numeric
    }
 
  } else if (header.indexOf("/wcmd/battE/recheck") >= 0) {
    int vbat_raw = analogRead(PIN_VBAT_E_CHK);
    float batt_volts_E = (config.batt_E_scale * vbat_raw) / 1000.0;     
    return "VALUE BATTVOLTS " + String(batt_volts_E);
 
 /* 
  * ********************************************************************
  * API functions for Battery M config
  * ********************************************************************
  */
    
  } else if (header.indexOf("/wcmd/battM/setMinGreen") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 5.0) {
        return "ERROR value too large, must be less than 5.0";
      }
      config.batt_M_min_green = myFloat;
      cfg_save();
      return "SUCCESS Config file updated for MINIMUM GREEN VOLTS (" + String(myFloat) + ")";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/battM/setMinYellow") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 5.0) {
        return "ERROR value too large, must be less than 5.0";
      }
      config.batt_M_min_yellow = myFloat;
      cfg_save();
      return "SUCCESS Config file updated for MINIMUM YELLOW VOLTS (" + String(myFloat) + ")";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/battM/setMinOrange") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 5.0) {
        return "ERROR value too large, must be less than 5.0";
      }
      config.batt_M_min_orange = myFloat;
      cfg_save();
      return "SUCCESS Config file updated for MINIMUM ORANGE VOLTS (" + String(myFloat) + ")";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
  } else if (header.indexOf("/wcmd/battM/setBattUsed") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
    if (myValue == "0") {
      // index 0 indicates "No"
      config.batt_M_used = false;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt M NOT USED";
    } else if (myValue == "1") {
      // index 1 indicates "Yes"
      config.batt_M_used = true;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt M IS USED";
    } else {
      return "ERROR unexpected value sent";
    }
    
  } else if (header.indexOf("/wcmd/battM/setShutdownRed") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
    if (myValue == "0") {
      // index 0 indicates "No"
      config.batt_M_shutdown_on_red = false;
      cfg_save();
      return "SUCCESS Config file updated, Batt M does NOT shutdown on RED";
    } else if (myValue == "1") {
      // index 1 indicates "Yes"
      config.batt_M_shutdown_on_red = true;
      cfg_save();
      return "SUCCESS Config file updated, Batt M will shutdown on RED";
    } else {
      return "ERROR unexpected value sent";
    }
    
  } else if (header.indexOf("/wcmd/battM/setNumCellsIndex") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
    if (myValue == "0") {
      // index 0 indicates "1 cell"
      config.batt_M_numcells = 1;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt M 1-Cell";
    } else if (myValue == "1") {
      // index 1 indicates "2 cells"
      config.batt_M_numcells = 2;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt M 2-Cell";
    } else if (myValue == "2") {
      // index 2 indicates "3-cells"
      config.batt_M_numcells = 3;
      cfg_save();
      return "SUCCESS Config file updated, reflecting Batt M 3-Cell";
    } else {
      return "ERROR unexpected value sent";
    }
    
  } else if (header.indexOf("/wcmd/battM/recalibrate") >= 0) {
    int locOfQuestion = header.indexOf('?');
    int locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    int vbat_raw;
    float newScale;
    String myValue = header.substring(locOfQuestion+7, locOfTrailingSpace);  
    if (isFloat(myValue)) {
      float myFloat = myValue.toFloat();
      if (myFloat < 1.0) {
        return "ERROR value too small, must be bigger than 1.0";
      }
      if (myFloat > 18.0) {
        return "ERROR value too large, must be less than 18.0";
      }
      //vbat_raw = analogRead(PIN_VBAT_M_CHK);
      vbat_raw = readAnalog5xAveraged(PIN_VBAT_M_CHK);
      newScale = (1000.0 * myFloat) / float (vbat_raw); 
      config.batt_M_scale = newScale; 
      cfg_save();
      return "SUCCESS Config file updated with new calibration coefficient";   // success
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 18.0)";   // non-numeric
    }
 
  } else if (header.indexOf("/wcmd/battM/recheck") >= 0) {
    int vbat_raw = analogRead(PIN_VBAT_M_CHK);
    float batt_volts = (config.batt_M_scale * vbat_raw) / 1000.0;     
    return "VALUE BATTVOLTS " + String(batt_volts); 
  } 
  
  return "NOMATCH";
}   
