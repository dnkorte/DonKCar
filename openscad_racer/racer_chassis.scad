/*
 * Module: racer_chassis.scad
 * this main module generates 3d-printable mounting plates and 
 * components for a variety of purchasable RC Car chassis platforms. 
 * these chassis hold cameras and electronics to support Autonomous Racer
 * projects.  Use of the OpenSCAD "customizer" allows the user to simply
 * select appropriate parts to build various configurations of racer --
 * including built-in options compatible with DonkeyCar Raspberry Pi 3b,
 * DonkeyCar Jetson Nano, and a custom DonKCar platform based on the OpenMV
 * camera running in conjunction with ESP32 based communication and main control.
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

part_to_print="Show Assembly";  // [ "Top Plate", "Top Plate (front half)", "Top Plate (back half)", "Show Assembly", "RPi Cam Pillar (no handle)", "RPi Cam Pillar (with handle)", "OpenMV Cam Pillar (no handle)", "OpenMV Cam Pillar (with handle)", "Back Arch", "Back Arch w/TFT", "NeoPixel Bezel 3", "Handle", "Mount Pillar (car-to-plate; TT02 single-post design)", "Power Hub", "Power Hub with ServoConverter", "Fuse Holder Lid", "Battery Box Lid", "Thumbnut 12", "Thumbnut 15", "Thumbnut 18", "Thumbnut 18 tall", "Thumbnut 18 super",  "test" ]

chassis = 0; // [0:TT02-2, 1:TT02-2 Extended, 2:TT02-4, 3:Holyton, 4:Hosim, 5:Exceed, 6:Custom ]

baseconfig = "metro"; // ["metro", "Donkey RPi3b", "Donkey Jetson Nano"]
aux_battery = "none"; // ["none", "anker6700", "urgenex2000", "NiMH 7.2v 2200 mAH" ]

wantLightStrip = "Yes"; // [ "Yes", "No" ]

frontRightBoard = "None"; // ["SmallMint", "None"]
frontLeftBoard = "None"; // ["SmallMint",  "None"]

cameraDownAngle = 10;	// [ 5, 10, 12, 15, 17, 20 ]
cameraPillarAccessory = "OpenMV Cam"; // [ "OpenMV Cam", "RPi Cam", "SmallMint Proto", "None"]
arch_mount_width = 120; // [ 88, 120, 135 ]

tt02mountbracket_height = 40; // [ 35, 40, 45, 50 ]

pwrhub_side = "right"; // ["right", "left"]
switch = "Toggle Switch (notch up)"; // [ "Toggle Switch (notch up)", "Toggle Switch (notch down)", "Boat Rocker Switch"]

wantLogo = "split"; // [ "arch", "split", "none" ]
// note if you change carident you must CLICK out of box; the ENTER key does not work
carIdent = "racer3";


/*
 *********************************************************************
 * define a dummy function here, to stop the configurator "fields phase"
 *********************************************************************
 */

module dummy() {
	// this doesn't do anything useful
}

/*
 *********************************************************************
 * define other parameters that don't want configurator fields
 *********************************************************************
 */

// note for all dimensions on plate, the center of the body mount pins is the origin (0,0) point
// and "forward" is (+), "backward" is (-);  left is (-), right is (+)

// note indices are:  0/TT02-2,      1/TT02-2 ext, 2/TT02-4,   3/Holyton,   4/Hosim,   5/Exceed,  6/Custom

pillars_fb_sep =      [ 224,         224,         260,         253,         194,       261,			253 ];
// left/right separation may be different at front end than at back, so here show [front sep, back sep]
pillars_lr_sep =      [ [60,60],     [60,60],     [65,65],     [73,83],     [54,54],    [80,80],	    [73,83] ];
pillars_dia = 		  [ 3.5,         3.5,		  7.2,         5.5,         5.5,        7.2, 		7.2 ];
pillars_max_depth =	  [ 4,           4,           4,           3,           1.8,          4,          4 ];

