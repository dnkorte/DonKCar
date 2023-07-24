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
#include "mode_mgr.h"
#include "status.h"
#include "drivetrain.h"
#include "webap_core.h"
#include "cam.h"

#define HEARTBEAT_MAX 8             // heartbeat (nunchuk) timeout in 500 mS increments ( = 4 seconds)
#define MENU_TIMEOUT 15             // menu timeout in seconds
#define WEBAP_TIMEOUT 60            // timeout during web config mode, in 500 mS increments (= 30 seconds)
#define WEBAP_INITIAL_TIMEOUT 150   // timeout allows time for user to connect, in 500 mS increments
// note the web configuration page sends heartbeat every 5 seconds, when running
// we allow 30 seconds missing beats before terminating web config mode (except at startup, when
// we allow 75 seconds for user to get browser connected)

int curMode, lastMode;
int heartbeat_downcounter;
int menu_downcounter;
int webap_downcounter;
int menu_index;
bool but_C_status, but_Z_status;    // true = pressed, false = not pressed
bool but_U_status, but_D_status;    // based on joyY if at > 80% of full travel
bool but_R_status, but_L_status;    // based on joyX if at > 80% of full travel

int16_t current_steer_angle, last_steer_angle;
int16_t angle_error;
char speed_mode_color;   // color code for neopixel display center section based on speed mode
char speed_creep_normal_boost_reverse;      // 'C', 'N', 'B', or 'R'
int  speed_mode_straight_throttle;          // throttle setting for non-turning operation
int cmd_joyX, cmd_joyY;    // as-commanded values


#define NUM_MENU_ITEMS 5
String menuStrings[] = {
  "IDLE", "Manual Drive", "Autonomous Drive", "Web Configurator",  "Quick Setup"
};

int menuItems[] = {
  MODE_IDLE, MODE_MANUAL2, MODE_AUTO, MODE_WAITING_CNX, MODE_QUICKSETUP
};

int menuColors_NEO[] = {
  NEO_COLOR_YELLOW, NEO_COLOR_BLUE, NEO_COLOR_GREEN, NEO_COLOR_PURPLE, NEO_COLOR_ORANGE
};

/*
 * templates for private functions
 */
 void mode_but_U_clicked_action();
 void mode_but_D_clicked_action();
 void mode_but_L_clicked_action();
 void mode_but_R_clicked_action();

/*
 * public functions
 */

void mode_init(void) {
  status_init();
  mode_set_mode(MODE_INITIALIZING);
  heartbeat_downcounter = HEARTBEAT_MAX;
  lastMode = 0;
  but_C_status = false;
  but_Z_status = false;
  but_U_status = false;
  but_D_status = false;
  but_L_status = false;
  but_R_status = false;
  current_steer_angle = 0;
  last_steer_angle = 0;
  angle_error = 0;
  
  speed_mode_color = 'W';
  speed_creep_normal_boost_reverse = 'N';
  speed_mode_straight_throttle = config.manual_speed_normal;
}

void mode_notice_heartbeat(void) {
  heartbeat_downcounter = HEARTBEAT_MAX;
}

void mode_notice_menuaction(void) {
  menu_downcounter = MENU_TIMEOUT;
}

void mode_notice_webap_heartbeat(void) {
  webap_downcounter = WEBAP_TIMEOUT;
  if (curMode == MODE_WAITING_CNX) {
    mode_set_mode(MODE_CONFIGURING);
  } 
}

void mode_init_webap_heartbeat() {
  webap_downcounter = WEBAP_INITIAL_TIMEOUT;
}

void mode_check_heartbeat() {  
  heartbeat_downcounter-- ;
  if ((heartbeat_downcounter < 0) && ((curMode == MODE_MANUAL1)  || (curMode == MODE_MANUAL2) || (curMode == MODE_AUTO))) {
    drivetrain_stop();
    mode_set_mode(MODE_ERROR_HBEAT);
  }  
}

void mode_check_menu_timeout() {  
  menu_downcounter-- ;
  if ((menu_downcounter < 0) && (curMode == MODE_MENU)) {
    mode_set_mode(MODE_IDLE);
  }  
}

