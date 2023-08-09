/*
 * Module: nunchuk.scad
 * this main module generates a box to hold components of a belt-clip portable
 * box into which a user can plug a Wii Nunchuk controller which can then be
 * used to control an RC Car or robot. 
 *
 * Project: DonKCar
 * Author(s): Don Korte
 * github: https://github.com/dnkorte/DonKCar.git
 * 
 *  
 * MIT License
 * 
 * Copyright (c) 2023 Don Korte
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
 * SOFTWARE.
 */

part_to_print="box with 1200 mAH";  // [ "box with 1200 mAH", "box with 500 mAH", "lid without hanger", "lid for Nunchuk cradle", "lid for Nunchuk hanger", "Nunchuk Cradle plate", "Cap for Nunchuk Cradle", "Nunchuk Hanger plate", "beltclip", "lanyard clip", "battbox_lid 500", "battbox_lid 1200", "test" ]

draw_part();

module draw_part() {
    $fn = 36;
    

    if (part_to_print == "beltclip") {
        part_beltclip();
    } else if (part_to_print == "lanyard clip") {
        part_lanyard_clip();
    } else if (part_to_print == "battbox_lid 500") {
        component_battbox_lipo_500_flat_lid();
    } else if (part_to_print == "battbox_lid 1200") {
        component_battbox_lipo_1200_flat_lid();
    } else if (part_to_print == "box with 1200 mAH") {
        box_feather("1200");
    } else if (part_to_print == "box with 500 mAH") {
        box_feather("500");
    } else if (part_to_print == "lid without hanger") {
        lid_feather("N");
    } else if (part_to_print == "lid for Nunchuk hanger") {
        lid_feather("H");
    } else if (part_to_print == "lid for Nunchuk cradle") {
        lid_feather("C");
    } else if (part_to_print == "Nunchuk Cradle plate") {
        nunchuk_cradle_plate();
    } else if (part_to_print == "Cap for Nunchuk Cradle") {
        nunchuk_cradle_cap();
    } else if (part_to_print == "Nunchuk Hanger plate") {
        nunchuk_hanger_plate();
    } else if (part_to_print == "test") {
        translate([ 0, 0, 2 ]) nunchuk_hanger();
        roundedbox(50, 50, 3, 2);
    }
}

box_length = 80;
box_width = 120;
box_height = 37;
box_corner_radius = 8;

feather_box_length = 80;
feather_box_width = 85;
feather_plate_width = 120;
feather_box_height = 35;
hanger_mount_sep = 42;
cradle_mount_sep = 62;

plate_thick = 3;
wall_thick = 5;
wall_height = 16;
corner_radius = 8;

plate_x = 80;
plate_y = 120;

beltclip_hole_sep_x = 20;
beltclip_hole_sep_y = 15;

mountplate_thick = 2;

/*
 ************************************************************************************************
 * this includes all of the needed library modules
 ************************************************************************************************
 */
include <../../lib_robo/config_core.scad>;
include <../../lib_robo/config_box.scad>
include <../../lib_robo/components_core.scad>;
include <../../lib_robo/components_boards.scad>;
include <../../lib_robo/components_switches_and_lights.scad>;
include <../../lib_robo/components_power_batteries.scad>;
include <../../lib_robo/components_power_boards.scad>;
include <../../lib_robo/components_power_fuses_connectors.scad>;
//include <../../lib_robo/components_camera_rollbar.scad>;
include <../../lib_robo/simple_box_and_lid.scad>
include <../../lib_robo/components_robo_misc.scad>; 

/*
 * *****************************************************************
 * mainl box-building code for drawing parts
 * most standard components are already handled here, by means
 * of configuration parameters, however custom parts may be
 * added by adding appropriate code to these modules
 * *****************************************************************
 */

module box_feather(batteryFlavor = "1200") {
    nunchuk_window_wide = 27;   // was 27
    nunchuk_window_tall = 20;   // should raise it 5 above box bottom (was 22)

    usb_window_wide = 14;
    usb_window_tall = 8;
    usb_window_height = 15;