added_fronts =        [ 10,          10,          10,          10,          65,         10,			10 ];
added_backs =         [ 10,          30,          10,          10,          10,         10, 		10 ];
plate_widths =        [ 170,         170,         170,         170,         170,        170, 		170 ];

// ligthstrip locations (POSITIVE but as distance behind origin), 2nd parameter is "n" or "r"
// "n" indicates normal mounting-tab-forward orientation, "r" indicates mounting-tab-backward orientation
lightStrip_locs =     [ [101,"r"],    [130,"n"],   [108,"r"],  [110,"n"],   [78,"r"],   [112,"n"], 	[110,"n"] ];

// for these features, params are x, y
metro_locs =          [ [0,-42],     [0,-50],     [0,-40],     [0,-38],     [0,-22],    [0,-44],    [0,-38] ];
rpi3b_locs =          [ [0,-42],     [0,-50],     [0,-40],     [0,-39],     [0,-22],    [0,-44],    [0,-38] ];
jetson_locs =         [ [0,-52],     [0,-55],     [0,-46],     [0,-47],     [0,-28],    [0,-48],    [0,-47] ];
frontL_locs = 		  [ [-58, 92],   [-58, 92],   [-60, 100],  [-60, 100],  [-60, 124],  [-62, 100], [-60, 101] ];
frontR_locs =         [ [58, 92],    [58, 92],    [60, 100],   [60, 100],   [60, 124],   [62, 100],  [60, 101] ];

// for servo locs, params are x, y, z-rotation
servo_locs =          [ [58, 23],    [58, 21],    [58, 31],    [58, 28],    [58, 50],   [58, 32],   [58, 28] ];

// for aux batteries, params are x, y, z-rotation
auxb_ank67_info =     [ [35,47,180], [35,47,180], [35,52,180], [35,54,180], [35,76,180], [35,60,180],  [35,54,180]  ];
auxb_urge_info =      [ [-55,2,0],   [-55,2,0],   [-55,12,0],  [-55,9,0],   [-55,32,0],   [-55,12,0],  [-55,9,0]  ];
auxb_nimh2200_info  = [ [-64,-4,0],  [-64,-6,0],  [-64,4,0],   [-64,3,0],   [-64,25,0],   [-64,5,0],   [-64,3,0]  ];

// note wirehole_locs indicate right-side hole, but there is also a left-side hole that mirrors is
wirehole_locs =       [ [60,-38],    [60,-42],    [60,-36],    [60,-36],    [60,-12],   [60,-42],   [60,-36] ];

// location where top plate is split for 3d printing convenience
// 0 means exact middle of plate, + is number of mm FORWARD of middle, - is mm BEHIND middle
split_locs = 		  [ -10,        -13,         -3,          -5,          16,         -4,  	   -5 ];
joiner_centers = 	  [ 0,          -0,           3,           2,          8,           4, 		    2 ];

// for camera / arch info, paramters are (y location of camera plate center), (length of handle)
// note that the arch center is towards the back of the chassis, which is why the y location is (negative)
cam_to_arch_info = 	  [ [82, 107],  [80, 126],    [90, 125],  [90, 127],   [138, 140], [93, 132],   [90, 127] ];

cam_to_arch_separation = cam_to_arch_info[ chassis][1];
cam_arch_loc_y = cam_to_arch_info[ chassis][0];

camera_center_y = cam_to_arch_info[chassis][0];
handle_length = cam_to_arch_info[chassis][1];
arch_center_y = camera_center_y - handle_length - 50;

pwrhub_mount_sep = 120;
pwrhub_mount_inset = 8;

plate_y = pillars_fb_sep[chassis] + added_fronts[chassis] + added_backs[chassis];
plate_x = plate_widths[chassis];	
plate_thick = 4;
corner_radius = 14;
					
front_edge = (plate_y/2);
back_edge = -(plate_y/2);
left_edge = -(plate_x/2);
right_edge = (plate_x/2);

