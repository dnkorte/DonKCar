#
# PID calculation
# this module must be manually copied into the root folder of the OpenMV Cam
#

import pyb, math, time
import utility

radians_degrees = 57.3 # constant to convert from radians to degrees

seek_angle = None
old_time = None
old_error = None
p_term = None
i_term = None
d_term = None
kp = None
ki = None
kd = None
steering_direction = None
steering_gain = None
steering_center = None
max_i_term = None

# target angle is the angle the camera wants it to STEER so that it might
# bring itself into proper alignment with the lane (ultimately it would
# like the target angle to become 0 which would mean its exactly right...)
#
# the function returns the apprpriate servo angle for the servos based
# upon the PID calculation, that will help to make that happen.  note that
# the servo angle is constrained to -50 => +50 (where 0 = straight forward)
#
# it also returns the current error angle in degrees (the angle by which the
# current target angle differs from the seek angle
def update_pid(target_angle):
    global seek_angle
    global old_time, old_error
    global p_term, i_term, d_term
    global kp, ki, kd
    global steering_direction, steering_gain, steering_center
    global max_i_term

    now = pyb.millis()
    dt = now - old_time
    angle_error = seek_angle - target_angle
    de = angle_error - old_error

    p_term = kp * angle_error
    i_term += ki * angle_error
    i_term = utility.constrain(i_term, -max_i_term, max_i_term)
    d_term = (de / dt) * kd

    old_error = angle_error
    servo_angle = steering_direction * (p_term + i_term + d_term)
    servo_angle = utility.constrain(servo_angle, -50, 50)
    # print(kp, ki, kd, p_term, i_term, d_term, angle_error, servo_angle)
    # print(kp, ki, kd, int(p_term), int(i_term), int(d_term), int(angle_error), int(servo_angle))
    return angle_error, servo_angle

def initialize_pid():
    global seek_angle
    global old_time, old_error
    global p_term, i_term, d_term
    global kp, ki, kd
    global steering_direction, steering_gain, steering_center
    global max_i_term

    old_error = 0
    seek_angle = 0  # it always wants to seek STRAIGHT AHEAD
    p_term = 0
    i_term = 0
    d_term = 0
    old_time = pyb.millis()

    steering_direction = -1   # use this to reverse the steering if your car goes in the wrong direction
    steering_gain = 1.7  # calibration for your car's steering sensitivity
    steering_center = 0  # set to your car servo's center point
    max_i_term = 3

    kp = 0.8   # P gain of the PID
    ki = 0.0   # I gain of the PID
    kd = 0.4   # D gain of the PID

def set_pid_kp(newvalue):
    global kp, ki, kd
    kp = newvalue
    #print("set kp="+str(kp))

def set_pid_ki(newvalue):
    global kp, ki, kd
    ki = newvalue
    #print("set ki="+str(ki))

def set_pid_kd(newvalue):
    global kp, ki, kd
    kd = newvalue
    #print("set kd="+str(kd))

# steering gain converts desired servp angle (-50 ==> +50) to robot command value (-255 => +255)
def set_pid_steering_gain(newvalue):
    global steering_gain
    steering_gain = newvalue

def get_pid_steering_gain():
    return steering_gain

def set_pid_steering_direction(newvalue):
    # note on entry this uses web-command index values to reflect steering direction
    # index 0 ==> Normal (steering direction +1
    # index 1 ==> Reverse (steering direction -1)
    global steering_direction
    if (newvalue == 1):
        steering_direction = -1
    if (newvalue == 0):
        steering_direction = 1

def get_pid_steering_direction():
    return steering_direction
