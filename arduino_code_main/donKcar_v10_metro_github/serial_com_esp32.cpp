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
#include "serial_com_esp32.h"
#include <SoftwareSerial.h>

/*
 * *********************************************************
 * this module is currently NOT USED but is kept for 
 * future reference
 * *********************************************************
 */

/*
 * note there are 3 UARTs on ESP32-DEV
 * https://circuits4you.com/2018/12/31/esp32-hardware-serial2-example/https://circuits4you.com/2018/12/31/esp32-hardware-serial2-example/
 *  
 * There are three serial ports on the ESP-DEV known as U0UXD, U1UXD and U2UXD. * 
 * U0UXD is used to communicate with the ESP32 for programming and during reset/boot.
 * U1UXD is unused and can be used for your projects. Some boards use this port for SPI Flash access though
 * U2UXD is unused and can be used for your projects. * 
 * note this uses U2UXD on pins 16 and 17
 *  
 * BUT NOTE there are only 2 UARTs on ESP32-S2, so software serial is used for the NeoPixel interface (Serial2)
 * this reserves Serial1 (hardware channel) for the faster camera
 * 
 * https://github.com/plerup/espsoftwareserial/
 * https://github.com/plerup/espsoftwareserial
 * 
 */

bool sercom1avail = false;
bool sercom2avail = false;
 
SoftwareSerial myPort;

void sercom2_init(void) {  
  if (PIN_TX_SERCOM1 > 0) {
    sercom2avail = true;
  }  
  pinMode(PIN_TX_SERCOM2, OUTPUT);
  Serial.print("initializing sercom on pin ");
  Serial.println(PIN_TX_SERCOM2);
  
  if (sercom2avail) {
    myPort.begin(BAUD_RATE_SERCOM2, SWSERIAL_8N1, PIN_RX_SERCOM2, PIN_TX_SERCOM2, false);
    
    if (!myPort) {   // If the object did not initialize, then its configuration is invalid
      DEBUG_PRINTLN("Invalid SoftwareSerial pin configuration, check config"); 
      sercom2avail = false;
    } 
  }
}

void sercom2_sendchar(char theChar) {
  if (sercom2avail) {
    myPort.print(theChar);
  }
}

bool sercom2_available() {  
  if (sercom2avail) {
    return myPort.available();
  } else {
    return false;
  }
}

char sercom2_read() {
  if (sercom2avail) {
    return myPort.read();
  } else {
    return '\0';
  }
}


void sercom1_init(void) {
  if (PIN_TX_SERCOM1 > 0) {
    sercom1avail = true;
  }
  
  if (sercom1avail) {
    // Note the format for setting a serial port is as follows: Serial1.begin(baud-rate, protocol, RX pin, TX pin);
    Serial1.begin(BAUD_RATE_SERCOM1, SERIAL_8N1, PIN_RX_SERCOM1, PIN_TX_SERCOM1);
  }
}

void sercom1_sendchar(char theChar) {
  if (sercom1avail) {
    Serial1.print(theChar);
  }
}

bool sercom1_available() {
  if (sercom1avail) {
    return Serial1.available();
  } else {
    return false;
  }
}

char sercom1_read() {
  if (sercom1avail) {
    return Serial1.read();
  } else {
    return '\0';
  }
}
