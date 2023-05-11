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
#include "servo.h"

/*
 * *************************************************************************************
 * NOTE: functions from this module should only be called from the following modules
 * drivetrain.cpp (for normal driving), webap_api.cpp (for configuration)
 * because (drivetrain.cpp wraps these in functions that also update the TFT display
 * *************************************************************************************
 */

/*
 *****************************************************************************************************************************
  DNK NOTE: demo for servo use on ESP32S2 -- note that must use library ESP32_S2_IS_Servo by Khoi Hoang
  for ESP32-S2 devices (ie QTPY ESP32S2), rather than ESP32_Servo (which is ok for WROOM32 devices)
  DNK NOTE: note the library accepts arguments of 0 to 180 (no negative, no 270))
 
  ESP32_S2_MultipleServos.ino
  For ESP32_S2 boards
  Written by Khoi Hoang
  
  Built by Khoi Hoang https://github.com/khoih-prog/ESP32_ISR_Servo
           https://www.arduino.cc/reference/en/libraries/esp32_s2_isr_servo/
  Licensed under MIT license
  
  The ESP32 has two timer groups, each one with two general purpose hardware timers. All the timers
  are based on 64 bits counters and 16 bit prescalers
  The timer counters can be configured to count up or down and support automatic reload and software reload
  They can also generate alarms when they reach a specific value, defined by the software.
  The value of the counter can be read by the software program.
  
  Now these new 16 ISR-based PWM servo contro uses only 1 hardware timer.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
  
  Notes:
  Special design is necessary to share data between interrupt code and the rest of your program.
  Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
  variable can not spontaneously change. Because your function may change variables while your program is using them,
  the compiler needs this hint. But volatile alone is often not enough.
  When accessing shared variables, usually interrupts must be disabled. Even with volatile,
  if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
  If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
  or the entire sequence of your code which accesses the data.
 ****************************************************************************************************************************
*/

int servoSteering, servoThrottle;

#ifdef SERVO_NATIVE_ESP32S2
#include "ESP32_S2_ISR_Servo.h"

// Select different ESP32 timer number (0-3) to avoid conflict
#define USE_ESP32_TIMER_NO          3   // orig 3

#define TIMER_INTERRUPT_DEBUG       0
#define ISR_SERVO_DEBUG             0



/*
 * ************************************************************************
 * private functions  SERVO_NATIVE_ESP32S2
 * ************************************************************************
 */ 

/*
 * the raw servo driver always accepts angle of 0-180  
 */
void set_angle(int servoIndex, uint16_t angle) {
  uint16_t myAngle;

  myAngle = angle;
  ESP32_ISR_Servos.setPosition(servoIndex, myAngle);
}

/*
 * sets pulse width; note that a 50 Hz update rate is assumed (20 mS)
 *  @param pulseWidth is duration in microseconds
 */
void set_pulsewidth(int servoIndex, uint16_t pulseWidth) {
  uint16_t myPosition;

  myPosition = pulseWidth;
  ESP32_ISR_Servos.setPulseWidth(servoIndex, myPosition);
}

/*
 * ************************************************************************
 * public functions  SERVO_NATIVE_ESP32S2
 * ************************************************************************
 */ 
void servo_init(void) {
  int temp;
  
  //Select ESP32 timer USE_ESP32_TIMER_NO
  ESP32_ISR_Servos.useTimer(USE_ESP32_TIMER_NO);  
  servoSteering = ESP32_ISR_Servos.setupServo(PIN_SERVO_STEER, SERVO_MIN_MICROS, SERVO_MAX_MICROS);
  servoThrottle = ESP32_ISR_Servos.setupServo(PIN_SERVO_THROT, SERVO_MIN_MICROS, SERVO_MAX_MICROS); 
  servo_disable_servos();
}

 void servo_enable_servos() {
  ESP32_ISR_Servos.enable(servoSteering);
  ESP32_ISR_Servos.enable(servoThrottle);
 }
 
 void servo_disable_servos() {
  ESP32_ISR_Servos.disable(servoSteering);
  ESP32_ISR_Servos.disable(servoThrottle);
 }
#endif  // SERVO_NATIVE_ESP32S2





#ifdef SERVO_PCA9685
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// our servo # counter
uint8_t servonum = 0;



void servo_init() {  
  
  servoSteering = 1;
  servoThrottle = 0;
  
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates  
  servo_disable_servos();

  delay(10);
}

 void servo_enable_servos() {
  // nothing here
 }
 
 void servo_disable_servos() {
  // nothing here
 }

/*
 * sets pulse width; note that a 50 Hz update rate is assumed (20 mS)
 *  @param pulseWidth is duration in microseconds
 */
void set_pulsewidth(int servoIndex, uint16_t microsec) {  
  pwm.writeMicroseconds(servoIndex, microsec);
}

#endif // SERVO_PCA9685

/*
 * ************************************************************************
 * public functions - regardless of which driver is used
 * ************************************************************************
 */

/*
 * @param value:  -255 full left, 0 center, +255 full right
 * (dnk note calculations confirmed, proper PW is used for angles)
 */
void servo_set_steering_value(int value) {
  int send_uS;
  float turn_fraction_of_fullrange;
  //DEBUG_PRINT("Servo angle:");
  //DEBUG_PRINTLN(angle);

  if (abs(value) < 5) {
    send_uS = config.steer_center_us;
    turn_fraction_of_fullrange = 0;
  } else if (value < 0) {
    turn_fraction_of_fullrange = abs(config.steer_center_us - config.steer_left_us) * (float) value / 255.00;
    send_uS = (float) config.steer_center_us + turn_fraction_of_fullrange;
  } else {
    turn_fraction_of_fullrange = abs(config.steer_center_us - config.steer_right_us) * (float) value / 255.00;
    send_uS = (float) config.steer_center_us + turn_fraction_of_fullrange;
  }
  set_pulsewidth(servoSteering, send_uS);
}

/*
 * this used primarily during configuration process,
 * it sets the steering pulsewidth to specified number of microseconds
 * with no error checking or scaling
 */
void servo_set_steering_uS(int send_uS) {
  set_pulsewidth(servoSteering, send_uS);
}

/*
 * @param throttle:  -255 full back, 0 stop, +255 full forward
 */
void servo_set_throttle(int throttle) {
  int send_uS;
  float throttle_fraction_of_fullrange;

  if (abs(throttle) < 10) {
    send_uS = config.esc_reverse_throttle;
  } else if (throttle < 0) {
    throttle_fraction_of_fullrange = abs(config.esc_max_throttle - config.esc_min_throttle) * (float) throttle / 255.00;
    send_uS = config.esc_reverse_throttle + (int) throttle_fraction_of_fullrange;
  } else {
    throttle_fraction_of_fullrange = abs(config.esc_max_throttle - config.esc_min_throttle) * (float) throttle / 255.00;
    send_uS = config.esc_reverse_throttle + (int) throttle_fraction_of_fullrange;
  }
  set_pulsewidth(servoThrottle, send_uS);
}

void servo_arm_throttle() {  
  set_pulsewidth(servoThrottle, config.esc_arm_signal);
  delay(config.esc_arm_time);
  set_pulsewidth(servoThrottle, config.esc_reverse_throttle);
 }
