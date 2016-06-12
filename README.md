#Punch Training Coach
This is a feedback and training device to help a boxer optimize punching force, reaction time, workout intensity, and endurance.

##Minimum Viable Product
* (Done) Detect, record, and display g-force of a punch from a target
* (Done) Build with Arduino 101 with built-in accelerometer and Bluetooth

##Additional Software Features
* (Done) Pattern recognition to detect straights/jabs, hooks, and uppercuts
* (Done) Bluetooth communication with computer or phone
* Call out punch force instead of displaying
* Audible prompt for punches
* Measure reaction time after audible prompt
* Take video with forward-facing camera and sync to data feed

##Additional Features requiring Hardware
* Load cell could calibrate directly measured force (lbs) with acceleration of the target (g-force). This accomplishes 2 things: (1) gives you a real-world force measurement for punches (2) allows you to normalize readings from different targets (e.g. 70 lbs. heavy bag and BOB)
* Utilize [Arduino built-in IMU](https://www.arduino.cc/en/Reference/CurieIMU) to measure g-force to 16g, [ADXL377](https://www.adafruit.com/products/1413?gclid=CISd382-_8wCFVNqfgodWAcAMg) to measure from 16g to 200g
* Multiple accelerometers wired to one Arduino could select sensor with the highest force and use data to localize the punch (head/body)
* Connect heartrate strap and pace punches to match target HR zone
* Acceleration sensor on hands could add a lot of useful data:
	- Hand speed
	- Inefficiency - cocking arm before punching
	- Break out reaction time vs. time to target into separate measurements
* Pressure sensors in feet could correlate weight transfer to punching power

##Problems and ideas
* How to baseline power measurement? Use a [load cell](http://www.digikey.com/product-detail/en/te-connectivity-measurement-specialties/FC2311-0000-0500-L/MSP6953-ND/809399
) to compare directly measured force to force calculated from g-force detection.
* Note that this has an output of 100mV, but the Arduino wants analog inputs of 3.3V. I think one of [these amplifiers](http://www.robotshop.com/en/strain-gauge-load-cell-amplifier-shield-2ch.html?gclid=CNqAhNSPhc0CFZNhfgodKigMhQ
) should solve the problem.
* Interesting [sensor for cheap calibration](https://www.sparkfun.com/products/8685). You could also use this to give accuracy points for landing a punch on the button.
* Hold all accelerometer values in a 4x4 array (currently held in variables)

|| total | ax | ay | az
|---|---|---|---|---
| current_reading  | [0,0] | [0,1] | [0,2] | [0,3]
| previous_reading | [1,0] | [1,1] | [1,2] | [1,3]
| previous_punch   | [2,0] | [2,1] | [2,2] | [2,3]
| peak_hold        | [3,0] | [3,1] | [3,2] | [3,3]



##Math
* Calculating energy from a pendulum:
http://www.endmemo.com/physics/spendulum.php
http://www.wikihow.com/Calculate-Kinetic-Energy
- Maximum punch speed = 10m/s
- Arm mass = 6 kg
- Kinetic energy = 300J
- Similar reference punch:
	- 68lb weight dropped from ceiling on a 1m pendulum – or – 
	- 45 lb weight on a 1.5m pendulum –or-
	- 34 lb weight on a 2.0m pendulum

##Similar Products and Projects
* [Puncho boxing glove](http://www.industrial-design-germany.com/innovations/intelligent-boxing-glove-puncho.html) (development prototype)

* [StrikeTec](http://efdsports.com/) – Really slick UI, but no indication that it will ever ship: 
![](http://efdsports.com/wp-content/uploads/ipad_3_ready.png)

* [Hysko](https://www.hykso.com/) – taking pre-orders for Sept 2016 ship date @ $129. 2017 price planned @ $219. Raised $120K from Y-Combinator and $80K from other investors.
![](http://i1.wp.com/wp6c4a81c0b4a7.blob.core.windows.net/wp-media/2016/04/DSC00022.png?zoom=2&w=1184&ssl=0)

* Nicely documented project with a 250g sensor:
https://abieneman.wordpress.com/2010/04/04/punch-acceleration-sensor/

* A different approach, but no useful code or parts list: http://www.eetimes.com/document.asp?doc_id=1279578

##Daily Development Notes

5/30
* Detection working
* Display working and displaying g-force on a 3 axes
* Peak hold feature working – might want to make this a function
* Wrote Accelerometer-calibrate function to calibrate for starting orientation
Issue:
* Need to be able to factor out 1g gravity
CureIMU.setAcceleratorOffset() might fix this, but I couldn’t get it to work as I expected: https://www.arduino.cc/en/Reference/CurieIMUsetAccelerometerOffset

6/1
* Ordered load cell, amplifier, 200g accelerometer
* Added straight/uppercut/hook detection, but I’m not sure it’s working right

6/2
* Acceleration calculation verified
* Punch type detection working

| aX | aY | aZ | gCalc | gDisp | punchType |
|---|---|---|---|---|---|
|   0.19 | 1.21 | 9.55 | 9.63 | 9.63 | Straight |
|   5.29 | 0.21 | 3.84 | 6.54 | 6.54 | Hook |
|   0.21 | 7.99 | 2.58 | 8.40 | 8.39 | Uppercut |
|   1.33 | 2.90 | 8.54 | 9.12 | 9.11 | Straight |
|   0.76 | 6.47 | 4.97 | 8.19 | 8.17 | Uppercut |
|   0.79 | 2.85 | 9.72 | 10.16 | 10.16 | Straight |

![](./images/accTest.jpg)

6/3
* Get Bluetooth working with this [heartrate monitor sketch](./HeartbeatBLE)
* Adapted from https://www.arduino.cc/en/Tutorial/Genuino101CurieBLEHeartRateMonitor

6/4
* Set up Github repo
* Integrate GitHub repo w/Slack channel
* Display accelerometer readings from BLE.
	* http://www.forward.com.au/pfod/BLE/index.html
	* Using nRF Master Control Panel, I see this is broadcasting as “101 BLE”, and accelerometer values are updating under the “TX Characteristic”

6/5
* Break sketch into multiple files
* Report TotalAcceleration to Bluetooth HeartRate service (AccelerationBLE sketch)

##Next steps
* Get serial port working w/Mac
* Record a stream of raw data
* Test IMU calibration sketch: https://www.arduino.cc/en/Tutorial/Genuino101CurieIMUOrientationVisualiser
* Test this sketch to visualize orientation of punching bag in real time:
https://www.arduino.cc/en/Tutorial/Genuino101CurieIMUOrientationVisualiser
* Create a reference for generating kinetic energy: weight swinging from a pendulum to hit a load cell
* As a second test, place load cell on far side of bag and have it strike a hard surface. This should measure how much energy is lost to deformation/compression of punching bag


