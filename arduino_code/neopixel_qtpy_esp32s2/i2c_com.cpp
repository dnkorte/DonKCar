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
#include "i2c_com.h"

#include <Wire.h>
#include <RingBufHelpers.h>
#include <RingBufCPP.h>

/*
 * i2c interface for a "slave" device
 */

void isr_commandreceived(int);
void isr_datarequested();

/*
 * private data
 */
RingBufCPP<uint8_t, 12> commandbuffer;  // buffer has room for 12 characters
uint8_t myregisters[NUM_REGISTERS];     // array of info (device "registers")

/*
 * public methods
 */
 
void i2c_init(uint8_t addr) {
  Wire.begin(addr);                  // join i2c bus as a slave with this address
  Wire.onReceive(isr_commandreceived);  // register event processing function
  Wire.onRequest(isr_datarequested);    // register event processing function      
}

bool i2c_stream_isEmpty() {
  return(commandbuffer.isEmpty());
}

uint8_t i2c_stream_pull() {
  uint8_t thisChar;
  
  commandbuffer.pull(&thisChar);
  return(thisChar);
}

void i2c_register_set_8(int register_index, uint8_t value) {
  if ((register_index >=0) && (register_index < NUM_REGISTERS)) {
    myregisters[register_index] = value;
  }
}

void i2c_register_set_16(int register_index, int value) {
  if ((register_index >=0) && (register_index < NUM_REGISTERS-1)) {    
    // low byte first
    myregisters[register_index] = (value & 0xff);
    // high byte next
    myregisters[register_index + 1] = (value >> 8) & 0xff;
  }
}

void i2c_register_set_32(int register_index, unsigned long value) {
  if ((register_index >=0) && (register_index < NUM_REGISTERS-3)) {    
    // lowest byte first, hightest byte last
    myregisters[register_index] = (value & 0xff);
    myregisters[register_index + 1] = (value >> 8) & 0xff;
    myregisters[register_index + 2] = (value >> 16) & 0xff;
    myregisters[register_index + 3] = (value >> 24) & 0xff;
  }
}


uint8_t i2c_register_get_8(int register_index) {
  uint8_t value;  
  value = myregisters[register_index];
  return value;
}

int i2c_register_get_16(int register_index) {
  int value;  
  value = myregisters[register_index] | (myregisters[register_index+1] << 8);
  return value;
}

unsigned long i2c_register_get_32(int register_index) {
  int value;  
  value = myregisters[register_index] | (myregisters[register_index+1] << 8) | (myregisters[register_index+2] << 16) | (myregisters[register_index+3] << 24);
  return value;
}


/*
 * *********************************************************************
 * interrupt service routine to support i2c
 * 
 * function that executes whenever data (ie command) is received from master
 * this function is registered as an event          see setup() 
 * *********************************************************************
 */
void isr_commandreceived(int howMany) {
  
  int i;
  uint8_t c;
  while (Wire.available() > 0) { // loop through all characters
    c = Wire.read();             // receive byte as character
    commandbuffer.add(c);        // and store it to fifo buffer
  }
}

/*
 * *********************************************************************
 * interrupt service routine to support i2c
 * function that executes whenever request for data is received from master
 * this function is registered as an event          see setup() 
 * 
 * NOTE it ALWAYS sends 2 bytes, to avoid having to figure out 
 *      if the specified register is 1-byte or 2-bytes
 * *********************************************************************
 */
void isr_datarequested() { 
  //Wire.write(myregisters, NUM_REGISTERS);
  //Wire.write(&myregisters[REG_WHEEL_DIA], 2);
  //Wire.write(&myregisters[myregisters[REG_READREG]], 2);
}  
