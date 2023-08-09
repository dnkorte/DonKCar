#
# lane following controller for OpenMV, as used in DonKCar racers
#
# note this is a multi-module MicroPython program.  Only the "main" module is managed directly
# by the OpenMV IDE -- the other .py modules may be edited in the OpenMV IDE but must be manually
# transferred to the camera board using file manager (don't forget to "disconnhect" the camera
# using IDE connect icon AND eject it using file manager before disconnecting wires).
#
# note also that although the "main" module (which is managed by the IDE) can be run using the triangle
# icon at bottom left of IDE, you must use "Tools/Save Open Script to OpenMV Cam (as main.py)" to
# transfer it to the camera to make it bootable at camera power up.
#

import sensor, image, pyb, math, time
from pyb import LED
from pyb import Pin, Timer
import ustruct
import ubinascii
import mypid         # this is a "local library" and must be manually stored on OpenMV cam root folder
import utility       # this is a "local library" and must be manually stored on OpenMV cam root folder
import communicator  # this is a "local library" and must be manually stored on OpenMV cam root folder
import commands      # this is a "local library" and must be manually stored on OpenMV cam root folder
import blob_tracker  # this is a "local library" and must be manually stored on OpenMV cam root folder
import color_tracker # experimental replacement for blob_Tracker
import regression_tracker  # this is a "local library" and must be manually stored on OpenMV cam root folder
#import lanelines_tracker  # this is a "local library" and must be manually stored on OpenMV cam root folder


# initialze camera
sensor.reset() # Initialize the camera sensor.
sensor.__write_reg(0x6B, 0x22)  # switches camera into advanced calibration mode. See this for more: http://forums.openmv.io/viewtopic.php?p=1358#p1358
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.set_auto_gain(True)    # do some calibration at the start
sensor.set_auto_whitebal(False)
sensor.skip_frames(time = 0)  # When you're inside, set time to 2000 to do a white balance calibration. Outside, this can be 0
#sensor.set_auto_gain(False)   # now turn off autocalibration before we start color tracking
#sensor.set_auto_whitebal(False)


utility.init_perspective_corrector()
mypid.initialize_pid()
communicator.init_communicator()
commands.init_commands(sensor)
#blob_tracker.init_blob_params()
color_tracker.init_blob_params()
regression_tracker.init_line_params()
#lanelines_tracker.init_lanelines_params()
utility.set_cam_rez_for_blobs(sensor)
commands.set_mode('I', sensor)
nextSendData = pyb.millis() + 100
nextPID = pyb.millis() + 75
nextDebug = pyb.millis() + 225
wantDebug = False

rho = 0
theta = 0

lastServoCmdVal = 0
out_buf = bytearray(10)
clock = time.clock() # Tracks FPS

#commands.set_mode('R', sensor)              #  TEMPORARY FOR DEBUGGING --------------------
#img = sensor.snapshot()                     #  TEMPORARY FOR DEBUGGING --------------------
#img = sensor.snapshot()                     #  TEMPORARY FOR DEBUGGING --------------------
#img = sensor.snapshot()                     #  TEMPORARY FOR DEBUGGING --------------------
#img = sensor.snapshot()                     #  TEMPORARY FOR DEBUGGING --------------------
#color_tracker.preset_color_thresholds(img)  #  TEMPORARY FOR DEBUGGING --------------------

while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    if utility.get_histeq_wanted():
        img = sensor.snapshot().histeq() # Take a picture and return the image. The "histeq()" function does a histogram equalization to compensate for lighting changes
    else:
        img = sensor.snapshot() # Take a picture and return the image. The "histeq()" function does a histogram equalization to compensate for lighting changes

    if utility.get_negate_wanted():
        img.negate()

    utility.correct_perspective(img)

    # note blob_tracker returns desired target angle 0=straight, -45-ish max left, +45-ish max right
    if commands.get_mode() == 'B':
        #target_angle = blob_tracker.blob_track(img)
        target_angle = color_tracker.blob_track(img)

    # note regression line_tracker (1-line) returns desired target angle 0=straight, -45-ish max left, +45-ish max right
    if commands.get_mode() == 'R':
        #r, t, target_angle = regression_tracker.lines_track(img)
        target_angle = regression_tracker.lines_track(img)


    # note lane lines_tracker (1-line) returns desired target angle 0=straight, -45-ish max left, +45-ish max right
    # if commands.get_mode() == 'L':
    #     target_angle = lanelines_tracker.lanelines_track(img)

    if (commands.get_mode() == 'G') or (commands.get_mode() == 'I') :
        target_angle = 0
        servo_angle = 0
        angle_error = 0

    # pid calculator expects target angle 0=straight, -45-ish max left, +45-ish max right
    # it returns angles in degrees per same model (0 is straight; -50 full left, +50 full right (constrained)))
    now = pyb.millis()
    if  (now > nextPID):  # time has passed since last measurement; do the PID at 50hz
        nextPID = now + 20
        if (commands.get_mode() == 'B'):
            angle_error, servo_angle = mypid.update_pid(target_angle)


    if (now > nextSendData) and (commands.get_mode() == 'B') and (commands.get_send_driving_info()):
        nextSendData = now + 100
        notused = 0
        checksum = 0
        # note PID function returns number between -50 and +50, so we scale it by 5 to get +/- 250 for servo command
        #servo_cmd_val = utility.constrain( (mypid.get_pid_steering_gain() * mypid.get_pid_steering_direction() * servo_angle), -255, 255)
        servo_cmd_val = utility.constrain( (5 * mypid.get_pid_steering_direction() * servo_angle), -255, 255)
        #if (abs(servo_cmd_val - lastServoCmdVal) > 2):
        #if (int(servo_cmd_val) != int(lastServoCmdVal)):
        if True:
            out_buf = ustruct.pack("<bbhhhbb", 0xAA, 1, int(servo_cmd_val), int(angle_error), int(target_angle), checksum, 0xA8)
            for i in range(1, 8, 1):
                checksum += out_buf[i]
            checksum = checksum & 0xFF
            out_buf = ustruct.pack("<bbhhhbb", 0xAA, 1, int(servo_cmd_val), int(angle_error), int(target_angle), checksum, 0xA8)
            communicator.write_buf(out_buf)

            lastServoCmdVal = servo_cmd_val

            if (wantDebug):
                print("sent message:", str(int(servo_cmd_val)), str(int(target_angle)))

    communicator.check_for_commands(sensor, img)

    if (now > nextDebug) and (wantDebug):
        nextDebug = now + 1000
        #print( "target angle:" + str(target_angle) + " r:" + str(r) + " t:" + str(t))
