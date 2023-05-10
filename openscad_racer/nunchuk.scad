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

part_to_print="backplate";  // [ "box", "lid", "backplate", "backplate AA", "beltclip", "battbox_lid", "test" ]

draw_part();

module draw_part() {
    $fn = 36;
    
    if (part_to_print == "backplate") {
        part_backplate();
    } else if (part_to_print == "backplate AA") {
        part_backplate_AA();
    } else if (part_to_print == "beltclip") {
        part_beltclip();
    } else if (part_to_print == "battbox_lid") {
        component_battbox_lipo_500_flat_lid();
    } else if (part_to_print == "box") {
        box();
    } else if (part_to_print == "lid") {
        lid();
    } else if (part_to_print == "test") {
        //difference() {
        //    roundedbox(50, 30, 2, 2.4);
        //    part_128x32_oled_window("holes");
        //}
        //part_128x32_oled_backer();
        part_oled_bezel();
    }
}

box_length = 80;
box_width = 120;
box_height = 35;
box_corner_radius = 8;

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

module box() {
    nunchuk_window_wide = 27;   // was 27
    nunchuk_window_tall = 20;   // should raise it 5 above box bottom (was 22)

    usb_window_wide = 14;
    usb_window_tall = 8;
    usb_window_height = 15;

    difference() {
        union() {
            // the basic hollow box
            difference() {
                roundedbox(box_length, box_width, box_corner_radius, box_height);
                translate([0, 0, body_bottom_thickness])  
                    roundedbox((box_length - 2*body_wall_thickness), 
                           (box_width - 2*body_wall_thickness), 
                           box_corner_inner_radius, 
                           (box_height - body_wall_thickness+1));
            }

            /*
             ************************************************************************
             * here enter all the parts that are added to the box
             ************************************************************************
             */
            translate([ -12, 34, plate_thick ]) rotate([ 0, 0, 90 ]) component_battbox_lipo_500_flat("adds");

            translate([ (plate_x/2)-13, -35, plate_thick-1 ]) rotate([ 0, 0, 90 ]) part_nunchuk();
            
            translate([ (plate_x/2)-1, -34, 1 ]) rotate([ 90, 90, 90 ]) rotate([ 0, 0, 90 ]) 
                linear_extrude(2) text("Notch", size=6,  halign="center", font = "Liberation Sans:style=Bold");  

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
        translate([ 0, -36, -0.1 ]) component_beltclip_mount();
        translate([ -12, 34, plate_thick ]) rotate([ 0, 0, 90 ]) component_battbox_lipo_500_flat("holes");
        //translate([ (box_width/2)+0.1, 30, box_height/2 ]) rotate([ 0, 90, 180 ]) component_mini_toggle_switch("holes");

        // access hole for nunchuk connector
        translate([ (plate_x/2)+0.1, -35, 4.2 + plate_thick + (nunchuk_window_tall/2) ]) rotate([ 0, -90, 0 ]) 
            roundedbox( nunchuk_window_tall, nunchuk_window_wide, 2, body_wall_thickness+0.2); 

        // access hole for usb charge/programming connector
        translate([ (plate_x/2)+0.1, 0, usb_window_height ]) rotate([ 0, -90, 0 ]) 
            roundedbox( usb_window_tall, usb_window_wide, 2, body_wall_thickness+0.2); 
    }
}

/*
 * uses a 12mm pushbutton, like https://www.amazon.com/Waterproof-Momentary-Button-Switch-Colors/dp/B07F24Y1TB
 * and "boat" rocker  switch https://www.amazon.com/5Pcs-Rocker-Switch-Position-QTEATAK/dp/B07Y1GDRQG 
 */

module lid() {   
    difference() {

        union() {
            roundedbox(box_length, box_width, box_corner_radius, lid_thickness);
            translate([0, 0, lid_thickness]) lip();  

            /*
             ************************************************************************
             * here enter all the parts that are added to the lid
             ************************************************************************
             */
            translate([ -7, 0, plate_thick ]) component_smallmint_protoboard("adds");
        }
        
        /*
         ************************************************************************
         * here enter all the parts that are removed from the lid
         ************************************************************************
         */
        translate([ -7, 0, plate_thick ]) component_smallmint_protoboard("holes");
        translate([ -10, -(box_width/2)+25, 0 ])  component_oled_bezel("holes");    // make a hole for bezel to fit in
        translate([ -20, 35, -0.1 ]) component_12mm_button("holes");
        translate([  15, 35, -0.1 ])  component_boat_rocker_switch("holes");
    }

    // add on bezel for OLED display (must be added AFTER its big acceptance hole)
    translate([ -10, -(box_width/2)+25, 0 ])  component_oled_bezel("adds");
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

module part_128x32_oled() {
    oled_plate_x = 35;
    oled_plate_y = 23;
    oled_mount_sep_x = 28;
    oled_mount_sep_y = 16.5;

