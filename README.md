This project holds files related to the DonKCar project, which is an
implementation of autonomous RC-sized racer.  The project includes
OpenSCAD code to generate 3d-printable chassis that fit a variety
of purchasable RC car platforms; the chassis is user-configurable
and currently includes built-in options to generate models including a 
variety of hardware configurations including Raspberry Pi
and/or Jetson Nano for DonkeyCar implementations, and OpenMV-based
solutions using Adafruit Metro ESP32-S2 as a main controller.  

The OpenSCAD modules in this project are dependent on the use of the 
lib_robo files which are available in a separate repository here.
The "include" links in the OpenSCAD modules must be updated
to point to the library files, wherever you have stored them in
your filesystem

All plates may be automatically "split" so that they can be printed
on standard 180x180mm or larger 3d-printers.  The designs are currently 
configured for the following RC chassis:

*[1/10 Tamiya TT02B Neo Scorcher (2-post)](https://smile.amazon.com/gp/product/B00DSVF3BY)

*[1/10 Tamiya TT02 Toyota GR 86  (4-post)](https://www.amazon.com/TAMIYA-10-Toyota-TT-02-TAM58694/dp/B09KRR5GKS)

*[Hosim 1/12 Monster Truck](https://www.amazon.com/Hosim-9155-Speed-Monster-Trucks/dp/B07SRTHB3L?th=1) 

*[Holyton 1:10 Large High Speed Remote Control Car](https://www.amazon.com/dp/B08B1F3494)

*[Electric Infinitive EP RTR Off Road Truck](https://www.nitrorcx.com/51c803-truck-ttyellowcarbon.html)

Note that these are all readily available chassis in early 2023
 

<h3 align="center">
	<img width="853" src="https://github.com/dnkorte/DonKCar/blob/main/images/openscad_screenshot_chassis.jpg" alt="sample openscad screen"><br>
</h3>

If using a standard DonkeyCar software package on either a Raspberry Pi or Jetson Nano, none of the Arduino code packages are needed, and the OpenSCAD models are very stable and complete for those configurations.

## ESP32-S2 + OpenMV Configuration

This project has an as-yet still under development option  that uses an OpenMV camera for the vision and line-tracking portions and uses an ESP32-S2 (Adafruit Metro ESP32-S2 board) as a vehicle / platform main controller.  At the current time, the manual functions are very complete and functional -- allowing manual driving of the vehicle using a remote controller (clips to driver's belt), and allowing for setup/configuration using a self-hosted web menu system that works with a phone or laptop (straight-up web interface no software installation on the phone or laptop).  The onboard ESP32-S2 system incorporates a 240x240 TFT display for detailed user feedback, and a 24 element NeoPixel strip that provides runtime feedback for users "at a distance".  

The belt-mounted remote controller uses an Adafruit QTPy ESP32-S2 with a small OLED display connected to a Wii Nunchuk controller that provides driving commands. An optional variant uses an Adafruit Reverse TFT Feather -- this has a larger display but is slightly more costly).  The remote communicates with the racer using ESP-NOW protocol which means that all control is totally independent of local networks.  

All of the above works at the current time, with the software as included here.  

The autonomous driving functions using the OpenMV camera are still under development, but are modelled on the OpenMV line following protocol, modified to communicate to the main controller over an i2c connection.  The racer main controller handles the actual throttle and steering servo control, accepting path commands provided by the OpenMV software.  

**Note that the ESP32-S2 / OpenMV configuration is currently very much still under development so use or reference at your own risk.**

<table><tr>
	<td>
		<img width="280" src="https://github.com/dnkorte/DonKCar/blob/main/images/tt02b_chassis.jpg" alt="Tamiya TT02b 2-post chassis printed in 1 piece">
	</td>
	<td>
		<img width="280" src="https://github.com/dnkorte/DonKCar/blob/main/images/holyton_chassis.jpg" alt="Holyton RTR chassis printed in 2 pieces, joined">
	</td>
	<td>
		<img width="280" src="https://github.com/dnkorte/DonKCar/blob/main/images/rpi_on_nitro_chassis.jpg" alt="Raspberry Pi on Nitro RCX chassis (older scad design)">
	</td>
</tr></table>