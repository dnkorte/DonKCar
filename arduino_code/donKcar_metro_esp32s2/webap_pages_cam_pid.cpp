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
#include "webap_pages_cam_pid.h"
#include "webap_core.h"
#include "util.h"
#include "cam.h"

extern String pageBuf;

/*
 * this checks for pages related to battery configuration
 * 
 * if an appropriate page request is present in (header) then
 * it builds the page in pageBuf and returns true
 * otherwise it returns false and does not modify pageBuf
 */
 
bool webap_build_cam_pid(String header) {
  char   tmpBuf[36];  // for simple numeric conversions
  
  bool processed_a_page = false;
  
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page cam_blobs.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  if (header.indexOf("GET /cam_pid.html") >=0) {
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Camera PID Driving Setup</h1>\n";
      pageBuf = pageBuf + "<table>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>PID Controller Gains</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Prop. Gain Kp (0-1000)</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_kp' value='" + config.pid_kp + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_kp();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Integral Gain Ki (0-300)</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_ki' value='" + config.pid_ki + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_ki();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Deriv. Gain Kd (0-300)</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_kd' value='" + config.pid_kd + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_kd();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";        


#ifdef NOLONGERUSED
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>Steering Setup for PID</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Steering Gain (0.0 - 3.0)</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_steering_gain' value='" + config.pid_steering_gain + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_steering_gain();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Auto Steer Direction</td>\n";
         
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_steer_direction' id='inp_steering_direction'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.pid_steering_direction) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Normal</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.pid_steering_direction) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Reverse</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                  
          
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_steering_direction();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix' colspan='5'>Note Steering Gain converts desired servo angle (in degrees) from PID to servo command value (-255 to +255).  Allowed values 0.5 to 6.0.</td>\n";
        pageBuf = pageBuf + "</tr>\n";
  #endif // (NOLONGERUSED)
        
      pageBuf = pageBuf + "</table>\n";        

    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();
      
      pageBuf = pageBuf + "function save_kp() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_kp').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camp/kp?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function save_ki() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_ki').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camp/ki?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function save_kd() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_kd').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camp/kd?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
         
      pageBuf = pageBuf + "function save_steering_gain() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_steering_gain').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camp/steering_gain?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
         
      pageBuf = pageBuf + "function save_steering_direction() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_steering_direction').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camp/steering_direction?value=' + myIndex + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
      
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();

    processed_a_page = true;
  } // if (header.indexOf("GET /cam_pid.html") >=0)
  
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
 *          
 */
String webap_process_API_cam_pid(String header) { 
  String myValue, AllThreeParams, param1, param2, param3;
  int delimiter_1, delimiter_2;
  int locOfQuestion, locOfTrailingSpace;
    
  locOfQuestion = header.indexOf('?');
  locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
  myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 

  // we assume that the parameter "value" holds 3 number values delimited by ":" characters
  // they represent 3 arguments to this page and start after   ?value=
  AllThreeParams = header.substring(locOfQuestion+7);
  delimiter_1 = AllThreeParams.indexOf(":");
  delimiter_2 = AllThreeParams.indexOf(":", delimiter_1 +1);
  param1 = AllThreeParams.substring(0, delimiter_1);
  param2 = AllThreeParams.substring(delimiter_1 + 1, delimiter_2);
  param3 = AllThreeParams.substring(delimiter_2 + 1);
  param3.substring(0,1);
  
 /* 
  * ********************************************************************
  * API functions for camera blobs tracker setup
  * ********************************************************************
  */
    
  if (header.indexOf("/wcmd/camp/kp") >= 0) {
    int action;
    float myFloat;

    // param1 holds "weight" value (0.0 = 1000.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 1000.0)";
      }
      if (myFloat > 1000.0) {
        return "ERROR value too large, must be (0.0 - 1000.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 1000.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_PID_SET_KP, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_PID_SET_KP, myFloat);
      config.pid_kp = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camp/kp
    
  } else if (header.indexOf("/wcmd/camp/ki") >= 0) {
    int action;
    float myFloat;

    // param1 holds "weight" value (0.0 = 300)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 100.0)";
      }
      if (myFloat > 300.0) {
        return "ERROR value too large, must be (0.0 - 300.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 100300.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_PID_SET_KI, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_PID_SET_KI, myFloat);
      config.pid_ki = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camp/ki
    
  } else if (header.indexOf("/wcmd/camp/kd") >= 0) {
    int action;
    float myFloat;

    // param1 holds "weight" value (0.0 = 300.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 300.0)";
      }
      if (myFloat > 300.0) {
        return "ERROR value too large, must be (0.0 - 300.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 300.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_PID_SET_KD, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_PID_SET_KD, myFloat);
      config.pid_kd = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camp/kd
    
    
  } else if (header.indexOf("/wcmd/camp/steering_gain") >= 0) {
    int action;
    float myFloat;

    // param1 holds "weight" value (0.0 = 6.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.5) {
        return "ERROR value must be positive (0.5 - 6.0)";
      }
      if (myFloat > 6.0) {
        return "ERROR value too large, must be (0.5 - 6.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.5 - 6.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_PID_SET_STEERING_GAIN, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_PID_SET_STEERING_GAIN, myFloat);
      config.pid_steering_gain = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camp/steering_gain

    

    
  } else if (header.indexOf("/wcmd/camp/steering_direction") >= 0) {  
    int action;
    int myIndex;

    // param1 holds selector index; 0 representing NORMAL, 1 representing REVERSE
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param1)) {
      myIndex = param1.toInt();
      if (myIndex < 0) {
        return "ERROR invalid selection";
      }
      if (myIndex > 1) {
        return "ERROR invalid selection";
      }
    } else {
      return "ERROR invalid index";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_PID_SET_STEERING_DIRECTION, myIndex);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_PID_SET_STEERING_DIRECTION, myIndex);
      config.pid_steering_direction = myIndex;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camp/steering_direction


  } 
  
  return "NOMATCH";
}   
