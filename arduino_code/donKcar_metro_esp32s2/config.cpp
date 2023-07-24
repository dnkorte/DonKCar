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
/*
 * ******************************************************************************
 * ******************************************************************************
 * this module gets and manages robot configuration parameters
 * the config.h file has default values for all user-configurable parameters
 * but cfg_init() attempts to read an SD card to get "actual" values for
 * these parameters.   It is based significantly on the following modules
 *    https://arduinojson.org/v6/example/config/
 *    https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
 * it assumes that an attached SD card has a file called "/config.txt"
 * which is a json file with the specified values
 * ******************************************************************************
 * ******************************************************************************
 */
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>                // SD card & FAT filesystem library (adafruit version)
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library (adafruit version)
#include <ArduinoJson.h>
#include "config.h"
#include "util.h"
#include "status.h"

 /*
 * ******************************************************************************
 * ******************************************************************************
 * DEFAULT VALUES for user-configurable parameters
 * generally these do not need to be changed as they are
 * suitable generic values for getting the racer up and running
 *
 * when changes are made to a specific racer's configuration
 * they should be done using the web-based configuration utility
 * or by directly editing the json configuration on the SD card (/config.txt) 
 * ******************************************************************************
 * ******************************************************************************
 */

#define ROBOT_NAME "myracer"    // note this should be limited to 8 characters

#define STEER_CENTER_US  1440   // was 1440
#define STEER_LEFT_US    1100   // was 1020
#define STEER_RIGHT_US   1750   // was 1750

#define STEERING_FRACTION 0.12    // (note could set this to 0.35 if using superslow MAX_THROT 160)

#define MANUAL_SPEED_CREEP 64     // driving speed during manual creep (-255 to +255) 
#define MANUAL_SPEED_NORMAL 128   // normal driving throttle (-255 to +255)
#define MANUAL_SPEED_BOOST 128   // normal driving throttle (-255 to +255)
#define MANUAL_SPEED_TURNS  64    // throttle on turns
#define MANUAL_TURN_THRESHOLD 32  // steering PWM above which MANUAL_SPEED_TURNS is triggered
#define AUTO_SPEED_NORMAL 128     // normal driving throggle
#define AUTO_SPEED_TURNS 64       // throttle on turns
#define THROT_START_PWM 32        // pwm for start pulse when starting from stopped condition
#define THROT_START_DURATION 2    // duration of start pulse in 10 mS increments
#define ACCEL_LIMIT 20          

#define ESC_ARM_SIGNAL 1000       // uS to write during arming period
#define ESC_ARM_TIME 3000         // mS long for arming
#define ESC_MIN_THROTTLE 1070
#define ESC_MAX_THROTTLE 1852
#define ESC_REVERSE_THROTTLE 1502 // orig 1488

#define BATT_E_SCALE 7.212      // this times raw a/d raw reading = mV
#define BATT_E_NUMCELLS 2       // typically 3 (11.1v 3S) or 2 (7.4v 2S)
#define BATT_E_USED true        // true = this battery is used, false = not used
#define BATT_E_MIN_GREEN  3.75   // cell voltages above this will be displayed in green
#define BATT_E_MIN_YELLOW 3.5
#define BATT_E_MIN_ORANGE 3.28   // below this value will be displayed in red and cause shutdown
#define BATT_E_SHUTDOWN_ON_RED true  // shutdown racer if battery goes red

#define BATT_M_SCALE 7.212      // this times raw a/d raw reading = mV
#define BATT_M_NUMCELLS 2       // typically 3 (11.1v 3S) or 2 (7.4v 2S)
#define BATT_M_USED false        // true = this battery is used, false = not used
#define BATT_M_MIN_GREEN  3.75   // cell voltages above this will be displayed in green
#define BATT_M_MIN_YELLOW 3.5
#define BATT_M_MIN_ORANGE 3.28   // below this value will be displayed in red and cause shutdown
#define BATT_M_SHUTDOWN_ON_RED false  // shutdown racer if battery goes red

