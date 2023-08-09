#
# regression line tracker library
# this module must be manually copied into the root folder of the OpenMV Cam
#
# https://openmv.io/blogs/news/linear-regression-line-following
#
# explanation of theta, rho for hough transform:
#   https://docs.opencv.org/3.4/d3/de6/tutorial_js_houghlines.html
#   https://stackoverflow.com/questions/40531468/explanation-of-rho-and-theta-parameters-in-houghlines
#
# essentially points on lines in polar space are represented by rho and theta where
#   rho is the distance from the origin to the point, and theta is the angle of the
#   perpendicular line drawn to it.   so if rho is zero (0) then the line passes thru the origin
#   (i.e. you are really close to it)
#
# https://www.geeksforgeeks.org/opencv-real-time-road-lane-detection/
# https://towardsdatascience.com/line-detection-make-an-autonomous-car-see-road-lines-e3ed984952c
#
# GREAT explanation of how HOUGH TRANSFORM works starting at 40:17 and RHO, THETA at 45:40
#    in https://www.youtube.com/watch?v=eLTLtUVuuy4
#

import sensor, image, pyb, math, time


# binary view  True shows only the contrast limited view, False shows normal full range
BINARY_VIEW = False
# WHITE_LINES is True for white lines on black field, is False for black lines on white field
WHITE_LINES = True
lowest_gray = 240
GRAYSCALE_THRESHOLD = [lowest_gray, 255]
MAG_THRESHOLD = 4
THETA_GAIN = 40.0
RHO_GAIN = -1.0

line = None

def init_line_params():
    return


def line_to_theta_and_rho(line):
    if line.rho() < 0:              # ie line is ahead of us
        if line.theta() < 90:       # ie line is slanting up and to the right
            return (math.sin(math.radians(line.theta())),
                math.cos(math.radians(line.theta() + 180)) * -line.rho())
        else:
            return (math.sin(math.radians(line.theta() - 180)),
                math.cos(math.radians(line.theta() + 180)) * -line.rho())
    else:
        if line.theta() < 90:
            if line.theta() < 45:
                return (math.sin(math.radians(180 - line.theta())),
                    math.cos(math.radians(line.theta())) * line.rho())
            else:
                return (math.sin(math.radians(line.theta() - 180)),
                    math.cos(math.radians(line.theta())) * line.rho())
        else:
            return (math.sin(math.radians(180 - line.theta())),
                math.cos(math.radians(line.theta())) * line.rho())

def line_to_theta_and_rho_error(line, img):
    t, r = line_to_theta_and_rho(line)
    return (t, r - (img.width() // 2))

def lines_track(img):
    global line
    global w_correction, w_left, w_right, perspective_corrector
    global MAG_THRESHOLD, THETA_GAIN, RHO_GAIN, BINARY_VIEW, GRAYSCALE_THRESHOLD

    #if (WHITE_LINES):
    #    img = sensor.snapshot().histeq()
    #else:
    #    img = sensor.snapshot().negate().histeq()

    if BINARY_VIEW:
        img.binary([GRAYSCALE_THRESHOLD])
        img.erode(1)


    line = img.get_regression([(200, 255)], robust=True)
    #line = img.get_regression([(0, 255, 0 , 255, 0, 255)], robust=True)

    if line and (line.magnitude() >= MAG_THRESHOLD):
        # color=127 is good gray for a binary view
        #img.draw_line(line.line(), color=127, thickness=3)
        img.draw_line(line.line(), color=(200, 0, 0), thickness=5)

        t, r = line_to_theta_and_rho_error(line, img)
        #print("actual line rho:" + str(line.rho()) + " theta:" + str(line.theta()))
        new_result = (t * THETA_GAIN) + (r * RHO_GAIN)
        #print("actual line rho:" + str(line.rho()) + " theta:" + str(line.theta())+" result:"+str(new_result))
        print("error r:" + str(r) + " t:" + str(t)+" result:"+str(new_result))
        #return r, t, new_result
        return new_result





def orig_line_to_theta_and_rho(line):
    if line.rho() < 0:
        if line.theta() < 90:
            return (math.sin(math.radians(line.theta())),
                math.cos(math.radians(line.theta() + 180)) * -line.rho())
        else:
            return (math.sin(math.radians(line.theta() - 180)),
                math.cos(math.radians(line.theta() + 180)) * -line.rho())
    else:
        if line.theta() < 90:
            if line.theta() < 45:
                return (math.sin(math.radians(180 - line.theta())),
                    math.cos(math.radians(line.theta())) * line.rho())
            else:
                return (math.sin(math.radians(line.theta() - 180)),
                    math.cos(math.radians(line.theta())) * line.rho())
        else:
            return (math.sin(math.radians(180 - line.theta())),
                math.cos(math.radians(line.theta())) * line.rho())

def orig_line_to_theta_and_rho_error(line, img):
    t, r = line_to_theta_and_rho(line)
    return (t, r - (img.width() // 2))
