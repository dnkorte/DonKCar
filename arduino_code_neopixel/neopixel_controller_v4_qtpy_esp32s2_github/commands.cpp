/*
 * Summary: this package is a newpixel strip display driver for robots/racers
 *    it runs on an Adafruit QTPy ESP32-S2 board and drives a 24 element 
 *    NeoPixel Strip.  It receives commands over i2c from the robot main controller
 *    and generates approprate displays on the strip
 * 
 * Author(s):  Don Korte
 * Repository: https://github.com/dnkorte/DonKCar
 *
 * MIT License
 * Copyright (c) 2023 Don Korte
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * 0000000
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
#include "commands.h"
#include "i2c_com.h"
#include "general_support_lib.h"
#include "neo_functions.h"

/*
 * * private variables related to commands processor
 */
int i2c_waiting_parameters;             // 1=waiting on parameters for (some) command 
    
int i2c_inprocess_cmd_write_register2;   // i2c interrupts not waiting on params for CMD_WRITE_REGISTER2
int i2c_inprocess_cmd_write_register1;   // i2c interrupts not waiting on params for CMD_WRITE_REGISTER1

// these parameters support new multibyte structure 9/30/22
int i2c_inprocess_byte_index;           // index indicates where in buffer to put next character
int i2c_inprocess_num_bytes_expected;   // number of bytes expected
uint8_t i2c_inprocess_buffer[32];       // buffer to hold incoming parameter string

MyCallbackFunction i2c_inprocess_doit;  // callback function for multi-byte parameter command handler

uint8_t thisCommand, thisParam; 


/*
 * private functions related to commands processor
 */

void process_null_callback() {
  // do nothing
}

/* 
 *  private callback functions that implement command internals for multibyte commands
 */

void process_write_register_1byte() {
  // expects 2 1-byte params; first is register index, second is the value
  uint8_t temp1, temp2;
  temp1 = i2c_inprocess_buffer[0];
  temp2 = i2c_inprocess_buffer[1];
  i2c_register_set_8(temp1, temp2);
}


void process_write_register_2byte() {
  // expects 3 1-byte params; first is register index, second is low order value, third is high order value
  uint8_t temp1, temp2, temp3;
  int     value;
  temp1 = i2c_inprocess_buffer[0];
  temp2 = i2c_inprocess_buffer[1];
  temp3 = i2c_inprocess_buffer[2];
  value = temp2 | ((temp3 & 0xFF) << 8);
  i2c_register_set_16(temp1, value);
}

void process_setRR() {
  // set read register idex (expects one 1-byte unsigned integer) 
  int temp;
  temp = i2c_inprocess_buffer[0]; 
  i2c_register_set_8(REG_READREG, temp);
}

void process_set_background() {
  int parameter;
  parameter = i2c_inprocess_buffer[0];  
  // set background color
  if ((parameter < 0) || (parameter >= NUM_OF_COLORS)) {
    parameter = 0;
  }
  neo_set_background(parameter);
  if (neo_get_mode() == MODE_SOLID) {
    // if we're currently in solid display mode, then show it
    neo_fill_background();
  }  
}

void process_set_foreground() {
  int parameter;
  parameter = i2c_inprocess_buffer[0];
  // set foreground color
  if ((parameter < 0) || (parameter >= NUM_OF_COLORS)) {
    parameter = 0;
  } 
  neo_set_foreground(parameter); 
}

void process_set_win_ctr() {
  int parameter;
  parameter = i2c_inprocess_buffer[0];
  // set center of window (desire 0-22, default was 11)
  if ((parameter < 0) || (parameter >= 24)) {
    parameter = 11;
  }  
  neo_set_win_center(parameter);
}

void process_set_win_width() {
  int parameter;
  parameter = i2c_inprocess_buffer[0];
  // set width of window (desire 0-12, default was 8)
  if ((parameter < 0) || (parameter >= 12)) {
    parameter = 8;
  }
  neo_set_win_width(parameter);
}

void process_set_mode () {
  int parameter;
  parameter = i2c_inprocess_buffer[0];   
  switch(parameter) {
    case MODE_OFF:
      neo_set_mode(MODE_OFF);
      break;
    case MODE_RAINBOW:
      neo_set_mode(MODE_RAINBOW);
      break;
    case MODE_RAINBOW_BLUE:
      neo_set_mode(MODE_RAINBOW_BLUE);
      break;
    case MODE_RAINBOW_GRAY:
      neo_set_mode(MODE_RAINBOW_GRAY);
      break;
    case MODE_SOLID:
      neo_set_mode(MODE_SOLID);
      break;
    case MODE_FLASHING:
      neo_set_mode(MODE_FLASHING);
      break;
    case MODE_WINDOWED:
      neo_set_mode(MODE_WINDOWED);
      break;
  }
}       
  
/*
 * public functions related to commands processor
 */

