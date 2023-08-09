/*
 * Summary: openMV + esp32 based autonomous racer
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
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.  
 * 
 * Note this module implements a very simple non-robust communication protocol
 * for passing data between processors over serial path.  It is intended for primary  
 * usage in receiving lane-following data from camera.  in this case, a few random
 * missed packets are not fatal -- the next message will provide similar data a little
 * bit later.
 * 
 * All messages have a constant length and frame format.  all are preceded by a START_CHAR
 * and terminated by an END_CHAR; they all also include a checksum character just prior
 * to the END_CHAR. 
 *
 * processing starts in FSM_SEEKING_START state looking for a 
 * START_CHAR.  once it is found, it accepts the approprate number of characters, then
 * it expects to see an END_CHAR.  if the END_CHAR is not seen, then we assume that we
 * "started" the frome on a message-embedded byte that matches START_CHAR (this is called  
 * a framing error), so we discard the packet and start over at FSM_SEEKING_START.  
 * simiarly, if the checksum does not match we assume an error and discard the packet.
 * 
 * Note that the same protocol is used to send configuration commands from the main
 * processor to the camera.  In this case discarded packets would be a problem, so 
 * its best for the caller to then REQUEST a readback of the presumably sent parameter
 * to verify that it got it.  
 * 
 * Message format for camera-to-main packets (10 bytes):
 *    0   START_CHAR 
 *    1   Status  (context-depended; camera related)
 *    2,3 Parameter 1 (turn cmd (-255 to +255))  LSB, MSB
 *    4,5 Parameter 2 (measured angle (blob mode), theta (regres1 mode))  LSB, MSB
 *    6,7 Parameter 3 (rho (regres1 mode))  LSB, MSB
 *    8   checksum (LSB of sum of 1,2,3,4,5,6,7) NOT 2's complemented
 *    9   END_CHAR
 *    
 *    Note that the meaning of bytes 2-7 may be interpreted differently
 *    for some context messages
 *    
 * Message format for main-to-camera command packets (8 bytes) (enhanced simple commands mode)
 *    0       START_CHAR
 *    1       Command
 *    2,3,4,5 data (may be (2) ints, (1) float, or (4) bytes
 *    6       checksum (LSB of sum of 1,2,3,4,5) NOT 2's complemented
 *    7   END_CHAR
 *    
 *        
 * May enhance it to communicate using OpenMV RPC library (using uart, same Serial1 port; not simultaneous)   
 *    https://github.com/openmv/openmv-arduino-rpc 
 *    https://github.com/openmv/openmv-arduino-rpc/blob/master/examples/popular_features_as_the_controller_device_example/popular_features_as_the_controller_device_example.ino
 *    https://github.com/openmv/openmv-arduino-rpc/blob/master/examples/image_transfer_jpg_as_the_controller_device_example/image_transfer_jpg_as_the_controller_device_example.ino
 */

#include "cam.h"
#include "serial_com_esp32.h"
#include "mode_mgr.h"

#define CAM_MODE_UNKNOWN  0
#define CAM_MODE_IDLE     1
#define CAM_MODE_BLOB1    2
#define CAM_MODE_REGRES1  3
#define CAM_MODE_LANE_LINES 4

#define START_CHAR 0xAA
#define END_CHAR 0xA8
#define MESSAGE_LENGTH 10
#define MESSAGE_LENGTH_ENHANCED 8
#define IMG_END_CHAR '\n'

#define FSM_SEEKING_START     0
#define FSM_COLLECTING_CHARS  1
#define FSM_EXPECTING_STOP    2
#define FSM_COLLECTING_IMAGE  3

// incoming message types
#define MSG_STEERANGLE    1

#define IMG_TIMEOUT_MS 5000

byte buffer[MESSAGE_LENGTH];
int  next_buffer_index;
int  fsm_state;
int  msg_type_expected;
int  numGoodMessages, numErrorFraming, numErrorChecksum;
int  cam_mode;