    difference() {
        union() {
            // the basic hollow box
            difference() {
                roundedbox(feather_box_length, feather_box_width, box_corner_radius, box_height);
                translate([0, 0, body_bottom_thickness])  
                    roundedbox((feather_box_length - 2*body_wall_thickness), 
                           (feather_box_width - 2*body_wall_thickness), 
                           box_corner_inner_radius, 
                           (box_height - body_wall_thickness+1));
            }

            /*
             ************************************************************************
             * here enter all the parts that are added to the box
             ************************************************************************
             */
            translate([  0, -(feather_plate_width-feather_box_width)/2, 0 ]) roundedbox( box_length, feather_plate_width, box_corner_radius, plate_thick);

            if (batteryFlavor == "1200") {
                translate([ -10, 5, plate_thick ]) rotate([ 0, 0, 0 ]) component_battbox_lipo_1200_flat("adds");
            } else {
                translate([ -10, 0, plate_thick ]) rotate([ 0, 0, 0 ]) component_battbox_lipo_500_flat("adds");
            }

            translate([ (plate_x/2)-13, 20, plate_thick-1 ]) rotate([ 0, 0, 90 ]) part_nunchuk();
            
            translate([ (plate_x/2)-1, 20, 1 ]) rotate([ 90, 90, 90 ]) rotate([ 0, 0, 90 ]) 
                linear_extrude(2) text("Notch", size=6,  halign="center", font = "Liberation Sans:style=Bold");  

            translate([ 0, -(feather_box_length/2)-1.5, plate_thick+15 ]) rotate([ 90, 180, 0 ]) component_reverse_feather(mode="adds", mount_height=7);

        }

        // create mounting holes along y side (by removal...)
        translate([-(box_length/2)-0.1, 0, (box_height - mount_center_offset_from_boxtop)]) rotate([0,90,0])  
              cylinder(h=box_length+0.2, r=screwhole_radius_M30_passthru);
      
        // create mounting holes along x side
        translate([0, -(box_width/2)-0.1, (box_height - mount_center_offset_from_boxtop)]) rotate([0,90,90])  
              cylinder(h=box_width+0.2, r=screwhole_radius_M30_passthru);

        /*
         ************************************************************************
         * here enter all the parts that are removed from the box
         ************************************************************************
         */
        translate([ 0, -63, -0.1 ]) component_beltclip_mount();


        if (batteryFlavor == "1200") {
            translate([ -10, 5, plate_thick ]) rotate([ 0, 0, 0 ]) component_battbox_lipo_1200_flat("holes");
        } else {
            translate([ -10, 0, plate_thick ]) rotate([ 0, 0, 0 ]) component_battbox_lipo_500_flat("holes");
        }

        translate([ (box_length/2)+0.1, -20, box_height/2 ]) rotate([ 90, 180, 0 ]) rotate([ 0, 90, 180 ]) component_mini_toggle_switch("holes");

        translate([ 0, -(feather_box_length/2)+0, plate_thick+15 ]) rotate([ 90, 0, 0 ]) roundedbox( 30, 16, 3, 6);
        translate([ 0, -(feather_box_length/2)-1.5, plate_thick+15 ]) rotate([ 90, 180, 0 ]) component_reverse_feather(mode="holes", mount_height=7);

        // access hole for nunchuk connector
        translate([ (plate_x/2)+0.1, 20, 4.2 + plate_thick + (nunchuk_window_tall/2) ]) rotate([ 0, -90, 0 ]) 
            roundedbox( nunchuk_window_tall, nunchuk_window_wide, 2, body_wall_thickness+0.2); 

    }
}


