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
#include "motors.h"
#ifdef FLAVOR_DIFFERENTIAL

void motors_init() {  
  // Set all the motor control pins to outputs
  pinMode(PIN_MOTOR_AIN1, OUTPUT);  // right motor
  pinMode(PIN_MOTOR_AIN2, OUTPUT);  // right motor
  pinMode(PIN_MOTOR_BIN1, OUTPUT);  // left motor
  pinMode(PIN_MOTOR_BIN2, OUTPUT);  // left motor
  
  // Turn off motors - Initial state
  digitalWrite(PIN_MOTOR_AIN1, LOW);
  digitalWrite(PIN_MOTOR_AIN2, LOW);
  digitalWrite(PIN_MOTOR_BIN1, LOW);
  digitalWrite(PIN_MOTOR_BIN2, LOW);

  ledcSetup(MOTOR_CHNA, MOTOR_FREQ, MOTOR_RES); // right motor
  ledcAttachPin(PIN_MOTOR_PWMA, MOTOR_CHNA);
  
  ledcSetup(MOTOR_CHNB, MOTOR_FREQ, MOTOR_RES); // left motor
  ledcAttachPin(PIN_MOTOR_PWMB, MOTOR_CHNB);
}

void motors_stop(void) {
  digitalWrite(PIN_MOTOR_AIN1, LOW);
  digitalWrite(PIN_MOTOR_AIN2, LOW);
  digitalWrite(PIN_MOTOR_BIN1, LOW);
  digitalWrite(PIN_MOTOR_BIN2, LOW);
}


void motors_enable() {
  // nothing to do for TB6612, they are enabled when throttle is set
}

void motors_disable() {
  motors_stop();
}


/*
 * motor_driveL() drives left motor
 * @param: int throt  should be -255 (back) to +255 (fwd)
 * output  is scaled to be "up to " MAX_THROT
 */
void motor_driveL(int throt) {
  int adjusted_throt;

  if (throt > 0) {
    adjusted_throt = map(throt,1,255, 30,MAX_THROT);
    digitalWrite(PIN_MOTOR_BIN1, HIGH); // left motor
    digitalWrite(PIN_MOTOR_BIN2, LOW);  // right motor
    ledcWrite(MOTOR_CHNB, adjusted_throt);
  } else if (throt < 0) {
    adjusted_throt = map(abs(throt),1,255, 30,MAX_THROT);
    digitalWrite(PIN_MOTOR_BIN1, LOW); // left motor
    digitalWrite(PIN_MOTOR_BIN2, HIGH);  // right motor
    ledcWrite(MOTOR_CHNB, adjusted_throt);
  } else {
    adjusted_throt = 0;
    digitalWrite(PIN_MOTOR_BIN1, LOW);  // left motor
    digitalWrite(PIN_MOTOR_BIN2, LOW);  // right motor
  }  
}

/*
 * motor_driveR() drives right motor
 * @param: int throt  should be -255 (back) to +255 (fwd)
 * output  is scaled to be "up to " MAX_THROT
 */
void motor_driveR(int throt) {
  int adjusted_throt;

  if (throt > 0) {
    adjusted_throt = map(throt,1,255, 30,MAX_THROT);
    digitalWrite(PIN_MOTOR_AIN1, LOW); // left motor
    digitalWrite(PIN_MOTOR_AIN2, HIGH);  // right motor
    ledcWrite(MOTOR_CHNA, adjusted_throt);
  } else if (throt < 0) {
    adjusted_throt = map(abs(throt),1,255, 30,MAX_THROT);
    digitalWrite(PIN_MOTOR_AIN1, HIGH); // left motor
    digitalWrite(PIN_MOTOR_AIN2, LOW);  // right motor
    ledcWrite(MOTOR_CHNA, adjusted_throt);
  } else {
    adjusted_throt = 0;
    digitalWrite(PIN_MOTOR_AIN1, LOW);  // left motor
    digitalWrite(PIN_MOTOR_AIN2, LOW);  // right motor
  }  
}

void motor_ramptest() {
  uint8_t i;
  
 for (i=0; i<255; i++) {
    digitalWrite(PIN_MOTOR_AIN1, LOW);  // right motor
    digitalWrite(PIN_MOTOR_AIN2, HIGH); // right motor
    digitalWrite(PIN_MOTOR_BIN1, HIGH); // left motor
    digitalWrite(PIN_MOTOR_BIN2, LOW);  // right motor
    //Serial.print("speeding up:");
    ledcWrite(MOTOR_CHNA, i);
    ledcWrite(MOTOR_CHNB, i);
    
    //Serial.println(i);
    delay(100);
   }

 for (i=255; i>0; i--) {
    digitalWrite(PIN_MOTOR_AIN1, LOW);   // right motor
    digitalWrite(PIN_MOTOR_AIN2, HIGH);  // right motor
    digitalWrite(PIN_MOTOR_BIN1, HIGH);  // left motor
    digitalWrite(PIN_MOTOR_BIN2, LOW);   // left motor
    ledcWrite(MOTOR_CHNA, i);
    ledcWrite(MOTOR_CHNB, i);
    //Serial.print("slowing down:");
    //Serial.println(i);
    delay(100);
   }
  
  //Serial.println("Stopped");
  // Turn off motors
  ledcWrite(MOTOR_CHNA, 0);
  ledcWrite(MOTOR_CHNB, 0);
  //analogWrite(PIN_MOTOR_PWMA, 0);
  //analogWrite(PIN_MOTOR_PWMB, 0);
  //digitalWrite(PIN_MOTOR_PWMA, LOW);
  //digitalWrite(PIN_MOTOR_PWMB, LOW);
    digitalWrite(PIN_MOTOR_AIN2, LOW);
    digitalWrite(PIN_MOTOR_AIN1, LOW);
    digitalWrite(PIN_MOTOR_BIN1, LOW);
    digitalWrite(PIN_MOTOR_BIN2, LOW);
}
#endif    // FLAVOR_DIFFERENTIAL
