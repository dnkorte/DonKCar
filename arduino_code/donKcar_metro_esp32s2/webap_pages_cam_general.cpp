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
#include "webap_pages_cam_general.h"
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
 
bool webap_build_cam_general(String header) {
  char   tmpBuf[36];  // for simple numeric conversions
  
  bool processed_a_page = false;
  
  /*
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * page cam_general.html
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   * PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
   */
  if (header.indexOf("GET /cam_general.html") >=0) {
    pageBuf = pageBuf + webap_start_page();
    pageBuf = pageBuf + show_mycss(); 
    pageBuf = pageBuf + "</head>\n";    
    
    pageBuf = pageBuf + "<body>\n<h1>Camera General Setup</h1>\n";
      pageBuf = pageBuf + "<table>\n";
             
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>Camera Mode</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGold\" onClick=\"set_blobs();\">Blobs</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnGreen\" onClick=\"set_regression();\">1 Line</button></td>\n";
          //pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"set_lane_lines();\">Lane Lines</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"set_grayscale();\"> Grayscale</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
      pageBuf = pageBuf + "</table>\n";
     
      pageBuf = pageBuf + "<table>\n";        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>Camera Preferred Mode</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Mode</td>\n";
         
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_startup_mode' id='inp_startup_mode'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.cam_startup_mode == 0) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">BLOB Tracking</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.cam_startup_mode == 1) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">1-Line (Regression)</option>\n";    
            
            //pageBuf = pageBuf + "<option value=2"; 
            //if (config.cam_startup_mode == 2) {
            //  pageBuf = pageBuf + " selected";
            //}
            //pageBuf = pageBuf + ">Lane Lines Tracking</option>\n";    
                   
          pageBuf = pageBuf + "</select>\n</td>\n";                  

          pageBuf = pageBuf + "<td class='matrix'>&nbsp;</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_startup_mode();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";


        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>Image Filtering</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Want HistEq?</td>\n";
         
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_histeq_wanted' id='inp_histeq_wanted'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.cam_histeq_wanted == 0) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">No</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.cam_histeq_wanted == 1) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Yes</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                  
           
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"set_histeq_wanted();\">Set</button></td>\n"; 
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_histeq_wanted();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Want Negate?</td>\n";
         
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_negate_wanted' id='inp_negate_wanted'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.cam_negate_wanted == 0) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">No</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.cam_negate_wanted == 1) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Yes</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                  
          
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"set_negate_wanted();\">Set</button></td>\n"; 
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_negate_wanted();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";



        
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix ltblue' colspan='5'>Lens Perspective Correction</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Corr. Wanted?</td>\n";
         
          pageBuf = pageBuf + "<td class='matrix'>\n<select name='sel_steer_direction' id='inp_perspective_wanted'>\n"; 
            pageBuf = pageBuf + "<option value=0"; 
            if (!config.cam_perspective_wanted == 0) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">No</option>\n"; 
            
            pageBuf = pageBuf + "<option value=1"; 
            if (config.cam_perspective_wanted == 1) {
              pageBuf = pageBuf + " selected";
            }
            pageBuf = pageBuf + ">Yes</option>\n";           
          pageBuf = pageBuf + "</select>\n</td>\n";                  
          
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"set_perspective_wanted();\">Set</button></td>\n"; 
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_perspective_wanted();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";

        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix left'>Corr. Factor</td>\n";
          pageBuf = pageBuf + "<td class='matrix'><input style='width:48px; max-width:48px;' type='text' id='inp_perspective_factor' value='" + config.cam_perspective_factor + "'/></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnBlue\" onClick=\"set_perspective_factor();\">Set</button></td>\n";
          pageBuf = pageBuf + "<td class='matrix'><button class=\"btnsmall btnRed\" onClick=\"save_perspective_factor();\">Save</button></td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
        pageBuf = pageBuf + "<tr>\n";
          pageBuf = pageBuf + "<td class='matrix' colspan='4'>This can reduce the effects of perspective, but it distorts the image and at higher levels tends to 'lower' items in the image. Best to leave it OFF. If used range is 0.05 - 0.35.</td>\n";
        pageBuf = pageBuf + "</tr>\n";
        
      pageBuf = pageBuf + "</table>\n";        

    pageBuf = pageBuf + show_menu();  
    pageBuf = pageBuf + webap_start_local_js();
         
      pageBuf = pageBuf + "function set_blobs() {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/set_blobs');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
         
      pageBuf = pageBuf + "function set_regression() {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/set_regression');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
         
      pageBuf = pageBuf + "function set_lane_lines() {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/set_lane_lines');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
         
      pageBuf = pageBuf + "function set_grayscale() {\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/set_grayscale');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
      
      pageBuf = pageBuf + "function save_startup_mode() {\n";
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_startup_mode').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/startup_mode?value=' + myIndex + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
         
      pageBuf = pageBuf + "function set_perspective_factor() {\n"; 
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_perspective_factor').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/perspective_factor?value=' + newVal + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
         
      pageBuf = pageBuf + "function set_perspective_wanted() {\n";   
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_perspective_wanted').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/perspective_wanted?value=' + myIndex + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
         
      pageBuf = pageBuf + "function set_histeq_wanted() {\n";   
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_histeq_wanted').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/histeq_wanted?value=' + myIndex + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
         
      pageBuf = pageBuf + "function set_negate_wanted() {\n";   
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_negate_wanted').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/negate_wanted?value=' + myIndex + ':XX:0');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";   
         
      pageBuf = pageBuf + "function save_perspective_factor() {\n"; 
      pageBuf = pageBuf + "  newVal = document.getElementById('inp_perspective_factor').value;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/perspective_factor?value=' + newVal + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n"; 
         
      pageBuf = pageBuf + "function save_perspective_wanted() {\n";   
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_perspective_wanted').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/perspective_wanted?value=' + myIndex + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
         
      pageBuf = pageBuf + "function save_histeq_wanted() {\n";   
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_histeq_wanted').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/histeq_wanted?value=' + myIndex + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
         
      pageBuf = pageBuf + "function save_negate_wanted() {\n";   
      pageBuf = pageBuf + "  myIndex = document.getElementById('inp_negate_wanted').selectedIndex;\n";
      pageBuf = pageBuf + "  Http.open('GET', urlBase+'camg/negate_wanted?value=' + myIndex + ':XX:1');\n";
      pageBuf = pageBuf + "  Http.send();\n";
      pageBuf = pageBuf + "  }\n";  
      
    pageBuf = pageBuf + webap_end_local_js();
    pageBuf = pageBuf + webap_commonJS();
    pageBuf = pageBuf + webap_end_page();

    processed_a_page = true;
  } // if (header.indexOf("GET /cam_general.html") >=0)
  
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
String webap_process_API_cam_general(String header) { 
 
 /* 
  * ********************************************************************
  * API functions for camera general setup
  * ********************************************************************
  */
  DEBUG_PRINTLN(header);  
  if (header.indexOf("/wcmd/camg/set_blobs") >= 0) {
      cam_send_cmd(CAM_CMD_MODE_BLOBS);
      return "SUCCESS set camera to BLOB TRACKING MODE";
      
  } else if (header.indexOf("/wcmd/camg/set_regression") >= 0) {
      cam_send_cmd(CAM_CMD_MODE_REGRESSION_LINE);
      return "SUCCESS set camera to 1-Line (Regression) TRACKING MODE";
      
  } else if (header.indexOf("/wcmd/camg/set_lane_lines") >= 0) {
      cam_send_cmd(CAM_CMD_MODE_LANE_LINES);
      return "SUCCESS set camera to LANE LINES TRACKING MODE";
      
  } else if (header.indexOf("/wcmd/camg/set_grayscale") >= 0) {
      cam_send_cmd(CAM_CMD_MODE_GRAYSCALE);
      return "SUCCESS set camera to GRAYSCALE MODE";

} else if (header.indexOf("/wcmd/camg/startup_mode") >= 0) {  
    int action;
    int myIndex;
    
    String AllThreeParams, param1, param2, param3;
    int delimiter_1, delimiter_2;
    int locOfQuestion, locOfTrailingSpace;
      
    locOfQuestion = header.indexOf('?');
    locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    //myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
  
    // we assume that the parameter "value" holds 3 number values delimited by ":" characters
    // they represent 3 arguments to this page and start after   ?value=
    AllThreeParams = header.substring(locOfQuestion+7);
    delimiter_1 = AllThreeParams.indexOf(":");
    delimiter_2 = AllThreeParams.indexOf(":", delimiter_1 +1);
    param1 = AllThreeParams.substring(0, delimiter_1);
    param2 = AllThreeParams.substring(delimiter_1 + 1, delimiter_2);
    param3 = AllThreeParams.substring(delimiter_2 + 1);
    param3.substring(0,1);

    // param1 holds selector index; 0/BLOBS, 1/1-Line (regression), 2/Lane Lines
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isInteger(param1)) {
      myIndex = param1.toInt();
      if (myIndex < 0) {
        return "ERROR invalid selection";
      }
      if (myIndex > 2) {
        return "ERROR invalid selection";
      }
    } else {
      return "ERROR invalid index";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      config.cam_startup_mode = myIndex;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camp/startup_mode
    
} else if (header.indexOf("/wcmd/camg/perspective_wanted") >= 0) {
    int action;
    int myIndex;
    
    String AllThreeParams, param1, param2, param3;
    int delimiter_1, delimiter_2;
    int locOfQuestion, locOfTrailingSpace;
      
    locOfQuestion = header.indexOf('?');
    locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    //myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
  
    // we assume that the parameter "value" holds 3 number values delimited by ":" characters
    // they represent 3 arguments to this page and start after   ?value=
    AllThreeParams = header.substring(locOfQuestion+7);
    delimiter_1 = AllThreeParams.indexOf(":");
    delimiter_2 = AllThreeParams.indexOf(":", delimiter_1 +1);
    param1 = AllThreeParams.substring(0, delimiter_1);
    param2 = AllThreeParams.substring(delimiter_1 + 1, delimiter_2);
    param3 = AllThreeParams.substring(delimiter_2 + 1);
    param3.substring(0,1);

    // param1 holds selector index; 0 representing NO, 1 representing YES
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
      if (myIndex == 0) {
        cam_send_cmd(CAM_CMD_CAM_PERSPECTIVE_OFF);
      } else {
        cam_send_cmd(CAM_CMD_CAM_PERSPECTIVE_ON);
      }
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      if (myIndex == 0) {
        cam_send_cmd(CAM_CMD_CAM_PERSPECTIVE_OFF);
      } else {
        cam_send_cmd(CAM_CMD_CAM_PERSPECTIVE_ON);
      }
      config.cam_perspective_wanted = myIndex;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camg/perspective_wanted

    
    
} else if (header.indexOf("/wcmd/camg/histeq_wanted") >= 0) {
    int action;
    int myIndex;
    
    String AllThreeParams, param1, param2, param3;
    int delimiter_1, delimiter_2;
    int locOfQuestion, locOfTrailingSpace;
      
    locOfQuestion = header.indexOf('?');
    locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    //myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
  
    // we assume that the parameter "value" holds 3 number values delimited by ":" characters
    // they represent 3 arguments to this page and start after   ?value=
    AllThreeParams = header.substring(locOfQuestion+7);
    delimiter_1 = AllThreeParams.indexOf(":");
    delimiter_2 = AllThreeParams.indexOf(":", delimiter_1 +1);
    param1 = AllThreeParams.substring(0, delimiter_1);
    param2 = AllThreeParams.substring(delimiter_1 + 1, delimiter_2);
    param3 = AllThreeParams.substring(delimiter_2 + 1);
    param3.substring(0,1);

    // param1 holds selector index; 0 representing NO, 1 representing YES
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
      cam_send_cmd(CAM_CMD_HISTEQ_WANTED, myIndex);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_HISTEQ_WANTED, myIndex);
      config.cam_histeq_wanted = myIndex;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camg/histeq_wanted

    
    
} else if (header.indexOf("/wcmd/camg/negate_wanted") >= 0) {
    int action;
    int myIndex;
    
    String AllThreeParams, param1, param2, param3;
    int delimiter_1, delimiter_2;
    int locOfQuestion, locOfTrailingSpace;
      
    locOfQuestion = header.indexOf('?');
    locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    //myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
  
    // we assume that the parameter "value" holds 3 number values delimited by ":" characters
    // they represent 3 arguments to this page and start after   ?value=
    AllThreeParams = header.substring(locOfQuestion+7);
    delimiter_1 = AllThreeParams.indexOf(":");
    delimiter_2 = AllThreeParams.indexOf(":", delimiter_1 +1);
    param1 = AllThreeParams.substring(0, delimiter_1);
    param2 = AllThreeParams.substring(delimiter_1 + 1, delimiter_2);
    param3 = AllThreeParams.substring(delimiter_2 + 1);
    param3.substring(0,1);

    // param1 holds selector index; 0 representing NO, 1 representing YES
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
      cam_send_cmd(CAM_CMD_NEGATE_WANTED, myIndex);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_NEGATE_WANTED, myIndex);
      config.cam_negate_wanted = myIndex;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camg/negate_wanted

    
  
  } else if (header.indexOf("/wcmd/camg/perspective_factor") >= 0) { 
    int action;
    float myFloat;
    
    String AllThreeParams, param1, param2, param3;
    int delimiter_1, delimiter_2;
    int locOfQuestion, locOfTrailingSpace;
      
    locOfQuestion = header.indexOf('?');
    locOfTrailingSpace = header.indexOf(' ', locOfQuestion);
    //myValue = header.substring(locOfQuestion+7, locOfTrailingSpace); 
  
    // we assume that the parameter "value" holds 3 number values delimited by ":" characters
    // they represent 3 arguments to this page and start after   ?value=
    AllThreeParams = header.substring(locOfQuestion+7);
    delimiter_1 = AllThreeParams.indexOf(":");
    delimiter_2 = AllThreeParams.indexOf(":", delimiter_1 +1);
    param1 = AllThreeParams.substring(0, delimiter_1);
    param2 = AllThreeParams.substring(delimiter_1 + 1, delimiter_2);
    param3 = AllThreeParams.substring(delimiter_2 + 1);
    param3.substring(0,1);

    // param1 holds "weight" value (0.0 = 6.0)
    // note could be isFloat(String), String.toFloat   OR  isInteger(String), String.toInt
    // note that the isFloat() and isInteger() methods are defined in util.cpp
    if (isFloat(param1)) {
      myFloat = param1.toFloat();
      if (myFloat < 0.05) {
        return "ERROR value must be positive (0.05 - 0.4)";
      }
      if (myFloat > 4.0) {
        return "ERROR value too large, must be (0.05 - 0.4)";
      }
    } else {
      return "ERROR only floating point numbers allowed (0.05 - 0.4)";   // non-numeric
    }
    
    // param3 holds action command (0=set, 1=save as default)
    int myInt = param3.toInt();
    if (myInt == 0) {
      // index 0 indicates just SET value
      cam_send_cmd(CAM_CMD_CAM_SET_PERSPECTIVE_FACTOR, myFloat);
      return "SUCCESS sent value to camera";
    } else if (myInt == 1) {
      // index 1 indicates SET values and SAVE AS DEFAULT
      cam_send_cmd(CAM_CMD_CAM_SET_PERSPECTIVE_FACTOR, myFloat);
      config.cam_perspective_factor = myFloat;
      cfg_save();
      return "SUCCESS sent values to camera and SAVED as DEFAULT";
    } else {
      return "ERROR unexpected action value sent";
    }
    // end of /wcmd/camg/perspective_factor

      
      
  } 
  return "NOMATCH";
}   