/*
 ************************************************************************************************
 * this includes all of the needed library modules
 ************************************************************************************************
 */
include <../../lib_robo/config_core.scad>;
include <../../lib_robo/components_core.scad>;
include <../../lib_robo/components_boards.scad>;
include <../../lib_robo/components_switches_and_lights.scad>;
include <../../lib_robo/components_power_batteries.scad>;
include <../../lib_robo/components_power_boards.scad>;
include <../../lib_robo/components_power_fuses_connectors.scad>;
include <../../lib_robo/components_camera_rollbar.scad>;
include <../../lib_robo/components_robo_misc.scad>; 

draw_part();

module draw_part() {
    $fn = 24; 

    if (part_to_print == "Top Plate") {
    	part_top_plate();
    } else if (part_to_print == "Top Plate (front half)") {
    	plate_front_half(split_locs[chassis]);
    } else if (part_to_print == "Top Plate (back half)") {
    	plate_back_half(split_locs[chassis]);
    } else if (part_to_print == "NeoPixel Bezel 3") {
    	part_3neopixel_strip_bezel();
    } else if (part_to_print == "RPi Cam Pillar (no handle)") {
        part_rpi_pillar();
    } else if (part_to_print == "RPi Cam Pillar (with handle)") {
        part_rpi_pillar(true);
    } else if (part_to_print == "OpenMV Cam Pillar (no handle)") {
        part_openmv_pillar();
    } else if (part_to_print == "OpenMV Cam Pillar (with handle)") {
        part_openmv_pillar(true);
    } else if (part_to_print == "Back Arch") {
        part_back_arch();
    } else if (part_to_print == "Back Arch w/TFT") {
        part_back_arch(true);
    } else if (part_to_print == "Handle") {
    	part_handle(handle_length);
    } else if (part_to_print == "Battery Box Lid") {
    	draw_battbox_lid("pins");
    } else if (part_to_print == "Power Hub") {
    	part_hanging_power_hub();
    } else if (part_to_print == "Power Hub with ServoConverter") {
    	part_hanging_power_hub_servo();
    } else if (part_to_print == "Fuse Holder Lid") {
    	fuse_holder_lid();
    } else if (part_to_print == "Thumbnut 12") {
    	part_thumbnut(12);
    } else if (part_to_print == "Thumbnut 15") {
    	part_thumbnut(15);
    } else if (part_to_print == "Thumbnut 18") {
    	part_thumbnut(18, 4);
    } else if (part_to_print == "Thumbnut 18 tall") {
    	part_thumbnut(18, 4, hub_thickness=7);
    } else if (part_to_print == "Thumbnut 18 super") {
    	part_thumbnut(18, 4, 9);
    } else if (part_to_print == "Mount Pillar (car-to-plate; TT02 single-post design)") {
    	part_tt02mountbracket_pillar();
    } else if (part_to_print == "Show Assembly") {
    	show_assembly();
    } else if (part_to_print == "test") {
    	//part_hanging_power_hub();
    	//component_arduino("adds");
    	//part_back_arch();
		cd1 = 10;
		cd2 = 15;
    	difference() {
    		roundedbox(40, 40, 3, 4);
			translate([ -10, 0, 2 ]) cylinder(d1=cd1, d2=cd2, h=2.1);
			translate([  10, 0, 3 ]) cylinder(d1=cd1, d2=cd2, h=1.1);

			translate([ -10, 0, -0.1 ]) cylinder(d=6, h=5); // was 3.5
			translate([  10, 0, -0.1 ]) cylinder(d=6, h=5); // was 3.5
    	}
    }
}

module show_assembly() {
	part_top_plate();
	translate([ 0, camera_center_y, plate_thick ]) color([ 1, 0, 0 ])  part_openmv_pillar(true);
	translate([ 0, arch_center_y+12.5, plate_thick+arch_leg_length ]) color([ 1, 0, 0 ]) rotate([ 90, 0, 0 ]) part_back_arch(true);
	translate([ 0, camera_center_y - 37.5, plate_thick+125 ]) color([ 0, 0, 1 ])  rotate([ 0, 0, 180 ]) part_handle(handle_length);