#define CAM_STARTUP_MODE 0      // 0 for BLOBS, 1 for LINES (structure doesn't allow char)
#define BLOB_ROI_T_LOC 50
#define BLOB_ROI_T_HEIGHT 19
#define BLOB_ROI_M_LOC 70
#define BLOB_ROI_M_HEIGHT 19
#define BLOB_ROI_B_LOC 90
#define BLOB_ROI_B_HEIGHT 19
#define BLOB_ROI_T_WEIGHT 0.45
#define BLOB_ROI_M_WEIGHT 0.25
#define BLOB_ROI_B_WEIGHT 0.35
#define BLOB_FLOAT_THRESH 0.2
#define BLOB_SEED_THRESH 0.4
#define BLOB_SEED_LOC 100
#define PID_KP 300
#define PID_KI 20
#define PID_KD 20
#define PID_STEERING_DIRECTION 1    // deprecated
#define PID_STEERING_GAIN 1.0       // deprecated
#define CAM_PERSPECTIVE_FACTOR 0.2
#define CAM_PERSPECTIVE_WANTED 0    // by index; 0 = NO, 1 = YES
  
SdFat                SD;         // SD card filesystem    (adafruit version)
  
/*
 * *********************************************************
 * *********************************************************
 * private functions, copied from 
 *    https://arduinojson.org/v6/example/config/
 *    
 * Note if new parameters are added to the configuration   
 * object, make sure they don't make it too big for the
 * size allocated.  the allocation size is specified
 * at 2 locations in this file in the StaticJsonDocument 
 * statement -- currently set to 1024 bytes
 * *********************************************************
 * *********************************************************
 */

