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
#include "webap_pages_cam_blobs.h"
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
 
bool webap_build_cam_blobs(String header) {
  char   tmpBuf[36];  // for simple numeric conversions
  
  bool processed_a_page = false;
  
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page cam_blobs.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  if (header.indexOf("GET /cam_blobs.html") >=0) {
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Camera Blob Tracker Setup</h1>\n";
      pageBuf = pageBuf + "<table>\n";
             
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>ROI Placement</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>Y Loc</td>\n";
          pageBuf = pageBuf + "<td class='matrix'>Height</td>\n";
          pageBuf = pageBuf + "<td class='matrix' colspan='2'>&nbsp</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Top</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiTloc' value='" + config.blob_roiTloc + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiTheight' value='" + config.blob_roiTheight + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setROIyh_t();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveROIyh_t();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Middle</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiMloc' value='" + config.blob_roiMloc + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiMheight' value='" + config.blob_roiMheight + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setROIyh_m();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveROIyh_m();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Bottom</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiBloc' value='" + config.blob_roiBloc + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiBheight' value='" + config.blob_roiBheight + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setROIyh_b();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveROIyh_b();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix' colspan='5'>Values are in PIXELS.  0=top, 119=bottom.</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>ROI Weights</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Top</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiTweight' value='" + config.blob_roiTweight + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setROIw_t();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveROIw_t();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Middle</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiMweight' value='" + config.blob_roiMweight + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setROIw_m();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveROIw_m();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Bottom</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_roiBweight' value='" + config.blob_roiBweight + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setROIw_b();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveROIw_b();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix' colspan='5'>Values are floating point (0.0 - 2.0)</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>Flood Fill Parameters</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left' colspan='2'>Floating Threshold</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_float_thresh' value='" + config.blob_float_thresh + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setThreshFloat();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveThreshFloat();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left' colspan='2'>Seed Threshold</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_seed_thresh' value='" + config.blob_seed_thresh + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setThreshSeed();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveThreshSeed();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left' colspan='2'>Seed Location</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_seed_loc' value='" + config.blob_seed_loc + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"setLocSeed();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"saveLocSeed();\">Make Default</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix' colspan='5'>Threshold values are floating point (0.0 - 1.0), Seed Location is in pixels (0=top, 119=bottom)</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
      pageBuf = pageBuf + "</table>\n";        

    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();
         
      pageBuf = pageBuf + "function saveROIyh_t() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiTloc').value;\n";
      pageBuf = pageBuf + "  newVal1 = document.getElementById('inp_roiTheight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIyh_t?value=' + newVal + ':' + newVal1 + ':1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function saveROIyh_m() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiMloc').value;\n";
      pageBuf = pageBuf + "  newVa1l = document.getElementById('inp_roiMheight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIyh_m?value=' + newVal + ':' + newVal1 + ':1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function saveROIyh_b() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiBloc').value;\n";
      pageBuf = pageBuf + "  newVal1 = document.getElementById('inp_roiBheight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIyh_b?value=' + newVal + ':' + newVal1 + ':1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function saveROIw_t() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiTweight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIw_t?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function saveROIw_m() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiMweight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIw_m?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function saveROIw_b() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiBweight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIw_b?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
         
      pageBuf = pageBuf + "function saveThreshFloat() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_float_thresh').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ThreshFloat?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";    
         
      pageBuf = pageBuf + "function saveThreshSeed() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_seed_thresh').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ThreshSeed?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function saveLocSeed() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_seed_loc').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/LocSeed?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";     
      
         
      pageBuf = pageBuf + "function setROIyh_t() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiTloc').value;\n";
      pageBuf = pageBuf + "  newVal1 = document.getElementById('inp_roiTheight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIyh_t?value=' + newVal + ':' + newVal1 + ':0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function setROIyh_m() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiMloc').value;\n";
      pageBuf = pageBuf + "  newVal1 = document.getElementById('inp_roiMheight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIyh_m?value=' + newVal + ':' + newVal1 + ':0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";         
         
      pageBuf = pageBuf + "function setROIyh_b() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiBloc').value;\n";
      pageBuf = pageBuf + "  newVal1 = document.getElementById('inp_roiBheight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIyh_b?value=' + newVal + ':' + newVal1 + ':0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function setROIw_t() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiTweight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIw_t?value=' + newVal + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function setROIw_m() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiMweight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIw_m?value=' + newVal + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
      
      pageBuf = pageBuf + "function setROIw_b() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_roiBweight').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ROIw_b?value=' + newVal + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";
         
      pageBuf = pageBuf + "function setThreshFloat() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_float_thresh').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ThreshFloat?value=' + newVal + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
         
      pageBuf = pageBuf + "function setThreshSeed() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_seed_thresh').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/ThreshSeed?value=' + newVal + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";        
         
      pageBuf = pageBuf + "function setLocSeed() {\n";
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_seed_loc').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camb/LocSeed?value=' + newVal + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";     
      
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();

    processed_a_page = true;
  } // if (header.indexOf("GET /cam_blobs.html") >=0)
  
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
String webap_process_API_cam_blobs(String header) { 
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
    
  if (header.indexOf("/wcmd/camb/ROIyh_t") >= 0) {
    int y_location, box_height, action;

    // param1 holds "y" value (0-119)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param1)) {
      int myInt = param1.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be 0 or greater";
      }
      if (myInt > 119) {
        return "ERROR value too large, must be less than 120";
      }
      y_location = myInt;
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
    // param2 holds "height" value (1-119)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param2)) {
      int myInt = param2.toInt();
      if (myInt < 1) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 119) {
        return "ERROR value too large, must be less than 120";
      }
      box_height = myInt;
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_T, y_location, box_height);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_T, y_location, box_height);
      config.blob_roiTheight = box_height;
      config.blob_roiTloc = y_location;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ROIyh_t

    
  } else if (header.indexOf("/wcmd/camb/ROIyh_m") >= 0) {
    int y_location, box_height, action;

    // param1 holds "y" value (0-119)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param1)) {
      int myInt = param1.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be 0 or greater";
      }
      if (myInt > 119) {
        return "ERROR value too large, must be less than 120";
      }
      y_location = myInt;
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
    // param2 holds "height" value (1-119)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param2)) {
      int myInt = param2.toInt();
      if (myInt < 1) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 119) {
        return "ERROR value too large, must be less than 120";
      }
      box_height = myInt;
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_M, y_location, box_height);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_M, y_location, box_height);
      config.blob_roiMheight = box_height;
      config.blob_roiMloc = y_location;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ROIyh_m

    
  } else if (header.indexOf("/wcmd/camb/ROIyh_b") >= 0) {
    int y_location, box_height, action;

    // param1 holds "y" value (0-119)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param1)) {
      int myInt = param1.toInt();
      if (myInt < 0) {
        return "ERROR value too small, must be 0 or greater";
      }
      if (myInt > 119) {
        return "ERROR value too large, must be less than 120";
      }
      y_location = myInt;
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
    // param2 holds "height" value (1-119)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param2)) {
      int myInt = param2.toInt();
      if (myInt < 1) {
        return "ERROR value too small, must be bigger than 0";
      }
      if (myInt > 119) {
        return "ERROR value too large, must be less than 120";
      }
      box_height = myInt;
    } else {
      return "ERROR only floating point numbers allowed (1.0 - 5.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_B, y_location, box_height);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_B, y_location, box_height);
      config.blob_roiBheight = box_height;
      config.blob_roiBloc = y_location;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ROIyh_b
    


  } else if (header.indexOf("/wcmd/camb/ROIw_t") >= 0) {
    int action;
    float myFloat;

    // param1 holds "weight" value (0.0 = 2.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 2.0)";
      }
      if (myFloat > 2.0) {
        return "ERROR value too large, must be (0.0 - 2.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 2.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_T, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_T, myFloat);
      config.blob_roiTweight = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ROIw_t
    
  } else if (header.indexOf("/wcmd/camb/ROIw_m") >= 0) {
    int action;
    float myFloat;

    // param1 holds "weight" value (0.0 = 2.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 2.0)";
      }
      if (myFloat > 2.0) {
        return "ERROR value too large, must be (0.0 - 2.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 2.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_M, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_M, myFloat);
      config.blob_roiMweight = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ROIw_m
    
  } else if (header.indexOf("/wcmd/camb/ROIw_b") >= 0) {
    int action;
    float myFloat;

    // param1 holds "weight" value (0.0 = 2.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 2.0)";
      }
      if (myFloat > 2.0) {
        return "ERROR value too large, must be (0.0 - 2.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 2.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_B, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_B, myFloat);
      config.blob_roiBweight = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ROIw_b

    
    
  } else if (header.indexOf("/wcmd/camb/ThreshFloat") >= 0) {  
    int action;
    float myFloat;

    // param1 holds "threshold value" value (0.0 = 2.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 2.0)";
      }
      if (myFloat > 2.0) {
        return "ERROR value too large, must be (0.0 - 2.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 2.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_SET_FLOATING_THRESH, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_SET_FLOATING_THRESH, myFloat);
      config.blob_float_thresh = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ThreshFloat
    
    
  } else if (header.indexOf("/wcmd/camb/ThreshSeed") >= 0) {  
    int action;
    float myFloat;

    // param1 holds "threshold value" value (0.0 = 2.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.0) {
        return "ERROR value must be positive (0.0 - 2.0)";
      }
      if (myFloat > 2.0) {
        return "ERROR value too large, must be (0.0 - 2.0";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.0 - 2.0)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_SET_SEED_THRESH, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_SET_SEED_THRESH, myFloat);
      config.blob_seed_thresh = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/ThreshSeed
    

    
  } else if (header.indexOf("/wcmd/camb/LocSeed") >= 0) {  
    int action;
    int seed_loc;

    // param1 holds "threshold value" value (0.0 = 2.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param1)) {
      seed_loc = param1.toInt();
      if (seed_loc < 1) {
        return "ERROR value must be positive (1-119)";
      }
      if (seed_loc > 119) {
        return "ERROR value too large, must be (1-19)";
      }
    } else {
      return "ERROR only integer numbers allowed (1-119)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_BLOB_SET_SEED_LOC, seed_loc);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_BLOB_SET_SEED_LOC, seed_loc);
      config.blob_seed_loc = seed_loc;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camb/LocSeed


  } 
  
  return "NOMATCH";
}   
