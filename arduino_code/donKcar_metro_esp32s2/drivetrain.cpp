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
#include "drivetrain.h"
#include "status.h"
#include "mode_mgr.h"
#include "util.h"

#ifdef FLAVOR_DIFFERENTIAL
#include "motors.h"
#endif

#ifdef FLAVOR_RC
#include "servo.h"
#endif
 

/*
 * **************************************************************************
 * **************************************************************************
 * **************************************************************************
 * RC MODE RACER MODE  DRIVEN BY DUAL PULSE WIDTH SERVO SIGNALS *************
 * **************************************************************************
 * **************************************************************************
 * **************************************************************************
 */
 
#ifdef FLAVOR_RC
/*
 * *************************************************************************
 * public functions (RC flavor)
 * 
 * void servo_init(void);
 * void servo_steer(int angle);
 * void servo_arm_throttle();
 * void servo_disable_servos();
 * void servo_enable_servos();
 * *************************************************************************
 */
  
void drivetrain_init() { 
  servo_init();
  status_disp_simple_msg( "Arming ESC", 'Y');
  servo_arm_throttle();  
  status_disp_clear_status_area();
}

void drivetrain_stop(void) {
  drivetrain_go(0, 0);
  status_disp_throt_value('L', "STOP", 'R');
  status_disp_throt_value('R', "STOP", 'R');
}

void drivetrain_enable() {
  servo_enable_servos();
}

void drivetrain_disable() {
  servo_disable_servos();
}

/*
 * drivetrain_go() drives both motors with independently set throttles
 * @param: int cmd_joyY, cmd_joyX  should be -255 (back) to +255 (fwd)
 */
void drivetrain_go(int cmd_joyY, int cmd_joyX) {  
  
  if (mode_motion_permitted()) { 
    servo_set_steering_value(cmd_joyX);
    servo_set_throttle(cmd_joyY);
    status_neo_show_movement_info(cmd_joyY, cmd_joyX);    // neopixel display
    status_disp_throt_value('Y', cmd_joyY, 'W');
    status_disp_throt_value('X', cmd_joyX, 'W');    
  }
}

/*
 * drivetrain_set_steering_value() sets steering angle as fraction of full travel
 * @param: int value  should be -255 (full left) to +255 (full right)
 */
void drivetrain_set_steering_value(int value) {  
  if (mode_motion_permitted()) { 
    servo_set_steering_value(value);
    status_neo_show_steering_info(value);    // neopixel display    
    status_disp_throt_value('X', value, 'W');     
  }  
}
#endif  /* FLAVOR_RC */

/*
 * **************************************************************************
 * **************************************************************************
 * **************************************************************************
 * DIFFERENTIAL MODE 2-WHEEL RACER ******************************************
 * **************************************************************************
 * **************************************************************************
 * **************************************************************************
 */
 
#ifdef FLAVOR_DIFFERENTIAL

/*
 * *************************************************************************
 * private functions for differential mode
 * *************************************************************************
 */

int motor_throtL, motor_throtR;  

/*
 * if either throttle command is out of legal range (ie > 255 or < -255)
 * this rescales both L and R to be within legal range
 */
void rescale_throttles() {   
  int excess_throt; // excessive throttle (amount by which it exceeds 255
  if (motor_throtL > 255) {
    excess_throt = motor_throtL - 255;
    motor_throtL = motor_throtL - excess_throt;
    motor_throtR = motor_throtR - excess_throt;
  }
  if (motor_throtL < -255) {
    excess_throt = motor_throtL + 255;
    motor_throtL = motor_throtL + excess_throt;
    motor_throtR = motor_throtR + excess_throt;
  }
  if (motor_throtR > 255) {
    excess_throt = motor_throtR - 255;
    motor_throtR = motor_throtR - excess_throt;
    motor_throtL = motor_throtL - excess_throt;
  }
  if (motor_throtR < -255) {
    excess_throt = motor_throtR + 255;
    motor_throtR = motor_throtR + excess_throt;
    motor_throtL = motor_throtL + excess_throt;
  }
}

/*
 * drivetrain_gostraight() drives both motors with independently set throttles
 * @param: int throtL, throtR  should be -255 (back) to +255 (fwd)
 */
void drivetrain_go_independent(int throtL, int throtR) {
  char myBuf[6];  
  
  if (mode_motion_permitted()) {
    motor_throtL = throtL;
    motor_driveL(throtL);     
    status_disp_throt_value('L', throtL, 'W');     
    
    motor_throtR = throtR;
    motor_driveR(throtR);
    status_disp_throt_value('R', throtR, 'W');
  }
}

/*
 * *************************************************************************
 * public functions (differential flavor)
 * *************************************************************************
 */
 
void drivetrain_init() { 
  motor_throtL = 0;
  motor_throtR = 0;
  motors_init();
}

void drivetrain_stop(void) {
  motors_stop();  
  motor_throtL = 0;
  motor_throtR = 0;  
  status_disp_throt_value('L', "STOP", 'R');
  status_disp_throt_value('R', "STOP", 'R');
}

void drivetrain_enable() {
  servo_enable_servos();
}

void drivetrain_disable() {
  servo_disable_servos();
}

/*
 * drivetrain_go() drives both motors with independently set throttles
 * @param: int cmd_joyY, cmd_joyX  should be -255 (back) to +255 (fwd)
 */
void drivetrain_go(int cmd_joyY, int cmd_joyX) {  
  if (mode_motion_permitted()) {  
    motor_throtL = cmd_joyY + ( (float) config.steering_fraction * (float) cmd_joyX);
    motor_throtR = cmd_joyY - ( (float) config.steering_fraction * (float) cmd_joyX);
    rescale_throttles();
    
    motor_driveL(throtL);
    status_disp_throt_value('L', throtL, 'W');
    
    motor_driveR(throtR);
    status_disp_throt_value('R', throtR, 'W');
    
    status_show_steering_info(cmd_joyY, cmd_joyX);    // neopixel display
  }
}
#endif  /* FLAVOR_DIFFERENTIAL */