void commands_init() {
  i2c_waiting_parameters = 0;             // not waiting to process a command parameter (ie next thing coming is a command)
  
  i2c_inprocess_cmd_write_register2 = 0;   // i2c interrupts not waiting on params for CMD_WRITE_REGISTER2
  i2c_inprocess_cmd_write_register1 = 0;   // i2c interrupts not waiting on params for CMD_WRITE_REGISTER1
  
  i2c_inprocess_num_bytes_expected = 0;   
  i2c_inprocess_byte_index = 0;           // 
  i2c_inprocess_doit = process_null_callback;
}

void commands_execute_cmd_if_avail() {  
  int temp;
  short signed16val;
    
  if (!i2c_stream_isEmpty()) {
    thisCommand = i2c_stream_pull(); 
    int cur_mode;

    //DEBUG_PRINT("Cmd Rcvd:");
    //DEBUG_PRINTLN(thisCommand);
    
    if (i2c_waiting_parameters == 1) {      
      // this grouping is for when expecting a PARAMETER, not a COMMAND
      
      //
      // this is the multibyte processor
      //
            
      if ((i2c_inprocess_byte_index >= 0) && (i2c_inprocess_byte_index < 32)) {
        i2c_inprocess_buffer[i2c_inprocess_byte_index++] = thisCommand;
        if (i2c_inprocess_byte_index >= i2c_inprocess_num_bytes_expected) {          
          i2c_inprocess_doit(); // have all the bytes, so call the callback function
          i2c_inprocess_num_bytes_expected = 0;
          i2c_inprocess_byte_index = 0;
          i2c_waiting_parameters = 0; 
          i2c_inprocess_doit = process_null_callback;         
        }
      } else {
          // this is error condition where byte index is out of range, so just cancel this whole sequence
          i2c_inprocess_num_bytes_expected = 0;
          i2c_inprocess_byte_index = 0;
          i2c_waiting_parameters = 0; 
          i2c_inprocess_doit = process_null_callback;           
      }   // if ((i2c_inprocess_byte_index >= 0) && (i2c_inprocess_byte_index < 32))

      
      //
      // this ends the multibyte processor
      // 
    
      } else {  // (if (i2c_waiting_parameters == 1) {  )
        
      // expecting a COMMAND, not a PARAMETER 
      switch(thisCommand) { 
               
        case NEO_CMD_WRITE_REGISTER2:
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 3; // expect 3 bytes register index, then 2 bytes of value
          i2c_inprocess_doit = process_write_register_2byte; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break;
          
        case NEO_CMD_WRITE_REGISTER1:
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 2; // expect 2 bytes register index, then 1 bytes of value
          i2c_inprocess_doit = process_write_register_1byte; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break; 
          
        case NEO_CMD_SET_READ_REGISTER:  // set read register          
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 1; // expect 1 byte register index
          i2c_inprocess_doit = process_setRR; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break; 
                    
        case NEO_CMD_SETMODE:
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 1; // expect 1 byte (mode desired)
          i2c_inprocess_doit = process_set_mode; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break;
        case NEO_CMD_SETBACKGROUND:
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 1; // expect 1 byte (mode desired)
          i2c_inprocess_doit = process_set_background; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break;
        case NEO_CMD_SETFOREGROUND:
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 1; // expect 1 byte (mode desired)
          i2c_inprocess_doit = process_set_foreground; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break;
        case NEO_CMD_SET_WIN_CTR:
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 1; // expect 1 byte (mode desired)
          i2c_inprocess_doit = process_set_win_ctr; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break;
        case NEO_CMD_SET_WIN_WIDTH:
          i2c_inprocess_byte_index = 0;
          i2c_inprocess_num_bytes_expected = 1; // expect 1 byte (mode desired)
          i2c_inprocess_doit = process_set_win_width; 
          i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
          break;       
          
        //case CMD_DRIVE_PCT_R:      // (example for a 1-byte parameter
        //  i2c_inprocess_byte_index = 0;
        //  i2c_inprocess_num_bytes_expected = 1; // expect 1 byte
        //  i2c_inprocess_doit = process_setPCT_right; 
        //  i2c_waiting_parameters = 1;           // indicate that next thing coming will be a parameter
        //  break;         
        
        //case CMD_TURN_A_DISTANCE: // example for a 2-byte signed integer parameter
        //  i2c_inprocess_byte_index = 0;
        //  i2c_inprocess_num_bytes_expected = 2; // (expects 1-byte integer, signed (low,high)
        //  i2c_inprocess_doit = process_turn_distance; 
        //  i2c_waiting_parameters = 1;           // indicate that next thing coming will be a paramete
        //  break;
          
        //case CMD_ESTOP:      // all motors stop (sample for a no-parameters command)
        //  // do all 4 even if we only want 2 in case it was previously set at 4       
        //  drivetrain_estop();
        //  break;
        
      }
    }   // } else {    ( of the 
  }     // if (!comm_stream_isEmpty()) {
}       // void commands_execute_cmd_if_avail() {
