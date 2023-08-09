#
# utility library
# this module must be manually copied into the root folder of the OpenMV Cam
#

import pyb, math, time
import sensor, image

perspective_factor = None     # typical range 0.10 - 0.20 for "normal" lens
wantPerspectiveCorrection = False
perspective_corrector = None
histeq_wanted = False
negate_wanted = False

def constrain(value, min, max):
    if value < min :
        return min
    if value > max :
        return max
    else:
        return value

def init_perspective_corrector():
    global perspective_factor, wantPerspectiveCorrection, perspective_corrector
    perspective_factor = 0.35
    lens_corr_factor = 1.85
    wantPerspectiveCorrection = False

def correct_perspective(img):
    global perspective_factor, wantPerspectiveCorrection, perspective_corrector
    if wantPerspectiveCorrection:
        img.rotation_corr( corners = perspective_corrector )
        #img.lens_corr(lens_corr_factor)

def set_histeq_wanted(newState):
    global histeq_wanted
    if (newState == 1):
        histeq_wanted = True
    else:
        histeq_wanted = False

def set_negate_wanted(newState):
    global negate_wanted
    if (newState == 1):
        negate_wanted = True
    else:
        negate_wanted = False

def get_histeq_wanted():
    global histeq_wanted
    return histeq_wanted

def get_negate_wanted():
    global negate_wanted
    return negate_wanted


def set_perspective_correction(newState):
    global wantPerspectiveCorrection
    wantPerspectiveCorrection = newState

def set_perspective_factor(new_factor):
    global perspective_factor, w_left, w_right, wantPerspectiveCorrection, perspective_corrector
    w = sensor.width()
    h = sensor.height()
    perspective_factor = new_factor
    w_left = (perspective_factor * w)
    w_right = w - (perspective_factor * w)
    perspective_corrector = [ [w_left, 0], [w_right, 0], [w, h], [0, h] ]


def set_cam_rez_for_blobs(sensor):
    global perspective_factor, perspective_corrector
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
    w = sensor.width()
    h = sensor.height()
    w_left = (perspective_factor * w)
    w_right = w - (perspective_factor * w)
    perspective_corrector = [ [w_left, 0], [w_right, 0], [w, h], [0, h] ]
    return

def set_cam_rez_for_grayscale(sensor):
    global perspective_factor, perspective_corrector
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
    w = sensor.width()
    h = sensor.height()
    w_left = (perspective_factor * w)
    w_right = w - (perspective_factor * w)
    perspective_corrector = [ [w_left, 0], [w_right, 0], [w, h], [0, h] ]
    return

def set_cam_rez_for_lane_lines(sensor):
    global perspective_factor, perspective_corrector
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
    w = sensor.width()
    h = sensor.height()
    w_left = (perspective_factor * w)
    w_right = w - (perspective_factor * w)
    perspective_corrector = [ [w_left, 0], [w_right, 0], [w, h], [0, h] ]
    return

def set_cam_rez_for_regression_lines(sensor):
    global perspective_factor, perspective_corrector
    # note QQQVGA yeilds approx 75 FPS, QQVGA yields approx 40 FPS
    sensor.set_pixformat(sensor.GRAYSCALE)
    #sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QQQVGA)
    #sensor.set_framesize(sensor.QQVGA)
    w = sensor.width()
    h = sensor.height()
    w_left = (perspective_factor * w)
    w_right = w - (perspective_factor * w)
    perspective_corrector = [ [w_left, 0], [w_right, 0], [w, h], [0, h] ]
    return
