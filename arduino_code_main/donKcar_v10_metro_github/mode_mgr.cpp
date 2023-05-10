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
#include "screen.h"
#include "status.h"
#include "drivetrain.h"
#include "webap_core.h"

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
bool but_C_status, but_Z_status;  // true = pressed, false = not pressed

int cmd_joyX, cmd_joyY;    // as-commanded values


#define NUM_MENU_ITEMS 5
String menuStrings[] = {
  "IDLE", "Manual Drive", "Autonomous Drive", "Configurator",  "Learn"
};

int menuItems[] = {
  MODE_IDLE, MODE_MANUAL2, MODE_AUTO, MODE_WAITING_CNX, MODE_LEARNING
};


int menuColors_SCREEN[] = {
  COLOR_YELLOW, COLOR_BLUE, COLOR_GREEN, COLOR_PURPLE, COLOR_ORANGE
};

int menuColors_NEO[] = {
  NEO_COLOR_YELLOW, NEO_COLOR_BLUE, NEO_COLOR_GREEN, NEO_COLOR_PURPLE, NEO_COLOR_ORANGE
};

/*
 * private functions
 */

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
  char tmpBuf[6];  
  webap_downcounter-- ;
  if (curMode == MODE_WAITING_CNX) {
    itoa(webap_downcounter/2, tmpBuf,10);
    screen_writeText_colrow(COL_LEFTEDGE+17, ROW_STAT2, 3, tmpBuf, COLOR_YELLOW);
    // in last 15 seconds of "wait for connection" mode, we change neopixels to flash yellow and purple
    if (webap_downcounter == 30) {
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_ORANGE);      
    }
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
      status_set_menu_msg("Initializing", COLOR_CYAN);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_RAINBOW_GRAY);
      drivetrain_stop();
      break;
    case MODE_IDLE:
      status_set_menu_msg("Idle", COLOR_YELLOW);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_RAINBOW_FULL);
      drivetrain_stop();
      drivetrain_disable();
      break;
    case MODE_MANUAL1:
      status_set_menu_msg("Manual Throt/Steer", COLOR_BLUE);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_CYAN);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SET_WIN_CTR, 0);
      status_neo_send(NEO_CMD_SET_WIN_WIDTH, 5);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      status_neo_show_movement_info(0, 0, true);
      drivetrain_enable();
      break;
    case MODE_MANUAL2:
      // this used to be CYAN
      status_set_menu_msg("Manual Steer", COLOR_BLUE);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_BLUE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SET_WIN_CTR, 0);
      status_neo_send(NEO_CMD_SET_WIN_WIDTH, 5);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      status_neo_show_movement_info(0, 0, true);
      drivetrain_enable();
      break;
    case MODE_AUTO:
      status_set_menu_msg("Auto Drive", COLOR_GREEN);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_GREEN);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      status_neo_show_movement_info(0, 0, true);
      drivetrain_enable();      
      break;
    case MODE_WAITING_CNX:                                    
      status_set_menu_msg("Waiting Connect", COLOR_PURPLE);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_PURPLE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_BLACK);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_FLASHING);
      drivetrain_stop();
      drivetrain_disable();
      mode_init_webap_heartbeat();
      webap_init();
      break;
    case MODE_CONFIGURING:
      status_set_menu_msg("Web Config Running", COLOR_PURPLE);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_PURPLE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_SOLID);
      drivetrain_stop();
      drivetrain_disable();
      screen_centerText(ROW_STAT1, "USING WEB BROWSER", COLOR_ORANGE);
      screen_centerText(ROW_STAT2, "TO CONFIGURE", COLOR_ORANGE);
      screen_centerText(ROW_STAT3, "Nunchuk Not Avail", COLOR_ORANGE);
      break;
    case MODE_LEARNING:
      status_set_menu_msg("Learning", COLOR_ORANGE);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_ORANGE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_SOLID);
      drivetrain_enable();
      break;
    case MODE_MENU:
      status_set_menu_msg("Menu", COLOR_WHITE);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_GRAY);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_WHITE);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
      //menu_index = 0;
      mode_notice_menuaction();
      drivetrain_stop();
      drivetrain_disable();
      break;
    case MODE_ERROR_BATT:
      status_set_menu_msg("Battery Very Low", COLOR_RED);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_RED);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_BLACK);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_FLASHING);
      drivetrain_stop();
      drivetrain_disable();
      break;
    case MODE_ERROR_HBEAT:
      status_set_menu_msg("No Nunchuk Detected", COLOR_ORANGE);
      status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_ORANGE);
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_BLACK);
      status_neo_send(NEO_CMD_SETMODE,NEO_MODE_FLASHING);
      drivetrain_stop();
      drivetrain_disable();
      break;
  }
}

bool mode_motion_permitted() {
  if ((curMode == MODE_MANUAL1) || (curMode == MODE_MANUAL2) || (curMode == MODE_AUTO) || (curMode == MODE_LEARNING)) {
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
    cmd_joyY = config.manual_speed_normal;
    drivetrain_go(cmd_joyY, cmd_joyX);
  }
}


void mode_joyY_event(int myValue) { 
  // note in MODE_MANUAL1 we disallow reverse (unless nunchuk button is pressed to allow it)
  // to avoid issues with ESC mode setting;  note right now only the disallow is coded
  if (curMode == MODE_MANUAL1) { 
    if (myValue >= 0) {   
      cmd_joyY = myValue;  
    } else {
      cmd_joyY = 0;
    }
    drivetrain_go(cmd_joyY, cmd_joyX);
  }
  if (curMode == MODE_MANUAL2) { 
    // ignore joystick Y events in MODE_MANUAL2
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
  status_set_menu_msg(menuStrings[menu_index], COLOR_MED_GRAY);
}

void mode_menu_itemselect() {
  if (curMode == MODE_MENU) {
    mode_notice_menuaction();
    mode_set_mode(menuItems[menu_index]);
  }
}