	if (wantLightStrip == "Yes") {
		if (lightStrip_locs[chassis][1] == "r") {
			translate([ 0, -lightStrip_locs[chassis][0], plate_thick+5.5 ]) color([ 0.8, 0.8, 0.8])  rotate([ 180, 0, 180 ]) part_3neopixel_strip_bezel();
		} else {
			translate([ 0, -lightStrip_locs[chassis][0], plate_thick+5.5 ]) color([ 0.8, 0.8, 0.8])  rotate([ 180, 0, 0 ]) part_3neopixel_strip_bezel();
		}
	}

	if (pwrhub_side == "right") {
		translate([ right_edge, 0, -pwr_panel_tall ]) rotate([ 0, 0, 90 ]) color([ 0.4, 0.4, 0.4 ]) part_hanging_power_hub();
	}
	if (pwrhub_side == "left") {
		translate([ left_edge, 0, -pwr_panel_tall ]) rotate([ 0, 0, -90 ]) color([ 0.4, 0.4, 0.4 ]) part_hanging_power_hub();
	}
	if ( (chassis == 0) || (chassis == 1) ) {
		translate([ 0, (pillars_fb_sep[chassis]/2)-(tt02mountbracket_thick/2), -tt02mountbracket_height ]) 
			rotate([ 90, 0, 180 ]) 
			color([ 0.6, 0.6, 1 ]) 
			part_tt02mountbracket_pillar();
		translate([ 0, -(pillars_fb_sep[chassis]/2)+(tt02mountbracket_thick/2), -tt02mountbracket_height ]) 
			rotate([ 90, 0, 0 ]) 
			color([ 0.6, 0.6, 1 ]) 
			part_tt02mountbracket_pillar();
	} else {
		// front mount pillars
		translate([ pillars_lr_sep[chassis][0]/2, pillars_fb_sep[chassis]/2, -30 ]) draw_body_mount_pin();
		translate([ -pillars_lr_sep[chassis][0]/2, pillars_fb_sep[chassis]/2, -30 ]) draw_body_mount_pin();
		// back mount pillars may have different L/R separation
		translate([ pillars_lr_sep[chassis][1]/2, -pillars_fb_sep[chassis]/2, -30 ]) rotate([ 0, 0, 180 ]) draw_body_mount_pin();
		translate([ -pillars_lr_sep[chassis][1]/2, -pillars_fb_sep[chassis]/2, -30 ]) rotate([ 0, 0, 180 ]) draw_body_mount_pin();
	}

	// draw phantom line where split would happen
	translate([ 0, split_locs[chassis]-0.5, plate_thick+2 ]) color([ 0.2, 1, 0.2, 0.2 ]) roundedbox( plate_widths[chassis] - 30, 1, 0.5, 3 );

	// show phantom joiner tubes
	translate([ -(plate_x/2), -50+joiner_centers[chassis], plate_thick ]) color([ 0.2, 1, 0.2, 0.3 ]) plate_joiner_tube(90);
	translate([ (plate_x/2) - 8, -50+joiner_centers[chassis], plate_thick ]) color([ 0.2, 1, 0.2, 0.3 ]) plate_joiner_tube(90);
}
 

module part_top_plate() {
    $fn = 36;