// Loads the configuration from a file
void loadConfiguration(const char *filename, Config &config) {
  // Open file for reading
  File file = SD.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/v6/assistant/#/step1 to compute the capacity.
  StaticJsonDocument<4096> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) {   
    status_disp_simple_msg("Couldn't read file", 'O');
    delay(2000);
    status_disp_simple_msg("Using default config", 'O');
  }

  // Copy values from the JsonDocument to the Config
  config.steer_center_us = doc["STEER_CENTER_US"] | STEER_CENTER_US;
  config.steer_left_us = doc["STEER_LEFT_US"] | STEER_LEFT_US;
  config.steer_right_us = doc["STEER_RIGHT_US"] | STEER_RIGHT_US;
  
  config.batt_E_scale = doc["BATT_E_SCALE"] | BATT_E_SCALE;
  config.batt_E_numcells = doc["BATT_E_NUMCELLS"] | BATT_E_NUMCELLS;
  config.batt_E_used = doc["BATT_E_USED"] | BATT_E_USED;
  config.batt_E_min_green = doc["BATT_E_MIN_GREEN"] | BATT_E_MIN_GREEN;
  config.batt_E_min_yellow = doc["BATT_E_MIN_YELLOW"] | BATT_E_MIN_YELLOW;
  config.batt_E_min_orange = doc["BATT_E_MIN_ORANGE"] | BATT_E_MIN_ORANGE;
  config.batt_E_shutdown_on_red = doc["BATT_E_SHUTDOWN_ON_RED"] | BATT_E_SHUTDOWN_ON_RED;
  
  config.batt_M_scale = doc["BATT_M_SCALE"] | BATT_M_SCALE;
  config.batt_M_numcells = doc["BATT_M_NUMCELLS"] | BATT_M_NUMCELLS;
  config.batt_M_used = doc["BATT_M_USED"] | BATT_M_USED;
  config.batt_M_min_green = doc["BATT_M_MIN_GREEN"] | BATT_M_MIN_GREEN;
  config.batt_M_min_yellow = doc["BATT_M_MIN_YELLOW"] | BATT_M_MIN_YELLOW;
  config.batt_M_min_orange = doc["BATT_M_MIN_ORANGE"] | BATT_M_MIN_ORANGE;   
  config.batt_M_shutdown_on_red = doc["BATT_M_SHUTDOWN_ON_RED"] | BATT_M_SHUTDOWN_ON_RED;
  
  config.steering_fraction = doc["STEERING_FRACTION"] | STEERING_FRACTION;
  
  config.manual_speed_creep = doc["MANUAL_SPEED_CREEP"] | MANUAL_SPEED_CREEP; 
  config.manual_speed_normal = doc["MANUAL_SPEED_NORMAL"] | MANUAL_SPEED_NORMAL;
  config.manual_speed_boost = doc["MANUAL_SPEED_BOOST"] | MANUAL_SPEED_BOOST;
  config.manual_speed_turns = doc["MANUAL_SPEED_TURNS"] | MANUAL_SPEED_TURNS;
  config.manual_turn_threshold = doc["MANUAL_TURN_THRESHOLD"] | MANUAL_TURN_THRESHOLD;
  config.auto_speed_normal = doc["AUTO_SPEED_NORMAL"] | AUTO_SPEED_NORMAL;
  config.auto_speed_turns = doc["AUTO_SPEED_TURNS"] | AUTO_SPEED_TURNS;
  config.throt_start_pwm = doc["THROT_START_PWM"] | THROT_START_PWM;
  config.throt_start_duration = doc["THROT_START_DURATION"] | THROT_START_DURATION;
  config.accel_limit = doc["ACCEL_LIMIT"] | ACCEL_LIMIT;
  
  config.esc_arm_signal = doc["ESC_ARM_SIGNAL"] | ESC_ARM_SIGNAL;
  config.esc_arm_time = doc["ESC_ARM_TIME"] | ESC_ARM_TIME;
  config.esc_min_throttle = doc["ESC_MIN_THROTTLE"] | ESC_MIN_THROTTLE;
  config.esc_max_throttle = doc["ESC_MAX_THROTTLE"] | ESC_MAX_THROTTLE;
  config.esc_reverse_throttle = doc["ESC_REVERSE_THROTTLE"] | ESC_REVERSE_THROTTLE; 

  config.cam_startup_mode = doc["CAM_STARTUP_MODE"] | CAM_STARTUP_MODE; 
  config.blob_roiTloc = doc["BLOB_ROI_T_LOC"] | BLOB_ROI_T_LOC; 
  config.blob_roiTheight = doc["BLOB_ROI_T_HEIGHT"] | BLOB_ROI_T_HEIGHT; 
  config.blob_roiMloc = doc["BLOB_ROI_M_LOC"] | BLOB_ROI_M_LOC; 
  config.blob_roiMheight = doc["BLOB_ROI_M_HEIGHT"] | BLOB_ROI_M_HEIGHT; 
  config.blob_roiBloc = doc["BLOB_ROI_B_LOC"] | BLOB_ROI_B_LOC; 
  config.blob_roiBheight = doc["BLOB_ROI_B_HEIGHT"] | BLOB_ROI_B_HEIGHT; 
  config.blob_roiTweight = doc["BLOB_ROI_T_WEIGHT"] | BLOB_ROI_T_WEIGHT; 
  config.blob_roiMweight = doc["BLOB_ROI_M_WEIGHT"] | BLOB_ROI_M_WEIGHT; 
  config.blob_roiBweight = doc["BLOB_ROI_B_WEIGHT"] | BLOB_ROI_B_WEIGHT; 
  config.blob_float_thresh = doc["BLOB_FLOAT_THRESH"] | BLOB_FLOAT_THRESH; 
  config.blob_seed_thresh = doc["BLOB_SEED_THRESH"] | BLOB_SEED_THRESH; 
  config.blob_seed_loc = doc["BLOB_SEED_LOC"] | BLOB_SEED_LOC; 
  config.pid_kp = doc["PID_KP"] | PID_KP; 
  config.pid_ki = doc["PID_KI"] | PID_KI; 
  config.pid_kd = doc["PID_KD"] | PID_KD; 
  config.pid_steering_direction = doc["PID_STEERING_DIRECTION"] | PID_STEERING_DIRECTION; 
  config.pid_steering_gain = doc["PID_STEERING_GAIN"] | PID_STEERING_GAIN;   
  config.cam_perspective_factor = doc["CAM_PERSPECTIVE_FACTOR"] | CAM_PERSPECTIVE_FACTOR;  
  config.cam_perspective_wanted = doc["CAM_PERSPECTIVE_WANTED"] | CAM_PERSPECTIVE_WANTED;  
 
  strlcpy(config.robot_name,                  // <- destination
          doc["ROBOT_NAME"] | ROBOT_NAME,     // <- source
          sizeof(config.robot_name));         // <- destination's capacity
  config.readconfirm = doc["READCONFIRM"] | 0;
    
  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

