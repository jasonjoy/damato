/*
   Copyright (c) 2015 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

/*
   This sketch example demonstrates how the BMI160 on the
   Intel(R) Curie(TM) module can be used to read accelerometer data
*/
#include "LiquidCrystal.h"
#include "CurieIMU.h"

LiquidCrystal lcd (12,11,5,4,3,2);
// set AccRange to 2, 4, 8, or 16, correspending to # of g's it will detect
int AccRange = 4;
// Frequency of reading acceleration in milliseconds
int ReadInterval = 250;
// Time to remember peak acceleration peak in milliseconds
int PeakHoldTime = 10000;
int PeakTimer = 0;

float axPeak; //Only using peak X-axis value for now

void setup() {
  Serial.begin(9600); // initialize Serial communication
  //while (!Serial);    // wait for the serial port to open

  // initialize device
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(AccRange);

  //Initialize LCD:
  lcd.begin(16,2);
}

void loop() {
  int axRaw, ayRaw, azRaw;         // raw accelerometer values
  float ax, ay, az;

  // read raw accelerometer measurements from device
  CurieIMU.readAccelerometer(axRaw, ayRaw, azRaw);

  // convert the raw accelerometer data to G's
  ax = convertRawAcceleration(axRaw);
  ay = convertRawAcceleration(ayRaw);
  az = convertRawAcceleration(azRaw);

  if (abs(ax) > abs(axPeak)) {
    axPeak = ax;
    PeakTimer = 0;
  }

  // display tab-separated accelerometer x/y/z values
  lcd.setCursor (0,0);
  lcd.print("X:");
  lcd.print(ax);
  lcd.setCursor (7,0);
  lcd.print(" Y:");
  lcd.print(ay);
  lcd.setCursor (0,1);
  lcd.print("Z:");
  lcd.print(az);
  lcd.setCursor (8,1);
  lcd.print(axPeak);
  lcd.setCursor (15,1);
  lcd.print(int(PeakTimer/1000));
  // wait 1 second
  delay(ReadInterval);
  PeakTimer = PeakTimer + ReadInterval;
  if (PeakTimer > PeakHoldTime) {
    axPeak = ax;
    PeakTimer = 0;
  }
}

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
  
  float a = (aRaw * AccRange) / 32768.0;

  return a;
}