	difference() {
		union() {
			translate([ 0, (added_fronts[chassis]-added_backs[chassis])/2, 0 ]) roundedbox( plate_x, plate_y, corner_radius, plate_thick );	

			if (baseconfig == "metro") {
				translate([ metro_locs[ chassis][0], metro_locs[ chassis][1], plate_thick ]) 
					rotate([ 0, 0, 180 ])  
					color([ 0.8, 0.7, 0 ]) 
					component_arduino("adds"); // general electronics			
			}
			if (baseconfig == "Donkey RPi3b") {
				translate([ rpi3b_locs[ chassis][0], rpi3b_locs[ chassis][1], plate_thick ]) 
					rotate([ 0, 0, 180 ]) 
					color([ 0.8, 0.7, 0 ]) 
					component_pi3("adds");
				translate([ servo_locs[ chassis][0], servo_locs[ chassis][1], plate_thick ]) 
					rotate([ 0, 0, 90 ]) 
					color([ 0.8, 0.7, 0 ]) 
					component_ada_servo_controller("adds");
			}
			if (baseconfig == "Donkey Jetson Nano") { 
				translate([ jetson_locs[ chassis][0], jetson_locs[ chassis][1], plate_thick ]) 
					rotate([ 0, 0, 180 ]) 
					color([ 0.8, 0.7, 0 ]) 
					component_jetson_nano(mode="adds");
				translate([ servo_locs[ chassis][0], servo_locs[ chassis][1], plate_thick ]) 
					rotate([ 0, 0, 90 ]) 
					color([ 0.8, 0.7, 0 ]) 
					component_ada_servo_controller("adds");
			}

			if (aux_battery == "anker6700") {
				translate([ auxb_ank67_info[chassis][0], auxb_ank67_info[chassis][1], plate_thick-3 ]) 
					rotate([ 0, 0, auxb_ank67_info[chassis][2] ]) 	
					color([ 0.8, 0.7, 0 ])
					draw_battbox_anker6700("pins");
			}
			if (aux_battery == "urgenex2000") {
				translate([ auxb_urge_info[chassis][0], auxb_urge_info[chassis][1], plate_thick-3 ]) 
					rotate([ 0, 0, auxb_urge_info[chassis][2] ]) 
				 	color([ 0.8, 0.7, 0 ])
					draw_battbox_urgenex2000("pins");
			}			
			if (aux_battery == "NiMH 7.2v 2200 mAH") {
				translate([ auxb_nimh2200_info[chassis][0], auxb_nimh2200_info[chassis][1], plate_thick-3 ]) 
					rotate([ 0, 0, auxb_nimh2200_info[chassis][2] ]) 
					color([ 0.8, 0.7, 0 ])
					draw_battbox_nimh2200_72("pins");
			}

			if (frontLeftBoard == "SmallMint") {
				translate([ frontL_locs[ chassis][0], frontL_locs[ chassis][1], plate_thick ]) 
					color([ 0.8, 0.7, 0 ])
					rotate([ 0, 0, 90 ]) 
					component_smallmint_protoboard("adds"); // for neopixel driver
			}
			if (frontRightBoard == "SmallMint") {
				translate([ frontR_locs[ chassis][0], frontR_locs[ chassis][1], plate_thick ]) 
					rotate([ 0, 0, 90 ]) 
					color([ 0.8, 0.7, 0 ])
					component_smallmint_protoboard("adds");  
			}

			if (wantLogo == "arch") {
				translate([ 0, arch_center_y-4, plate_thick ])  
			        linear_extrude(1) text("DonKCar", size=10, halign="center");
				translate([ 0, arch_center_y-14, plate_thick ])  
			        linear_extrude(1) text(carIdent, size=8, halign="center");
			}

			if (wantLogo == "split") {
				translate([ 0, (pillars_fb_sep[chassis]/2)-4, plate_thick ])  
			        linear_extrude(1) text("DonKCar", size=8, halign="center");
			    if (len(carIdent) > 11) {    
					translate([ 0, -(pillars_fb_sep[chassis]/2)-4, plate_thick ])  
			        	linear_extrude(1) text(carIdent, , halign="center");
			    } else {  
					translate([ 0, -(pillars_fb_sep[chassis]/2)-4, plate_thick ])  
			        	linear_extrude(1) text(carIdent, size=10, halign="center");
			    }
			}

		}

		if (baseconfig == "metro") {
			translate([ metro_locs[ chassis][0], metro_locs[ chassis][1], plate_thick ]) 
				rotate([ 0, 0, 180 ]) 
				component_arduino("holes"); 	
		}
		if (baseconfig == "Donkey RPi3b") {
			translate([ rpi3b_locs[ chassis][0], rpi3b_locs[ chassis][1], plate_thick ]) 
				rotate([ 0, 0, 180 ]) 
				component_pi3("holes");
			translate([ servo_locs[ chassis][0], servo_locs[ chassis][1], plate_thick ]) 
				rotate([ 0, 0, 90 ]) 
				component_ada_servo_controller("holes");
		}
		if (baseconfig == "Donkey Jetson Nano") {
			translate([ jetson_locs[ chassis][0], jetson_locs[ chassis][1], plate_thick ])
				rotate([ 0, 0, 180 ]) 
				component_jetson_nano(mode="holes");
			translate([ servo_locs[ chassis][0], servo_locs[ chassis][1], plate_thick ]) 
				rotate([ 0, 0, 90 ]) 
				component_ada_servo_controller("holes");
		}

		// wirehole for battery (note USB Power Packs do not get wireholes, because they don't use
		// the switch on the hanging power hub)
			if (aux_battery == "urgenex2000") {
				translate([ auxb_urge_info[chassis][0]+84, auxb_urge_info[chassis][1]+6, -0.1 ]) 
					roundedbox( 12, 12, 2, plate_thick+0.2);
			}			
			if (aux_battery == "NiMH 7.2v 2200 mAH") {
				translate([ auxb_nimh2200_info[chassis][0]+100, auxb_nimh2200_info[chassis][1]+8, -0.1 ]) 
					roundedbox( 12, 12, 2, plate_thick+0.2);
			}

		// camera pillar mount
        translate([ -32, camera_center_y-16, -0.1 ]) cylinder(d=TI30_through_hole_diameter, 7.2 );
        translate([ -32, camera_center_y+16, -0.1 ]) cylinder(d=TI30_through_hole_diameter, 7.2 );
        translate([  32, camera_center_y-16, -0.1 ]) cylinder(d=TI30_through_hole_diameter, 7.2 );
        translate([  32, camera_center_y+16, -0.1 ]) cylinder(d=TI30_through_hole_diameter, 7.2 );

		// holes for mounting back arch to chassis plate
		// note these dimensions depend on dims in the camera_piller.scad file
		// but for current arch they are 15 mm apart, and are +/- 44 mm from chassis centerline
		// (this assumes arch with 25mm "thickness" and outer/inner dia of 96 / 80 mm (48 / 40 mm radius))
		// (this also affects length of "handle")
		// (all of these parameters should go in local_config.scad file)

		translate([ -arch_mount_width/2, arch_center_y + (15/2), -0.1 ]) cylinder( r=screwhole_radius_M30_passthru, h=plate_thick+0.2);
		translate([ -arch_mount_width/2, arch_center_y - (15/2), -0.1 ]) cylinder( r=screwhole_radius_M30_passthru, h=plate_thick+0.2);
		translate([ arch_mount_width/2, arch_center_y + (15/2), -0.1 ]) cylinder( r=screwhole_radius_M30_passthru, h=plate_thick+0.2);
		translate([ arch_mount_width/2, arch_center_y - (15/2), -0.1 ]) cylinder( r=screwhole_radius_M30_passthru, h=plate_thick+0.2);
		
		// holes for mounting power hub (hanging)
		//pwrhub_side   pwrhub_mount_sep
		if (pwrhub_side == "left") {
			translate([ left_edge+pwrhub_mount_inset, (pwrhub_mount_sep/2), -0.1 ]) cylinder(d=M3_throughhole_dia, h=plate_thick+0.2);
			translate([ left_edge+pwrhub_mount_inset, -(pwrhub_mount_sep/2), -0.1 ]) cylinder(d=M3_throughhole_dia, h=plate_thick+0.2);
		} else {
			translate([ right_edge-pwrhub_mount_inset, (pwrhub_mount_sep/2), -0.1 ]) cylinder(d=M3_throughhole_dia, h=plate_thick+0.2);
			translate([ right_edge-pwrhub_mount_inset, -(pwrhub_mount_sep/2), -0.1 ]) cylinder(d=M3_throughhole_dia, h=plate_thick+0.2);
		}

		// holes for mounting the plate to the car chassis
		// front pillars
		translate([ -pillars_lr_sep[chassis][0]/2, pillars_fb_sep[chassis]/2, -0.1 ]) 
			cylinder( d=pillars_dia[chassis], plate_thick + 0.2 );
		translate([  pillars_lr_sep[chassis][0]/2, pillars_fb_sep[chassis]/2, -0.1 ]) 
			cylinder( d=pillars_dia[chassis], h=plate_thick + 0.2 );
		// back pillars may have different separation
		translate([ -pillars_lr_sep[chassis][1]/2, -pillars_fb_sep[chassis]/2, -0.1 ]) 
			cylinder( d=pillars_dia[chassis], plate_thick + 0.2 );
		translate([  pillars_lr_sep[chassis][1]/2, -pillars_fb_sep[chassis]/2, -0.1 ]) 
			cylinder( d=pillars_dia[chassis], h=plate_thick + 0.2 );

		if (pillars_max_depth[chassis] < plate_thick) {
			if (chassis > 1) {
				cd1 = 10;
				cd2 = 15;
				ch = (plate_thick - pillars_max_depth[chassis]) + 0.1;
				translate([ pillars_lr_sep[chassis][0]/2, pillars_fb_sep[chassis]/2, pillars_max_depth[chassis] ]) cylinder(d1=cd1, d2=cd2, h=ch);
				translate([ -pillars_lr_sep[chassis][0]/2, pillars_fb_sep[chassis]/2, pillars_max_depth[chassis] ]) cylinder(d1=cd1, d2=cd2, h=ch);
				translate([ pillars_lr_sep[chassis][1]/2, -pillars_fb_sep[chassis]/2, pillars_max_depth[chassis] ]) cylinder(d1=cd1, d2=cd2, h=ch);
				translate([ -pillars_lr_sep[chassis][1]/2, -pillars_fb_sep[chassis]/2, pillars_max_depth[chassis] ]) cylinder(d1=cd1, d2=cd2, h=ch);
			}
		}

		if (frontLeftBoard == "SmallMint") {
			translate([ frontL_locs[ chassis][0], frontL_locs[ chassis][1], plate_thick ]) 
				rotate([ 0, 0, 90 ]) 
				component_smallmint_protoboard("holes"); 
			translate([ frontL_locs[ chassis][0], frontL_locs[ chassis][1]-31, -0.1 ]) roundedbox(12, 8, 2, plate_thick+0.2);
		}
		if (frontRightBoard == "SmallMint") {
			translate([ frontR_locs[ chassis][0], frontR_locs[ chassis][1], plate_thick ]) 
				rotate([ 0, 0, 90 ]) 
				component_smallmint_protoboard("holes");  
			translate([ frontR_locs[ chassis][0], frontR_locs[ chassis][1]-31, -0.1 ]) roundedbox(12, 8, 2, plate_thick+0.2);
		}

		// wiring passthru holes
		translate([ wirehole_locs[ chassis][0], wirehole_locs[ chassis][1], -0.1 ]) roundedbox( 10, 35, 3, plate_thick+0.2 );
		translate([ -wirehole_locs[ chassis][0], wirehole_locs[ chassis][1], -0.1 ]) roundedbox( 10, 35, 3, plate_thick+0.2 );

		// holes for neopixel bezel
		if (wantLightStrip == "Yes") {
			if (lightStrip_locs[chassis][1] == "r") {
				translate([ 0, -lightStrip_locs[chassis][0], -0.1 ]) holes_for_3neopixel_strip("reverse");
			} else {
				translate([ 0, -lightStrip_locs[chassis][0], -0.1 ]) holes_for_3neopixel_strip("normal");
			}
		}
	}
}