#define IMAGE_BUFFER_SIZE 24000
char img_buffer[IMAGE_BUFFER_SIZE];
bool img_ready;
int img_index;
long img_timeout_ms;
/*
 * templates for private functions
 */

 /*
  * **************************************************************
  * public functions
  * **************************************************************
  */
void cam_init(void) {
  sercom1_init();  
  next_buffer_index = 0;
  fsm_state = FSM_SEEKING_START;
  msg_type_expected = MSG_STEERANGLE; 
  numGoodMessages = 0;
  numErrorFraming = 0;
  numErrorChecksum = 0;
  cam_mode = CAM_MODE_UNKNOWN;
  
  img_index = 0;
  img_ready = false;
  img_timeout_ms = 0;
  
  cam_preset_paremeters();
}

void cam_loop(void) {
  byte rcvdChar;
  int  checksumCalculated;
  int16_t steer_cmd_val, angle_error;  // params for MSG_STEERANGLE
  
  while (sercom1_available()) {
    rcvdChar = sercom1_read();  
     
    switch(fsm_state) {
      case FSM_SEEKING_START:
        if (rcvdChar == START_CHAR) {
          fsm_state = FSM_COLLECTING_CHARS;
          buffer[0] = rcvdChar;
          next_buffer_index = 1;
        }
        break;

      case FSM_COLLECTING_CHARS:
        /*
         * note START or STOP character is ok in middle of message
         * when we reach the LAST expected char, it must be a STOP char
         * if not, then we discard whole message and return to FSM_SEEKING_START
         * if it IS a STOP character, then we check the message for valid checksum
         * and parse it of checksum ok, else discard it
         * there is no ACK or NACK on messages or discarded messages, they just 
         * disappear. 
         */
        buffer[next_buffer_index++] = rcvdChar;
        if (next_buffer_index >= (MESSAGE_LENGTH - 1)) {
          // have received checksum (index 8) now expecting stop char (index 9)
          fsm_state = FSM_EXPECTING_STOP;
          next_buffer_index = (MESSAGE_LENGTH - 1);   // just to be sure
        }
        break;
        
      case FSM_EXPECTING_STOP:
        if (rcvdChar == END_CHAR) {
          /*
           * successfully received END_CHAR as expected, so buffer is complete
           * now verify checksum before accepting message
           */
          checksumCalculated = 0;
          for (int i=1; i<((MESSAGE_LENGTH - 2)); i++) {
            checksumCalculated += buffer[i];
          }
          checksumCalculated = checksumCalculated & 0xFF;
          if ( (checksumCalculated & 0xFF) != buffer[MESSAGE_LENGTH - 2]) {
            next_buffer_index = 0;
            fsm_state = FSM_SEEKING_START;
            numErrorChecksum++;            
          } else {
            if (msg_type_expected == MSG_STEERANGLE) {
              steer_cmd_val = buffer[2] | (buffer[3] << 8);
              angle_error =  buffer[4] | (buffer[5] << 8);
              // the "steer_angle" should be an integer -255 full left, +255 full right, 0=straight
              mode_got_msg_steerangle(steer_cmd_val, angle_error);
              DEBUG_PRINT("CAM GOT MESSAGE");
              DEBUG_PRINTLN(steer_cmd_val);
            } 
            
            next_buffer_index = 0;
            fsm_state = FSM_SEEKING_START; 
            numGoodMessages++;   
          }          
          
        } else {
          /*
           * if we wanted an END_CHAR and its not, then this is an error
           * so throw away this buffer and wait for a new one  
           */
          next_buffer_index = 0;
          fsm_state = FSM_SEEKING_START;
          numErrorFraming++;
        }
        break;        

      case FSM_COLLECTING_IMAGE:
        /*
         * note chars are stored in buffer until end char is reached ( ),  
         * or error condition is reached (too many chars or too much time)
         */
        if (rcvdChar == IMG_END_CHAR) {
          img_buffer[img_index] = '\0';
          img_ready = true;
          fsm_state = FSM_SEEKING_START;
          next_buffer_index = 0;
        } else {
          img_buffer[img_index++] = rcvdChar;  
          if (img_index >= (IMAGE_BUFFER_SIZE - 1)) {
            // note this is an error state -- it needs to be developed further
            fsm_state = FSM_SEEKING_START;
            img_index = (IMAGE_BUFFER_SIZE - 1);   // just to be sure
            DEBUG_PRINTLN("buffer over-run when collecting image");
          }
        }
        break;
      
    } // switch
  }   // if character available
}