void mode_check_webap_heartbeat() { 
  webap_downcounter-- ;
  if (curMode == MODE_WAITING_CNX) {
    status_disp_webconnect_downcounter(webap_downcounter);
  }
  if ((webap_downcounter < 0) && ((curMode == MODE_CONFIGURING) || (curMode == MODE_WAITING_CNX))) {
    webap_deinit(1);          // terminated due to lost signal
    mode_set_mode(MODE_IDLE);
  } 
}

void mode_set_mode(int newMode) {
  if (curMode != MODE_MENU) {
    lastMode = curMode;   // keep "current" mode so menu indexer cah start there
  }
  curMode = newMode;
  switch (newMode) {
    case MODE_INITIALIZING:
      status_disp_menu_msg("Initializing", 'C');
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_RAINBOW_GRAY);
      drivetrain_stop();    
      break;
    case MODE_IDLE:
      status_disp_menu_msg("Idle", 'Y');
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_RAINBOW_FULL);
      drivetrain_stop();
      drivetrain_disable();
      cam_send_cmd(CAM_CMD_MODE_IDLE);  
      cam_send_cmd(CAM_CMD_DRIVE_OFF);
      break;
    case MODE_MANUAL1:  // note not currently used
      status_disp_menu_msg("Manual Throt/Steer", 'B');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_CYAN);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SET_WIN_CTR, 0);
      status_neo_send(NEO_CMD_SET_WIN_WIDTH, 5);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      status_neo_show_movement_info(0, 0, true);
      drivetrain_enable();
      cam_send_cmd(CAM_CMD_MODE_IDLE);  
      cam_send_cmd(CAM_CMD_DRIVE_OFF);
      break;
    case MODE_MANUAL2:
      // this used to be CYAN
      status_disp_menu_msg("Manual Steer", 'B');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_BLUE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SET_WIN_CTR, 0);
      status_neo_send(NEO_CMD_SET_WIN_WIDTH, 5);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      status_neo_show_movement_info(0, 0, true);
      drivetrain_enable();
      cam_send_cmd(CAM_CMD_MODE_IDLE);
      cam_send_cmd(CAM_CMD_DRIVE_OFF);
      break;
    case MODE_AUTO:
      status_disp_menu_msg("Autonomous Drive", 'G');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_GREEN);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      status_neo_show_movement_info(0, 0, true);
      drivetrain_enable(); 
      cam_preset_paremeters();
      cam_enter_preferred_mode();   
      cam_send_cmd(CAM_CMD_DRIVE_ON);  
      break;
    case MODE_WAITING_CNX:                                    
      //status_disp_menu_msg("Waiting Connect", 'P');
      status_disp_info_msgs("Connecting", "to web browser", "Nunchuk Not Avail", 'O'); 
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_PURPLE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_BLACK);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_FLASHING);
      drivetrain_stop();
      drivetrain_disable();
      delay(200);   // time to get message sent out to nunchuk
      mode_init_webap_heartbeat();
      webap_init();
      break;
    case MODE_CONFIGURING:
      status_disp_menu_msg("Web Configurator", 'P');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_PURPLE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_SOLID);
      drivetrain_stop();
      drivetrain_disable();
      cam_enter_preferred_mode();   
      cam_send_cmd(CAM_CMD_DRIVE_OFF);
      status_disp_info_msgs("USING WEB BROWSER", "TO CONFIGURE", "Nunchuk Not Avail", 'O'); 
      break;
    case MODE_QUICKSETUP:
      status_disp_menu_msg("Quick Setup", 'O');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_ORANGE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_SOLID);
      drivetrain_enable();
      break;
    case MODE_MENU:
      status_disp_menu_msg("Menu", 'W');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_GRAY);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      //menu_index = 0;
      mode_notice_menuaction();
      drivetrain_stop();
      drivetrain_disable();
      break;
    case MODE_ERROR_BATT:
      status_disp_menu_msg("Battery Very Low", 'R');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_RED);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_BLACK);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_FLASHING);
      drivetrain_stop();
      drivetrain_disable();
      cam_send_cmd(CAM_CMD_MODE_IDLE);  
      cam_send_cmd(CAM_CMD_DRIVE_OFF);
      break;
    case MODE_ERROR_HBEAT:
      status_disp_menu_msg("No Nunchuk Detected", 'O');
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_ORANGE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_BLACK);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_FLASHING);
      drivetrain_stop();
      drivetrain_disable();
      cam_send_cmd(CAM_CMD_MODE_IDLE);  
      cam_send_cmd(CAM_CMD_DRIVE_OFF);
      break;
  }
}