module component_body_mount_tube() {
	difference() {
		roundedbox( 9, 9, 1, wall_height);
		translate([ 0, 0, -0.1 ]) cylinder( d=TI30_through_hole_diameter, h=wall_height+0.2);
		translate([ 0, 0, -0.1 ]) cylinder( d=TI30_through_hole_diameter, h=wall_height+0.2);
	}
}

// this is for visualization purposes only (on "Show Assembly")
module draw_body_mount_pin() {
	union() {
		color([ 0, 0, 0 ]) cylinder( d=pillars_dia[chassis]-1, h=40);
		translate([ 0, 8, 36 ]) rotate([ 90, 0, 0 ]) color([ 1,1,1 ]) cylinder(d=1.5, h=18);
		translate([ 0, -3, 36 ]) rotate([ 90, 0, -30 ]) color([ 1,1,1 ]) cylinder(d=1.5, h=8);
		difference() {
			translate([ -1, 11, 35.5 ]) color([ 1,1,1 ]) cylinder(d=8, h=1.5);
			translate([ -1, 11, 35.4 ]) cylinder(d=5, h=1.7);
		}
	}
}

module plate_joiner_tube(length=90) {
	difference() {
		cube([ 8, length, 10]);
		translate([ 4, -0.1, 6  ]) rotate([ -90, 0, 0 ]) cylinder( d=4, h=length+0.2 );
	}
}

