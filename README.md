This project holds files related to the DonKCar project, which is an
implementation of autonomous RC-sized racer.  The project includes
OpenSCAD code to generate 3d-printable chassis that fit a variety
of purchasable RC car platforms; the chassis is user-configurable
to currently includes built-in options to support  a variety of 
software/hardware implementations including Raspberry Pi
and/or Jetson Nano for DonkeyCar implementations, and OpenMV-based
solutions using Adafruit Metro ESP32-S2 as a main controller.  

The OpenSCAD modules in this project are dependent on the use of the 
lib_robo files which are available in a separate repository here.
The <include XXX> links in the OpenSCAD modules must be updated
to point to the library files.

All plates may be automatically "split" so that they can be printed
on standard 180x180mm or larger 3d-printers.  The designs are currently 
configured for the following RC chassis:

*[1/10 Tamiya TT02-2 Neo Scorcher (2-post)](https://smile.amazon.com/gp/product/B00DSVF3BY)

*[1/10 Tamiya TT02-4 Toyota GR 86  (4-post)](https://www.amazon.com/TAMIYA-10-Toyota-TT-02-TAM58694/dp/B09KRR5GKS)

*[Hosim 1/12 Monster Truck](https://www.amazon.com/Hosim-9155-Speed-Monster-Trucks/dp/B07SRTHB3L?th=1) 

*[Holyton 1:10 Large High Speed Remote Control Car](https://www.amazon.com/dp/B08B1F3494)

*[Electric Infinitive EP RTR Off Road Truck](https://www.nitrorcx.com/51c803-truck-ttyellowcarbon.html)

Note that these are all readily available chassis in early 2023

This project is currently very much still under development so use
or reference at your own risk.  

<h3 align="center">
	<img width="853" src="https://github.com/dnkorte/DonKCar/blob/main/images/openscad_screenshot_chassis.jpg" alt="sample openscad screen"><br>
</h3>