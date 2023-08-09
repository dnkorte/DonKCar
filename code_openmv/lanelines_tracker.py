#
# commands library
# this module must be manually copied into the root folder of the OpenMV Cam
#

import sensor, image, pyb, math, time

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green things. You may wish to tune them...
# old thresholds = [(30, 100, 15, 127, 15, 127), # generic_red_thresholds
#              (30, 100, -64, -8, -32, 32), # generic_green_thresholds
#              (0, 15, 0, 40, -80, -20)] # generic_blue_thresholds
#
# note that since we flood-fill the image prior to finding blobs, the color values
# are pretty much "fixed" and it is unlikely that it would ever be necessary
# to change these values...
#
color_tracking_thresholds = (0, 100, -30, 127,   60, 80)   # BEST threshold for filled yellow

# You may pass up to 16 thresholds above. However, it's not really possible to segment any
# scene with 16 thresholds before color thresholds start to overlap heavily.

# Each roi is (x, y, w, h). The line detection algorithm will try to find the
# centroid of the largest blob in each roi. The x position of the centroids
# will then be averaged with different weights where the most weight is assigned
# to the roi near the bottom of the image and less to the next roi and so on.
ROIS = [ # [ROI, weight]
        [ 0, 30,  160, 39, 1.5 ],
        [ 0, 70,  160, 19, 0.35 ],
        [ 0, 90, 160, 20, 0.20 ]
       ]


# Compute the weight divisor (we're computing this so you don't have to make weights add to 1).
weight_sum = None

seed_threshold=0.8
floating_threshold=0.3
seed_loc_y = 100
top_of_interesting_area_x = 30

# All line objects have a `theta()` method to get their rotation angle in degrees.
# You can filter lines based on their rotation angle.

min_degree = 0
max_degree = 179

def init_lanelines_params():
    global seed_threshold, floating_threshold, seed_loc_y
    global ROIS, weight_sum
    global w_correction, w_left, w_right, perspective_corrector
    global top_of_interesting_area_x

    # parameters for flood fill
    seed_threshold = 0.8
    floating_threshold = 0.3
    seed_loc_y = 100
    top_of_interesting_area_x = 50

    weight_sum = 0
    for r in ROIS: weight_sum += r[4] # r[4] is the roi weight.