module plate_front_half(joint_line=0) {
	difference() {
		union() {
			part_top_plate();
			translate([ -(plate_x/2), -50+joiner_centers[chassis], plate_thick ]) plate_joiner_tube(90);
			translate([ (plate_x/2) - 8, -50+joiner_centers[chassis], plate_thick ]) plate_joiner_tube(90);
		}
		//translate([ -150, -299+joint_line, -0.1 ]) cube([300, 300, 100]);
		translate([ -150, -300+joint_line, -0.1 ]) cube([300, 300, 100]);
	}	

}

module plate_back_half(joint_line=0) {
	difference() {
		union() {
			part_top_plate();
			translate([ -(plate_x/2), -50+joiner_centers[chassis], plate_thick ]) plate_joiner_tube(90);
			translate([ (plate_x/2) - 8, -50+joiner_centers[chassis], plate_thick ]) plate_joiner_tube(90);
		}
		//translate([ -150, 1+joint_line, -0.1 ]) cube([300, 300, 100]);
		translate([ -150, 0+joint_line, -0.1 ]) cube([300, 300, 100]);
	}	
}



module draw_battbox_lid(batt_lid_attach) {	
	if (aux_battery == "anker6700") {
		draw_battbox_anker6700_lid(batt_lid_attach);
			translate([ 70/2, (43/2)+6, 2 ])  
		        linear_extrude(1.5) text("DonKCar", size=10, halign="center");
			translate([ 70/2, (43/2)-6, 2 ])  
		        linear_extrude(1) text(carIdent, size=8, halign="center", font = "Liberation Serif:style=Bold");
	} else if (aux_battery == "urgenex2000") {
		draw_battbox_urgenex2000_lid(batt_lid_attach);
			translate([ 55/2, (37/2)+6, 2 ])  
		        linear_extrude(1.5) text("DonKCar", size=9, halign="center");
			translate([ 55/2, (37/2)-6, 2 ])  
		        linear_extrude(1) text(carIdent, size=8, halign="center", font = "Liberation Serif:style=Bold");
	} else if (aux_battery == "NiMH 7.2v 2200 mAH") {
		draw_battbox_nimh2200_72_lid(batt_lid_attach);
			translate([ 55/2, (50/2)+6, 2 ])  
		        linear_extrude(1.5) text("DonKCar", size=9, halign="center");
			translate([ 55/2, (50/2)-6, 2 ])  
		        linear_extrude(1) text(carIdent, size=8, halign="center", font = "Liberation Serif:style=Bold");
	}
}