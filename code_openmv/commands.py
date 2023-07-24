#
# commands library
# this module must be manually copied into the root folder of the OpenMV Cam
#

import pyb, math, time
import ustruct
import ubinascii
import communicator  # this is a "local library" and must be manually stored on OpenMV cam root folder
import blob_tracker  # this is a "local library" and must be manually stored on OpenMV cam root folder
import regression_tracker  # this is a "local library" and must be manually stored on OpenMV cam root folder
import utility       # this is a "local library" and must be manually stored on OpenMV cam root folder
import mypid        # this is a "local library" and must be manually stored on OpenMV cam root folder

# constants for commands module
CMD_MODE_IDLE = 1
CMD_MODE_BLOBS = 2
CMD_MODE_REGRESSION_LINE = 3
CMD_SEND_PIC = 4
CMD_MODE_GRAYSCALE = 5
CMD_DRIVE_ON = 6
CMD_DRIVE_OFF = 7

CMD_BLOB_SET_SEED_LOC = 8
CMD_BLOB_SET_FLOATING_THRESH = 9
CMD_BLOB_SET_SEED_THRESH = 10
CMD_BLOB_ROI_WEIGHT_T = 11
CMD_BLOB_ROI_WEIGHT_M = 12
CMD_BLOB_ROI_WEIGHT_B = 13
CMD_BLOB_ROI_POSITION_T = 14
CMD_BLOB_ROI_POSITION_M = 15
CMD_BLOB_ROI_POSITION_B = 16
CMD_PID_SET_KP = 17
CMD_PID_SET_KI = 18
CMD_PID_SET_KD = 19
CMD_PID_SET_STEERING_GAIN = 20
CMD_PID_SET_STEERING_DIRECTION = 21
CMD_CAM_SET_PERSPECTIVE = 22
CMD_CAM_PERSPECTIVE_ON = 23
CMD_CAM_PERSPECTIVE_OFF = 24
CMD_MODE_LANE_LINES = 25

red_led = pyb.LED(1)
green_led = pyb.LED(2)
blue_led = pyb.LED(3)
ir_leds = pyb.LED(4)

send_driving_info = None    # true to send periodic steering commands to main (from PID)
mode = None

def init_commands(sensor):
    set_mode('I', sensor)
    set_send_driving_info(False)

def process_cmd(cmd_char, sensor, img):
    if cmd_char == CMD_MODE_IDLE:          # blue LED
        set_mode('I', sensor)
    elif cmd_char == CMD_MODE_BLOBS:       # yellow LED for BLOBS mode
        set_mode('B', sensor)
    elif cmd_char == CMD_MODE_REGRESSION_LINE:       # green LED for REGRESSION LINE (1 line) mode
        set_mode('R', sensor)
    elif cmd_char == CMD_MODE_LANE_LINES:       # red LED for LANE LINES mode
        set_mode('L', sensor)
    elif cmd_char == CMD_MODE_GRAYSCALE:   # white LED for GRAYSCALE mode
        set_mode('G', sensor)
    elif cmd_char == CMD_SEND_PIC:
        # start = pyb.millis()
        base64_data = ubinascii.b2a_base64(img.compress(quality=90))
        communicator.write_buf(base64_data)
        # end = pyb.millis()
    elif cmd_char == CMD_DRIVE_ON:
        set_send_driving_info(True)
    elif cmd_char == CMD_DRIVE_OFF:
        set_send_driving_info(False)
    elif cmd_char == CMD_BLOB_SET_SEED_LOC:
        blob_tracker.adjust_seed_loc_y(communicator.get_int_param1())
    elif cmd_char == CMD_BLOB_SET_FLOATING_THRESH:
        blob_tracker.adjust_floating_threshold(communicator.get_float_param())
    elif cmd_char == CMD_BLOB_SET_SEED_THRESH:
        blob_tracker.adjust_seed_threshold(communicator.get_float_param())
    elif cmd_char == CMD_BLOB_ROI_WEIGHT_T:
        blob_tracker.adjust_roi_weight(0, communicator.get_float_param())
    elif cmd_char == CMD_BLOB_ROI_WEIGHT_M:
        blob_tracker.adjust_roi_weight(1, communicator.get_float_param())
    elif cmd_char == CMD_BLOB_ROI_WEIGHT_B:
        blob_tracker.adjust_roi_weight(2, communicator.get_float_param())
    elif cmd_char == CMD_BLOB_ROI_POSITION_T:
        blob_tracker.adjust_roi_position(0, communicator.get_int_param1(), communicator.get_int_param2())
    elif cmd_char == CMD_BLOB_ROI_POSITION_M:
        blob_tracker.adjust_roi_position(1, communicator.get_int_param1(), communicator.get_int_param2())
    elif cmd_char == CMD_BLOB_ROI_POSITION_B:
        blob_tracker.adjust_roi_position(2, communicator.get_int_param1(), communicator.get_int_param2())
    elif cmd_char == CMD_PID_SET_KP:
        mypid.set_pid_kp(communicator.get_float_param())
    elif cmd_char == CMD_PID_SET_KI:
        mypid.set_pid_ki(communicator.get_float_param())
    elif cmd_char == CMD_PID_SET_KD:
        mypid.set_pid_kd(communicator.get_float_param())
    elif cmd_char == CMD_PID_SET_STEERING_GAIN:
        mypid.set_pid_steering_gain(communicator.get_float_param())
    elif cmd_char == CMD_PID_SET_STEERING_DIRECTION:
        mypid.set_pid_steering_direction(communicator.get_int_param1())
    elif cmd_char == CMD_CAM_SET_PERSPECTIVE:
        utility.set_perspective_factor(communicator.get_float_param())
    elif cmd_char == CMD_CAM_PERSPECTIVE_ON:
        utility.set_perspective_correction(True)
    elif cmd_char == CMD_CAM_PERSPECTIVE_OFF:
        utility.set_perspective_correction(False)

# modes may be 'I' (idle), 'B' (blobs), 'L' (lines), 'G' (grayscale)
def set_mode(newmode, sensor):
    global mode
    if newmode == 'I':
        mode = newmode
        set_LED_blue()
        utility.set_cam_rez_for_grayscale(sensor)
    elif newmode == 'B':
        mode = newmode
        set_LED_yellow()
        utility.set_cam_rez_for_blobs(sensor)
    elif newmode == 'R':
        mode = newmode
        set_LED_green()
        utility.set_cam_rez_for_regression_lines(sensor)
    elif newmode == 'L':
        mode = newmode
        set_LED_red()
        utility.set_cam_rez_for_lane_lines(sensor)
    elif newmode == 'G':
        mode = newmode
        set_LED_white()
        utility.set_cam_rez_for_grayscale(sensor)

def get_mode():
    global mode
    return mode

def set_LED_blue():
    red_led.off()
    green_led.off()
    blue_led.on()

def set_LED_red():
    red_led.on()
    green_led.off()
    blue_led.off()

def set_LED_green():
    red_led.off()
    green_led.on()
    blue_led.off()

def set_LED_yellow():
    red_led.on()
    green_led.on()
    blue_led.off()

def set_LED_white():
    red_led.on()
    green_led.on()
    blue_led.on()

def set_LED_off():
    red_led.off()
    green_led.off()
    blue_led.off()

def set_send_driving_info(newstate):
    global send_driving_info
    send_driving_info = newstate

def get_send_driving_info():
    global send_driving_info
    return send_driving_info
