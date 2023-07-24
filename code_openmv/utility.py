#
# utility library
# this module must be manually copied into the root folder of the OpenMV Cam
#

import pyb, math, time
import sensor, image

w_correction = None     # typical range 0.10 - 0.20 for "normal" lens
w_left = None
w_right = None
wantPerspectiveCorrection = True
perspective_corrector = None

def constrain(value, min, max):
    if value < min :
        return min
    if value > max :
        return max
    else:
        return value

def init_perspective_corrector():
    global w_correction, w_left, w_right, wantPerspectiveCorrection, perspective_corrector
    w = sensor.width()
    h = sensor.height()
    w_correction = 0.25
    lens_corr_factor = 1.85
    w_left = (w_correction * w)
    w_right = w - (w_correction * w)
    perspective_corrector = [ [w_left, 0], [w_right, 0], [w, h], [0, h] ]
    wantPerspectiveCorrection = True

def correct_perspective(img):
    global w_correction, w_left, w_right, wantPerspectiveCorrection, perspective_corrector
    if wantPerspectiveCorrection:
        img.rotation_corr( corners = perspective_corrector )
        #img.lens_corr(lens_corr_factor)

def set_perspective_correction(newState):
    global wantPerspectiveCorrection
    wantPerspectiveCorrection = newState

def set_perspective_factor(new_factor):
    global w_correction, w_left, w_right, wantPerspectiveCorrection, perspective_corrector
    w = sensor.width()
    h = sensor.height()
    w_correction = new_factor
    w_left = (w_correction * w)
    w_right = w - (w_correction * w)
    perspective_corrector = [ [w_left, 0], [w_right, 0], [w, h], [0, h] ]


def set_cam_rez_for_blobs(sensor):
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
    return

def set_cam_rez_for_grayscale(sensor):
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
    return

def set_cam_rez_for_lane_lines(sensor):
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
    return

def set_cam_rez_for_regression_lines(sensor):
    # note QQQVGA yeilds approx 75 FPS, QQVGA yields approx 40 FPS
    sensor.set_pixformat(sensor.GRAYSCALE)
    #sensor.set_framesize(sensor.QQQVGA)
    sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