// Saves the configuration to a file
void saveConfiguration(const char *filename, const Config &config) {
  // Delete existing file, otherwise the configuration is appended to the file
  SD.remove(filename);

  // Open file for writing
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    status_disp_simple_msg("Couldn't create file", 'R');
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<4096> doc;

  // Set the values in the document
  doc["ROBOT_NAME"] = config.robot_name;
  doc["STEER_CENTER_US"] = config.steer_center_us;
  doc["STEER_LEFT_US"] = config.steer_left_us;
  doc["STEER_RIGHT_US"] = config.steer_right_us;
  
  doc["BATT_E_SCALE"] = config.batt_E_scale;
  doc["BATT_E_NUMCELLS"] = config.batt_E_numcells;
  doc["BATT_E_USED"] = config.batt_E_used;
  doc["BATT_E_MIN_GREEN"] = config.batt_E_min_green;
  doc["BATT_E_MIN_YELLOW"] = config.batt_E_min_yellow;
  doc["BATT_E_MIN_ORANGE"] = config.batt_E_min_orange; 
  doc["BATT_E_SHUTDOWN_ON_RED"] = config.batt_E_shutdown_on_red;  
  
  doc["BATT_M_SCALE"] = config.batt_M_scale;
  doc["BATT_M_NUMCELLS"] = config.batt_M_numcells;
  doc["BATT_M_USED"] = config.batt_M_used;
  doc["BATT_M_MIN_GREEN"] = config.batt_M_min_green;
  doc["BATT_M_MIN_YELLOW"] = config.batt_M_min_yellow;
  doc["BATT_M_MIN_ORANGE"] = config.batt_M_min_orange; 
  doc["BATT_M_SHUTDOWN_ON_RED"] = config.batt_M_shutdown_on_red; 
  
  doc["STEERING_FRACTION"] = config.steering_fraction;

  doc["MANUAL_SPEED_CREEP"] = config.manual_speed_creep;   
  doc["MANUAL_SPEED_NORMAL"] = config.manual_speed_normal; 
  doc["MANUAL_SPEED_BOOST"] = config.manual_speed_boost;
  doc["MANUAL_SPEED_TURNS"] = config.manual_speed_turns;
  doc["MANUAL_TURN_THRESHOLD"] = config.manual_turn_threshold;
  doc["AUTO_SPEED_NORMAL"] = config.auto_speed_normal;
  doc["AUTO_SPEED_TURNS"] = config.auto_speed_turns;
  doc["THROT_START_PWM"] = config.throt_start_pwm;
  doc["THROT_START_DURATION"] = config.throt_start_duration;
  doc["ACCEL_LIMIT"] = config.accel_limit;   
  
  doc["ESC_ARM_SIGNAL"] = config.esc_arm_signal;
  doc["ESC_ARM_TIME"] = config.esc_arm_time;
  doc["ESC_MIN_THROTTLE"] = config.esc_min_throttle;
  doc["ESC_MAX_THROTTLE"] = config.esc_max_throttle;
  doc["ESC_REVERSE_THROTTLE"] = config.esc_reverse_throttle;

  doc["CAM_STARTUP_MODE"] = config.cam_startup_mode; 
  doc["BLOB_ROI_T_LOC"] = config.blob_roiTloc; 
  doc["BLOB_ROI_T_HEIGHT"] = config.blob_roiTheight;
  doc["BLOB_ROI_M_LOC"] = config.blob_roiMloc; 
  doc["BLOB_ROI_M_HEIGHT"] = config.blob_roiMheight; 
  doc["BLOB_ROI_B_LOC"] = config.blob_roiBloc; 
  doc["BLOB_ROI_B_HEIGHT"] = config.blob_roiBheight; 
  doc["BLOB_ROI_T_WEIGHT"] = config.blob_roiTweight; 
  doc["BLOB_ROI_M_WEIGHT"] = config.blob_roiMweight;  
  doc["BLOB_ROI_B_WEIGHT"] = config.blob_roiBweight;  
  doc["BLOB_FLOAT_THRESH"] = config.blob_float_thresh; 
  doc["BLOB_SEED_THRESH"] = config.blob_seed_thresh;  
  doc["BLOB_SEED_LOC"] = config.blob_seed_loc;  
  doc["PID_KP"] = config.pid_kp;  
  doc["PID_KI"] = config.pid_ki;  
  doc["PID_KD"] = config.pid_kd;  
  doc["PID_STEERING_DIRECTION"] = config.pid_steering_direction;  
  doc["PID_STEERING_GAIN"] = config.pid_steering_gain;     
  doc["CAM_PERSPECTIVE_FACTOR"] = config.cam_perspective_factor;    
  doc["CAM_PERSPECTIVE_WANTED"] = config.cam_perspective_wanted; 

  doc["READCONFIRM"] = config.readconfirm;  

  // Serialize JSON to file
  //if (serializeJson(doc, file) == 0) {
  if (serializeJsonPretty(doc, file) == 0) {
    status_disp_simple_msg("Couldn't write file", 'R');
  }

  // Close the file
  file.close();
}

