This project holds files related to the DonKCar project, which is an
implementation of autonomous RC-sized racer.  The project includes
OpenSCAD code to generate 3d-printable chassis that fit a variety
of purchasable RC car platforms; the chassis is user-configurable
and currently includes built-in options to generate models including a 
variety of hardware configurations including Raspberry Pi
and/or Jetson Nano for DonkeyCar implementations, and OpenMV-based
solutions using ESP32-S2 on an Adafruit Metro board as a main controller.  

The OpenSCAD modules in this project are dependent on the use of the 
lib_robo files which are available in [a separate repository here](https://github.com/dnkorte/lib_robo).
The "include" links in the OpenSCAD modules must be updated
to point to the lib_robo library files, wherever you have stored them in
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

This project has includes an (optional) as-yet still under software/hardware development option  that uses an OpenMV camera for the vision and lane-tracking portions and uses an ESP32-S2 (Adafruit Metro ESP32-S2 board) as a vehicle / platform main controller.  At the current time, the manual driving and core functions are very complete and functional -- allowing manual driving of the vehicle using a remote controller (clips to driver's belt), and allowing for setup/configuration using a self-hosted web menu system that works with a phone or laptop (straight-up web interface no software installation needed on the phone or laptop).  The onboard ESP32-S2 software incorporates a 240x240 TFT display for detailed user feedback, and a 24 element NeoPixel strip that provides runtime feedback for users "at a distance" and also talks bi-directionally to the nunchuk controller.  

The belt-mounted remote controller uses an Adafruit Reverse Feather ESP32-S2 with a built-on TFT display connected to a Wii Nunchuk controller that provides driving commands. The remote communicates with the racer using ESP-NOW protocol which means that all control is totally independent of local networks, and is bi-directional and responsive.

All of the above works at the current time, with the software as included here.  

The autonomous driving functions using the OpenMV camera are still under development, but are modelled on the OpenMV line following protocols, modified to communicate to the main controller over an i2c connection.  The racer main controller handles the actual throttle and steering servo commands, accepting path commands provided by the OpenMV software.  

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

## Remote Controller Using Nunchuk
The user controls the racer / robot using a Wii Nunchuk control unit that plugs into a belt-clipped or lanyard-mounted control box that includes a 240 x 135 TFT.  An [Adafruit Reverse TFT Feather](https://www.adafruit.com/product/5345) uses ESP-NOW protocol to communicate bi-directionally with the racer / robot.  It sends joystick and button-press commands to the racer, and receives messages back from racer for display on the TFT.  It is a very quick and responsive protocol with good range (at least 100 feet in my yard, but not tested any farther). The simple messaging protocol could be easily used to control any robot / racer configuration.  The control box is powered by a small LiPo battery and may be worn using a belt-clip or a [neck lanyard](https://www.amazon.com/Bestom-Breakaway-Lanyards-Lanyards-no-Twist/dp/B09LQJJHFQ) (of the type typically used for ID badges).  

Some photos of the controller box are shown below, or a more complete [photo album is available here](https://photos.app.goo.gl/xJtTXaHJivE7bsBp9), and [short video of unit in operation is linked here](https://photos.app.goo.gl/UN85hcr63vpKxPkb9).

<table><tr>
	<td>
		<img width="280" src="https://github.com/dnkorte/DonKCar/blob/main/images/controller_nunchuk_on_lanyard.jpg" alt="Controller on Lanyard">
	</td>
	<td>
		<img width="280" src="https://github.com/dnkorte/DonKCar/blob/main/images/controller_nunchuk_on_lanyard_top_view.jpg" alt="Controller on Lanyard">
	</td>
	<td>
		<img width="280" src="https://github.com/dnkorte/DonKCar/blob/main/images/controller_nunchuk_on_belt_top_view.jpg" alt="Controller on belt-clip">
	</td>
</tr><tr>
	<td colspan=3>
		<img width="840" src="https://github.com/dnkorte/DonKCar/blob/main/images/controller_nunchuk_on_belt.jpg" alt="Controller on belt-clip">
	</td>

</tr></table>