module lid_feather(want_plate = "C") {   
    difference() {

        union() {
            roundedbox(feather_box_length, feather_box_width, box_corner_radius, lid_thickness);
            translate([0, 0, lid_thickness]) lip(feather_box_length, feather_box_width);  

            /*
             ************************************************************************
             * here enter all the parts that are added to the lid
             ************************************************************************
             */
            if (want_plate == "H") {
                translate([ -hanger_mount_sep/2, -(hanger_mount_sep/2)+8, 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
                translate([ -hanger_mount_sep/2, (hanger_mount_sep/2)+8, 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
                translate([ hanger_mount_sep/2, -(hanger_mount_sep/2)+8, 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
                translate([ hanger_mount_sep/2, (hanger_mount_sep/2)+8, 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
            }
            if (want_plate == "C") {
                translate([ -cradle_mount_sep/2, -(cradle_mount_sep/2), 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
                translate([ -cradle_mount_sep/2, (cradle_mount_sep/2), 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
                translate([ cradle_mount_sep/2, -(cradle_mount_sep/2), 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
                translate([ cradle_mount_sep/2, (cradle_mount_sep/2), 2 ]) cylinder(d=TI30_mount_diameter, h=TI30_default_height);
            }
        }
        
        /*
         ************************************************************************
         * here enter all the parts that are removed from the lid
         ************************************************************************
         */
        if (want_plate == "H") {
            translate([ -hanger_mount_sep/2, -(hanger_mount_sep/2)+8, -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
            translate([ -hanger_mount_sep/2, (hanger_mount_sep/2)+8, -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
            translate([ hanger_mount_sep/2, -(hanger_mount_sep/2)+8, -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
            translate([ hanger_mount_sep/2, (hanger_mount_sep/2)+8, -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
        }
        if (want_plate == "C") {
            translate([ -cradle_mount_sep/2, -(cradle_mount_sep/2), -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
            translate([ -cradle_mount_sep/2, (cradle_mount_sep/2), -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
            translate([ cradle_mount_sep/2, -(cradle_mount_sep/2), -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
            translate([ cradle_mount_sep/2, (cradle_mount_sep/2), -0.1 ]) cylinder(d=TI30_through_hole_diameter, h=TI30_default_height+2.2);
        }
    }
}


module part_nunchuk() {
    nunchuk_plate_x = 28;
    nunchuk_plate_y = 20;
    nunchuk_mount_sep_x = 20;
    nunchuk_mount_sep_y = 13;

    roundedbox( nunchuk_plate_x, nunchuk_plate_y, 1, mountplate_thick+2 );

    translate([ -nunchuk_mount_sep_x/2, -nunchuk_mount_sep_y/2, mountplate_thick+2 ]) TI25_mount();
    translate([ -nunchuk_mount_sep_x/2, nunchuk_mount_sep_y/2, mountplate_thick+2 ]) TI25_mount();
    translate([ nunchuk_mount_sep_x/2, -nunchuk_mount_sep_y/2, mountplate_thick+2 ]) TI25_mount();
    translate([ nunchuk_mount_sep_x/2, nunchuk_mount_sep_y/2, mountplate_thick+2 ]) TI25_mount();

    translate([ 0, -3, mountplate_thick+2 ])  
        linear_extrude(1) text("NC", size=6, halign="center");
}

module component_switches() {
    difference() {
        translate([ -15, -10, 0 ]) cube([ 50, 24, 5]);
        rotate([ 0, 0, 90 ]) translate([ -1, 0, -0.1 ]) component_mini_toggle_switch("holes");      // power toggle
        translate([ 22, -1, -0.1 ]) cylinder(d=10.5, h=5.2);                           // pushbutton
    }
}


module nunchuk_hanger_plate() {
    mounting_plate_size = 50;

    difference() {
        roundedbox(mounting_plate_size, mounting_plate_size, 3, 2);
        translate([ -hanger_mount_sep/2, -hanger_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
        translate([ -hanger_mount_sep/2, hanger_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
        translate([ hanger_mount_sep/2, -hanger_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
        translate([ hanger_mount_sep/2, hanger_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
    }
    translate([ 0, 0, 2 ]) nunchuk_hanger();
}



module nunchuk_cradle_plate() {
    mounting_plate_size = 75; // was 70

    difference() {
        roundedbox(mounting_plate_size, mounting_plate_size, 3, 2);
        translate([ -cradle_mount_sep/2, -cradle_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
        translate([ -cradle_mount_sep/2, cradle_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
        translate([ cradle_mount_sep/2, -cradle_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
        translate([ cradle_mount_sep/2, cradle_mount_sep/2, -0.1 ]) cylinder(d=4, h=2.2);
    }
    // translate([ 14, -(mounting_plate_size/2) + 15, 2 ]) nunchuk_cradle();
    //translate([ 16, -(mounting_plate_size/2) + 21, 2 ]) nunchuk_cradle();
    translate([ 16, -(mounting_plate_size/2) + 29, 2 ]) rotate([ 0, 0, 7 ]) nunchuk_cradle();
}