/*
 * *********************************************************
 * *********************************************************
 * public functions, copied from 
 * *********************************************************
 * *********************************************************
 */

// Globals globals;  // <- global variables (not part of config object)

Config config;    // <- global configuration object 
const char *filename = "/config.txt";  // <- SD library uses 8.3 filenames

void cfg_init(void) {
  util_enable_my_spi(PIN_SD_CS);
  
  //while (!SD.begin(PIN_SD_CS, SD_SCK_MHZ(10))) {  // Breakouts require 10 MHz limit due to longer wires
  while (!SD.begin(PIN_SD_CS)) {  // Breakouts require 10 MHz limit due to longer wires
    status_disp_simple_msg("Couldn't init SD lib", 'R'); 
  }
  util_enable_my_spi(PIN_SD_CS);
  
  loadConfiguration(filename, config);
  //globals.batt_show_raw = false;
}


void cfg_save(void) {
  util_enable_my_spi(PIN_SD_CS);
  // Initialize SD library
  while (!SD.begin(PIN_SD_CS)) {
    status_disp_simple_msg("Couldn't init SD lib", 'R');
  }
  util_enable_my_spi(PIN_SD_CS);
  saveConfiguration(filename, config);  
}


// Prints the content of a file to a String and returns it
String cfg_showfile(void) {
  String mystring;

  util_enable_my_spi(PIN_SD_CS);
  delay(1000);
  while (!SD.begin(PIN_SD_CS)) {
    status_disp_simple_msg("Couldn't init SD lib", 'R');
  }
  
  mystring = "";
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    return "Failed to read file (in cfg_showfile)";
  }

  util_enable_my_spi(PIN_SD_CS);
  // Extract each characters by one by one
  while (file.available()) {
    mystring += (char)file.read();
  }
  
  // Close the file
  file.close();
  return mystring;
}