/* 
 *  the "send command" function has several versions
 *  depending on the number of parameters needed
 *  
 *  they are called with same name but there are multiple
 *  prototypes and the compiler picks the correct one
 */
void cam_send_cmd(uint8_t cmd) {
  int  checksumCalculated;

  // some commands (the ones that change camera mode in a way
  // that the robot needs to care about) need to be noticed here
  switch(cmd) {
    case CAM_CMD_MODE_IDLE:
      cam_mode = CAM_MODE_IDLE;
      break;
    case CAM_CMD_MODE_BLOBS:
      cam_mode = CAM_MODE_BLOB1;
      break;
    case CAM_CMD_MODE_REGRESSION_LINE:
      cam_mode = CAM_MODE_REGRES1;
      break;
    case CAM_CMD_MODE_LANE_LINES:
      cam_mode = CAM_MODE_LANE_LINES;
      break;
    case CAM_CMD_SEND_PIC:
      fsm_state = FSM_COLLECTING_IMAGE;
      img_index = 0;
      img_ready = false;
      img_timeout_ms = millis() + IMG_TIMEOUT_MS;
      break;
  }  
  sercom1_sendchar(START_CHAR);
  sercom1_sendchar(cmd);
  checksumCalculated = cmd;   // note the START_CHAR is not included in checksum
  for (int i=0; i<4; i++) {
    sercom1_sendchar(0);
  }
  sercom1_sendchar( checksumCalculated & 0xFF );
  sercom1_sendchar(END_CHAR);
}

void cam_send_cmd(uint8_t cmd, int param1) {
  int  checksumCalculated;
  uint8_t thisByte;
  
  sercom1_sendchar(START_CHAR);
  sercom1_sendchar(cmd);
  checksumCalculated = cmd;   // note the START_CHAR is not included in checksum
  // least significant byte of integer param
  thisByte = param1 & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  // most significant byte of integer param
  thisByte = (param1 >> 8) & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  // 2 bytes of zeros for unused param
  sercom1_sendchar(0);
  sercom1_sendchar(0);
  
  sercom1_sendchar( checksumCalculated & 0xFF );
  sercom1_sendchar(END_CHAR);  
}

void cam_send_cmd(uint8_t cmd, int param1, int param2) {
  int  checksumCalculated;
  uint8_t thisByte;
  
  sercom1_sendchar(START_CHAR);
  sercom1_sendchar(cmd);
  checksumCalculated = cmd;   // note the START_CHAR is not included in checksum
  
  // least significant byte of integer_1 param
  thisByte = param1 & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  // most significant byte of integer_1 param
  thisByte = (param1 >> 8) & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  
  // least significant byte of integer_2 param
  thisByte = param2 & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  // most significant byte of integer_2 param
  thisByte = (param2 >> 8) & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  
  sercom1_sendchar( checksumCalculated & 0xFF );
  sercom1_sendchar(END_CHAR);    
}

void cam_send_cmd(uint8_t cmd, float param) {
  int  checksumCalculated;
  uint8_t thisByte;
  union {
    float floatval;
    uint8_t bytes[4];
  } myVal;
  float testy;
  
  sercom1_sendchar(START_CHAR);
  sercom1_sendchar(cmd);
  checksumCalculated = cmd;   // note the START_CHAR is not included in checksum

  myVal.floatval = param;
  
  // least significant byte of float param
  thisByte = myVal.bytes[0] & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  
  // next byte of float param
  thisByte = (myVal.bytes[1]) & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  
  // next byte of float param
  thisByte = myVal.bytes[2] & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  
  // most significant byte of float param
  thisByte = (myVal.bytes[3]) & 0xFF;
  sercom1_sendchar(thisByte);
  checksumCalculated += thisByte;
  sercom1_sendchar( checksumCalculated & 0xFF );
  sercom1_sendchar(END_CHAR);    
}