    roundedbox( oled_plate_x, oled_plate_y, 1, mountplate_thick );

    translate([ -oled_mount_sep_x/2, -oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();
    translate([ -oled_mount_sep_x/2, oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();
    translate([ oled_mount_sep_x/2, -oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();
    translate([ oled_mount_sep_x/2, oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();

    translate([ 0, -3, mountplate_thick ])  
        linear_extrude(1) text("OLED", size=6, halign="center");
}


module part_128x32_oled_backer() {
    oled_plate_x = 55;
    oled_plate_y = 23;
    oled_mount_sep_x = 28;
    oled_mount_sep_y = 16.5;

    roundedbox( oled_plate_x, oled_plate_y, 1, mountplate_thick );

    translate([ -oled_mount_sep_x/2, -oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();
    translate([ -oled_mount_sep_x/2, oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();
    translate([ oled_mount_sep_x/2, -oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();
    translate([ oled_mount_sep_x/2, oled_mount_sep_y/2, mountplate_thick ]) TI25_mount();

    translate([ 0, -3, mountplate_thick ])  
        linear_extrude(1) text("OLED", size=6, halign="center");
}

module part_128x32_oled_window(mode="holes") {
    oled_mount_sep_x = 28;
    oled_mount_sep_y = 16.5;
    window_wide = 33;
    window_tall = 13.5;

    if (mode == "holes") {
        translate([ -oled_mount_sep_x/2, -oled_mount_sep_y/2, -0.1 ]) cylinder( r=screwhole_radius_M25_passthru, h=lid_thickness+0.2);
        translate([ -oled_mount_sep_x/2, oled_mount_sep_y/2, -0.1 ]) cylinder( r=screwhole_radius_M25_passthru, h=lid_thickness+0.2);
        translate([ oled_mount_sep_x/2, -oled_mount_sep_y/2, -0.1 ]) cylinder( r=screwhole_radius_M25_passthru, h=lid_thickness+0.2);
        translate([ oled_mount_sep_x/2, oled_mount_sep_y/2, -0.1 ]) cylinder( r=screwhole_radius_M25_passthru, h=lid_thickness+0.2);
        translate([ -window_wide/2, -window_tall/2, -0.1 ]) cube([ window_wide, window_tall, lid_thickness+0.2 ]);
    }

}

module component_powerswitch() {
    difference() {
        translate([ -15, -10, 0 ]) cube([ 30, 20, 5]);
        rotate([ 0, 0, 90 ]) translate([ -3, 0, -0.1 ]) component_mini_toggle_switch("holes");
    }
}

module component_switches() {
    difference() {
        translate([ -15, -10, 0 ]) cube([ 50, 24, 5]);
        rotate([ 0, 0, 90 ]) translate([ -1, 0, -0.1 ]) component_mini_toggle_switch("holes");      // power toggle
        translate([ 22, -1, -0.1 ]) cylinder(d=10.5, h=5.2);                           // pushbutton
    }
}

module part_backplate() {
    difference() {
        union() {
            difference() {
                union() {
                    roundedbox( plate_x, plate_y, corner_radius, plate_thick );
                    translate([ 0, 0, plate_thick ]) component_smallmint_protoboard("adds");
                    translate([ -12, 39, plate_thick ]) rotate([ 0, 0, 90 ]) component_battbox_lipo_500_flat("adds");
                }
                translate([ 0, 0, plate_thick ]) component_smallmint_protoboard("holes");
                translate([ -12, 39, plate_thick ]) rotate([ 0, 0, 90 ]) component_battbox_lipo_500_flat("holes");
            }
            translate([ (plate_x/2)-14, -44, plate_thick-1 ]) rotate([ 0, 0, 90 ]) part_nunchuk();
            //translate([ (-plate_x/2)+14, -44, plate_thick-1 ]) rotate([ 0, 0, 90 ]) part_nunchuk();
            translate([ -15, -42, plate_thick-1 ]) rotate([ 0, 0, 0 ]) part_128x32_oled();

            translate([ (plate_x/2), 38, 10 ]) rotate([ -90, 0, 90 ]) component_powerswitch();
        }
        translate([ 0, -42, -0.1 ]) component_beltclip_mount();
    }
}


plate_AA_x = 85;
plate_AA_y = 140;
plate_AA_battmount_sep = 30;

module part_backplate_AA() {
    difference() {
        union() {
            difference() {
                union() {
                    roundedbox( plate_AA_x, plate_AA_y, corner_radius, plate_thick );
                    translate([ 0, -10, plate_thick ]) component_smallmint_protoboard("adds");
                }
                translate([ 0, -10, plate_thick ]) component_smallmint_protoboard("holes");

                // holes for mounting batt -- must use M2 panhead bolt (so head is small enough to not interfere with batt)
                translate([ -12, 44-(plate_AA_battmount_sep/2), -0.1 ]) cylinder( d=TI20_through_hole_diameter, h=5.2);
                translate([ -12, 44+(plate_AA_battmount_sep/2), -0.1 ]) cylinder( d=TI20_through_hole_diameter, h=5.2);
            }
            translate([ (plate_AA_x/2)-14, -50, plate_thick-1 ]) rotate([ 0, 0, 90 ]) part_nunchuk();
            translate([ (plate_AA_x/2)-4, -10, plate_thick ])
                rotate([ 0, 0, 90 ]) linear_extrude(1) text("<Notch Down", size=6, halign="center");

            translate([ -15, -52, plate_thick -1 ]) rotate([ 0, 0, 0 ]) part_128x32_oled();

            translate([ (plate_AA_x/2), 30, 16 ]) rotate([ -90, 0, 90 ]) component_switches();
        }
        translate([ 0, -52, -0.1 ]) component_beltclip_mount();
    }
    //translate([ (-58/2)-12, 20, 4 ]) color([ 1, 0, 0 ]) cube([ 58, 48, 15 ]);
}



oled_bezel_mount_sep = 42;  
oled_mount_sep_x = 28;
oled_mount_sep_y = 16.5;
oled_window1_wide = 33;          // the "bigger" window that oled sits "inside" (on layer1)
oled_window1_tall = 13.5;
oled_window2_wide = 30;          // the "smaller" window that covers a little bit of the window (on layer2) (was 30)
oled_window2_tall = 10.5;

module component_oled_bezel(mode="holes") {  
    plate_x = 45;
    plate_y = 25;
    plate1_thick = 1.5;         // first layer of bezel, oled glass fits inside its window
    plate2_thick = 3;           // the cover for bezel; just covers up a little bit of the ugly part of oled


    if (mode == "adds") {
        translate([ 0, 0, plate1_thick + plate2_thick ]) rotate([ 180, 0, 0 ]) difference() {   
            union() {
                roundedbox(plate_x, plate_y, 2, plate1_thick);
                translate([ 0, 0, plate1_thick ]) roundedbox(plate_x, plate_y, 2, plate2_thick);
            }

            translate([ -oled_mount_sep_x/2, -oled_mount_sep_y/2, -0.1 ]) cylinder( d=M25_selftap_dia, h=plate1_thick+plate2_thick+0.2);
            translate([ -oled_mount_sep_x/2, oled_mount_sep_y/2, -0.1 ]) cylinder( d=M25_selftap_dia, h=plate1_thick+plate2_thick+0.2);
            translate([ oled_mount_sep_x/2, -oled_mount_sep_y/2, -0.1 ]) cylinder( d=M25_selftap_dia, h=plate1_thick+plate2_thick+0.2);
            translate([ oled_mount_sep_x/2, oled_mount_sep_y/2, -0.1 ]) cylinder( d=M25_selftap_dia, h=plate1_thick+plate2_thick+0.2);

            translate([ -oled_window1_wide/2, -oled_window1_tall/2,-0.1 ]) cube([ oled_window1_wide, oled_window1_tall, plate1_thick+0.1 ]);
            translate([ -(oled_window2_wide/2), -oled_window2_tall/2, -0.1 ]) cube([ oled_window2_wide, oled_window2_tall, plate1_thick+plate2_thick+0.2 ]);

            // inset to allow room for wires
            translate([ 0, -(oled_window1_tall/2), -0.2 ]) roundedbox( 20, 8, 2, plate1_thick+0.2);
        }
    }
    if (mode == "holes") {
        translate([ -(oled_window1_wide+6)/2, -(oled_window1_tall+6)/2,-0.1 ]) cube([ oled_window1_wide+6, oled_window1_tall+6, plate1_thick+plate2_thick+0.2 ]);
    }
}

module component_oled_bezel_mount_hole(mode="holes", thick=8) {
    if (mode == "holes") {
        // bezel mount screws
        translate([ (oled_bezel_mount_sep/2), 0, -0.1 ]) cylinder( d=M3_throughhole_dia, h=plate1_thick+plate2_thick+0.2);
        translate([ -(oled_bezel_mount_sep/2), 0, -0.1 ]) cylinder( d=M3_throughhole_dia, h=plate1_thick+plate2_thick+0.2);

        // clearance hole for oled pcb
        translate([ 0, 0, -0.1 ]) roundedbox( 38, 25, 2, thick+0.2);
    }
    if (mode == "adds") {
        // pillars for mounting screws
    }
}