bool mode_motion_permitted() {
  if ((curMode == MODE_MANUAL1) || (curMode == MODE_MANUAL2) || (curMode == MODE_AUTO)) {
      return true;    
  }
  return false;
}

void mode_joyX_event(int myValue) { 
  if (curMode == MODE_MANUAL1) {      
    cmd_joyX = myValue;  
    drivetrain_go(cmd_joyY, cmd_joyX);
  }
  if ((curMode == MODE_MANUAL2) && (but_Z_status)) {      
    cmd_joyX = myValue;  
    if (abs(cmd_joyX) > config.manual_turn_threshold) {
      cmd_joyY = config.manual_speed_turns;
      speed_mode_color = 'Y';
    } else {
      cmd_joyY = speed_mode_straight_throttle;
      speed_mode_color = speed_mode_color;
    }
    drivetrain_go(cmd_joyY, cmd_joyX);
  }

  bool last_but_R_status = but_R_status;
  bool last_but_L_status = but_L_status;
  if (myValue > 180) {
    but_R_status = true;
  } else {
    but_R_status = false;
  }
  if (myValue < -180) {
    but_L_status = true;
  } else {
    but_L_status = false;
  }
  
  if (last_but_R_status && !but_R_status) {
    mode_but_R_clicked_action();
  }
  if (last_but_L_status && !but_L_status) {
    mode_but_L_clicked_action();
  }
}

void mode_joyY_event(int myValue) { 
  // note in MODE_MANUAL1 we disallow reverse (unless nunchuk button is pressed to allow it)
  // to avoid issues with ESC mode setting;  note right now only the disallow is coded
  if (curMode == MODE_MANUAL1) { 
    if (myValue >= 0) {   
      cmd_joyY = constrain(myValue, -255, 255);  
    } else {
      cmd_joyY = 0;
    }
    drivetrain_go(cmd_joyY, cmd_joyX);
  }
  if (curMode == MODE_MANUAL2) { 
    // ignore joystick Y events in MODE_MANUAL2
  }

  bool last_but_U_status = but_U_status;
  bool last_but_D_status = but_D_status;
  if (myValue > 180) {
    but_U_status = true;
  } else {
    but_U_status = false;
  }
  if (myValue < -180) {
    but_D_status = true;
  } else {
    but_D_status = false;
  }
  
  if (last_but_U_status && !but_U_status) {
    mode_but_U_clicked_action();
  }
  if (last_but_D_status && !but_D_status) {
    mode_but_D_clicked_action();
  }
}

/*
 * note this function is called when a message is received from camera
 * the "steer_angle" should be an integer -255 full left, +255 full right, 0=straight
 */
void mode_got_msg_steerangle(int16_t steer_angle, int16_t error_in_angle) {
  if (curMode == MODE_AUTO) { 
    
    last_steer_angle = current_steer_angle;
    current_steer_angle = constrain(steer_angle, -255, 255);
    status_disp_simple_msg(String(current_steer_angle), 'Y');

    if (but_Z_status) {         
      cmd_joyX = constrain(current_steer_angle, -255, 255);        
      if (abs(current_steer_angle) > config.manual_turn_threshold) {
        cmd_joyY = config.manual_speed_turns;
        speed_mode_color = 'Y';
      } else {
        cmd_joyY = speed_mode_straight_throttle;
        speed_mode_color = speed_mode_color;
      }         
    } else {
      //DEBUG_PRINTLN(" BUT Z OFF");
      cmd_joyY = 0;
    }
    drivetrain_go(cmd_joyY, cmd_joyX);
  }
}

/*
 * called when activity from z button on nunchuk
 * note this is the "bottom" (bigger) button, closer to the bottom of the nunchuk
 * @param:   action   1=pressed, 0=released
 */