def lanelines_track(img):
    global color_tracking_thresholds
    global seed_threshold, floating_threshold, seed_loc_y, top_of_interesting_area_x

    global ROIS, weight_sum
    global w_correction, w_left, w_right, perspective_corrector
    global min_degree, max_degree


    # seed_threshold controls the maximum allowed difference between
    # the initial pixel and any filled pixels. It's important to
    # set this such that flood fill doesn't fill the whole image.

    # floating_threshold controls the maximum allowed difference
    # between any two pixels. This can easily fill the whole image
    # with even a very low threshold.
    # flood_fill will fill pixels that both thresholds.

    # cover up everything in image above the top of highest ROI (so flood fill doesn't leak thru it)

    for l in img.find_lines([0,top_of_interesting_area_x,160, (160-top_of_interesting_area_x)], threshold = 700, theta_margin = 25, rho_margin = 25):
        if (min_degree <= l.theta()) and (l.theta() <= max_degree):
            #img.draw_line(l.line(), color = (255, 0, 0), thickness=5)
            img.draw_line(l.line(), color = (255, 255, 255), thickness=5)
            # print(l)

    img.draw_line( 0, top_of_interesting_area_x, 159, top_of_interesting_area_x, color = (255, 255, 255), thickness=5)
    #img.flood_fill( int(img.width()/2), 100, 0.8, 0.3, color=(200, 200, 0), clear_background=True)

    #img.flood_fill( int(img.width()/2), seed_loc_y, seed_threshold, floating_threshold, color=(200, 200, 0), clear_background=True)
    centroid_sum = 0
    for r in ROIS:
        blobs = img.find_blobs([color_tracking_thresholds], roi=r[0:4], merge=True) # r[0:4] is roi tuple.
        if blobs:
            # Find the index of the blob with the most pixels.
            most_pixels = 0
            largest_blob = 0
            for i in range(len(blobs)):
                if blobs[i].pixels() > most_pixels:
                    most_pixels = blobs[i].pixels()
                    largest_blob = i

            # Draw a rect around the blob.
            img.draw_rectangle(blobs[largest_blob].rect())
            img.draw_cross(blobs[largest_blob].cx(),
                           blobs[largest_blob].cy())

            img.draw_cross(80, 100, color=( 0, 0, 200))

            centroid_sum += blobs[largest_blob].cx() * r[4] # r[4] is the roi weight.

    center_pos = (centroid_sum / weight_sum) # Determine center of line.
    img.draw_circle(int(center_pos), 4, 4, color=( 0, 200, 0))
    #img.draw_circle(int(img.width()/2), int(img.height()-4), 4, color=( 0, 200, 0), fill=True)
    img.draw_line(int(img.width()/2), int(img.height()), int(center_pos), 0, color=(200, 0, 0), thickness=3)

    # Convert the center_pos to a deflection angle. We're using a non-linear
    # operation so that the response gets stronger the farther off the line we
    # are. Non-linear operations are good to use on the output of algorithms
    # like this to cause a response "trigger".

    # The 80 is from half the X res, the 60 is from half the Y res. The
    # equation below is just computing the angle of a triangle where the
    # opposite side of the triangle is the deviation of the center position
    # from the center and the adjacent side is half the Y res. This limits
    # the angle output to around -45 to 45. (It's not quite -45 and 45).
    if (abs(center_pos - (img.width()/2)) < 4):
        deflection_angle = 0
    else:
        deflection_angle = math.atan( (center_pos-(img.width()/2)) / img.height() )

    # Convert angle in radians to degrees, and reverse its sign to normalize it to racer orientation
    deflection_angle = -math.degrees(deflection_angle)

    return deflection_angle

"""
 this adjusts values of ROIs.  note that the OpenMV documentation suggests that the ROIs should
 be Tuples (actually here a list of tuples), but it appears that they can be lists too.

 params for this function
    which_roi :     0 = top, 1 = middle, 2 = bottom
    new_y :         0 = top of screen, 119 = bottom of screen (this is y coord of TOP of ROI)
    new_height :     height in pixels.
"""

def adjust_roi_position(which_roi, new_y, new_height):
    global ROIS, top_of_interesting_area_x
    if (which_roi < 0) or (which_roi > 2):
        return
    if (new_y < 0) or (new_y > 119) or (new_height < 0) or (new_height > 119):
        return
    ROIS[which_roi][1] = new_y
    ROIS[which_roi][3] = new_height
    if (which_roi == 0):
        top_of_interesting_area_x = new_y + 1


"""
 this adjusts weights of ROIs.  note that the OpenMV documentation suggests that the ROIs should
 be Tuples (actually here a list of tuples), but it appears that they can be lists too.

 params for this function
    which_roi :     0 = top, 1 = middle, 2 = bottom
    new_weight :    0 - 1.0 (float)
"""

def adjust_roi_weight(which_roi, new_weight):
    global ROIS, weight_sum
    if (which_roi < 0) or (which_roi > 2):
        return
    if (new_weight < 0.0):
        return
    ROIS[which_roi][4] = new_weight
    weight_sum = 0.0
    for r in ROIS:
        weight_sum += r[4]   # r[4] is the roi weight.


def adjust_seed_threshold(new_value):
    global seed_threshold, floating_threshold

    seed_threshold = new_value

def adjust_floating_threshold(new_value):
    global seed_threshold, floating_threshold

    floating_threshold = new_value


def adjust_seed_loc_y(new_value):
    global seed_loc_y
    if (new_value < 0) or (new_value > 119):
        return
    seed_loc_y = new_value