void cam_timeout_check(void) {
  if (fsm_state == FSM_COLLECTING_IMAGE) {
    if (millis() > img_timeout_ms) {
      // note this is an error state -- it needs to be developed further
      DEBUG_PRINT("timeout when collecting image "); 
      img_buffer[img_index] = '\0';
      fsm_state = FSM_SEEKING_START;
      img_index = (IMAGE_BUFFER_SIZE - 1);   // just to be sure
    }
  }
}

void cam_request_pic() {
  cam_send_cmd(CAM_CMD_SEND_PIC);
}

/*
 * https://forum.arduino.cc/t/string-arguments-to-functions-string-msg-or-string-msg/629828/2
 * https://www.tutorialspoint.com/concatenate-strings-in-arduino
 */
bool cam_append_pic(String &pagebuf) {
    //pagebuf = pagebuf + String(img_buffer);
    pagebuf.concat(img_buffer);
    img_ready = false;
    return true;
}

char* cam_return_pic() {
  img_ready = false;
  return img_buffer;
}

bool cam_check_cam_image_readiness() {
  return img_ready;
}

void cam_preset_paremeters() {  
  cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_T, config.blob_roiTweight);
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_M, config.blob_roiMweight);
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_ROI_WEIGHT_B, config.blob_roiBweight);  
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_T, config.blob_roiTloc, config.blob_roiTheight);
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_M, config.blob_roiMloc, config.blob_roiMheight);
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_ROI_POSITION_B, config.blob_roiBloc, config.blob_roiBheight);
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_SET_FLOATING_THRESH, config.blob_float_thresh);
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_SET_SEED_THRESH, config.blob_seed_thresh);
  delay(3);
  cam_send_cmd(CAM_CMD_BLOB_SET_SEED_LOC, config.blob_seed_loc);
  delay(3);
  cam_send_cmd(CAM_CMD_PID_SET_KP, config.pid_kp);
  delay(3);
  cam_send_cmd(CAM_CMD_PID_SET_KI, config.pid_ki);
  delay(3);
  cam_send_cmd(CAM_CMD_PID_SET_KD, config.pid_kd);
  delay(3);
  cam_send_cmd(CAM_CMD_PID_SET_STEERING_GAIN, config.pid_steering_gain);   
  delay(3);   
  cam_send_cmd(CAM_CMD_PID_SET_STEERING_DIRECTION, config.pid_steering_direction);
  delay(3);    
  cam_send_cmd(CAM_CMD_CAM_SET_PERSPECTIVE_FACTOR, config.cam_perspective_factor);
  delay(3);     
  cam_send_cmd(CAM_CMD_BLOB_SET_LUMI_LOW, config.blob_lumi_low);
  delay(3);    
  cam_send_cmd(CAM_CMD_BLOB_SET_LUMI_HIGH, config.blob_lumi_high);
  delay(3);   
  cam_send_cmd(CAM_CMD_HISTEQ_WANTED, config.cam_histeq_wanted);
  delay(3);   
  cam_send_cmd(CAM_CMD_NEGATE_WANTED, config.cam_negate_wanted);
  delay(3);  
  if (config.cam_perspective_wanted == 0) {
    cam_send_cmd(CAM_CMD_CAM_PERSPECTIVE_OFF);
  } else {
    cam_send_cmd(CAM_CMD_CAM_PERSPECTIVE_ON);
  }
}

void cam_enter_preferred_mode() {
  if (config.cam_startup_mode == 0) {
    cam_send_cmd(CAM_CMD_MODE_BLOBS);
  } else if (config.cam_startup_mode == 1) {
    cam_send_cmd(CAM_CMD_MODE_REGRESSION_LINE);
  } else if (config.cam_startup_mode == 2) {
    cam_send_cmd(CAM_CMD_MODE_LANE_LINES);
  } else {
    cam_send_cmd(CAM_CMD_MODE_IDLE);
  }
}
/*
 * **************************************************
 * private functions
 * **************************************************
 */