void mode_z_button_event(int action) {
  if (action == 1) {
    but_Z_status = true;
  } else {
    but_Z_status = false;
  }
  if ((curMode == MODE_MENU) && (action == 1)) {
    mode_menu_itemselect();
  }
  if ((curMode == MODE_MANUAL2) && (action == 0)) {
    cmd_joyY = 0;
    cmd_joyX = 0;
    drivetrain_go(cmd_joyY, cmd_joyX);
  }
  if ((curMode == MODE_MANUAL2) && (action == 1)) {
    cmd_joyY = config.manual_speed_normal;
    cmd_joyX = 0;
    drivetrain_go(cmd_joyY, cmd_joyX);
  }
}

/*
 * called when activity from c button on nunchuk
 * note this is the "upper" button, closest to the top of the nunchuk
 * @param:   action   1=pressed, 0=released
 */
void mode_c_button_event(int action) {
  if (action == 1) {
    but_C_status = true;
  } else {
    but_C_status = false;
  }
  if (action == 1) {
    mode_menu_indexer();
  }
}

bool mode_check_but_U() {
  return but_U_status;
}

bool mode_check_but_D() {
  return but_U_status;
}

bool mode_check_but_L() {
  return but_L_status;
}

bool mode_check_but_R() {
  return but_R_status;
}


void mode_menu_indexer() {
  if (curMode == MODE_MENU) {
    menu_index++;
    if (menu_index >= NUM_MENU_ITEMS) {
      menu_index = 0;
    }
  } else {
    menu_index = 0;
    mode_set_mode(MODE_MENU);
  }
  mode_notice_menuaction();
  status_neo_show_menu_psn5(menu_index, menuColors_NEO[menu_index]);
  status_disp_menu_msg(menuStrings[menu_index], 'Z');
}

void mode_menu_itemselect() {
  if (curMode == MODE_MENU) {
    mode_notice_menuaction();
    mode_set_mode(menuItems[menu_index]);
  }
}

char mode_get_speed_mode_color() {
  return speed_mode_color;
}

/* 
 * **********************************************
 * private functions
char speed_mode_color;   // color code for neopixel display center section based on speed mode
char speed_creep_normal_boost_reverse;      // 'C', 'N', 'B', or 'R'
int  speed_mode_straight_throttle;          // throttle setting for non-turning operation
 * **********************************************
 */
void mode_but_U_clicked_action() {
  if ((curMode == MODE_MANUAL2)) { 
    switch(speed_creep_normal_boost_reverse) {
      case 'C':
        speed_creep_normal_boost_reverse = 'N';
        speed_mode_straight_throttle = config.manual_speed_normal;
        speed_mode_color = 'W';
        break;
      case 'N':
        speed_creep_normal_boost_reverse = 'B';
        speed_mode_straight_throttle = config.manual_speed_boost;
        speed_mode_color = 'C';
        break;
      case 'B':
        // do nothing
        break;
      case 'R':
        // do nothing
        break;
    }
  }
  if ((curMode == MODE_QUICKSETUP)) { 
    // call action function for quicksetup menu
  }  
}

void mode_but_D_clicked_action() {
  if ((curMode == MODE_MANUAL2)) { 
    switch(speed_creep_normal_boost_reverse) {
      case 'C':
        // do nothing
        break;
      case 'N':
        speed_creep_normal_boost_reverse = 'C';
        speed_mode_straight_throttle = config.manual_speed_creep;
        speed_mode_color = 'P';
        break;
      case 'B':
        speed_creep_normal_boost_reverse = 'N';
        speed_mode_straight_throttle = config.manual_speed_normal;
        speed_mode_color = 'W';
        break;
        break;
      case 'R':
        // do nothing
        break;
    }    
  }
  if ((curMode == MODE_QUICKSETUP)) { 
    // call action function for quicksetup menu
  }
}

void mode_but_L_clicked_action() {
  if ((curMode == MODE_QUICKSETUP)) { 
    // call action function for quicksetup menu
  }
}

void mode_but_R_clicked_action() {
  if ((curMode == MODE_QUICKSETUP)) { 
    // call action function for quicksetup menu
  }
}
