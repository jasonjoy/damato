/*

*/
#include "LiquidCrystal.h"
#include "CurieIMU.h"

/* VARIABLES YOU SHOULD PLAY WITH */
/* ============================== */
// set Range to 2, 4, 8, or 16, corresponding to # of g's it will detect
int AccRange = 16;
// Frequency of reading acceleration in milliseconds
int ReadInterval = 2;
// Time to remember peak acceleration in milliseconds
int PeakHoldTime = 8000;
int xCalibration, yCalibration, zCalibration;

/* VARIABLES YOU SHOULD LEAVE ALONE */
/* ================================ */
LiquidCrystal lcd (12,11,5,4,3,2);
int PeakTimer = 0;
float TotalAcceleration;
// These are used to compare previous values so the LCD only updates when there is a change
// Values are initially set to improbable numbers so the the display always updates the first time through
float axPeak, ayPeak, azPeak;
float axPrev = -1, ayPrev = -1, azPrev = -1, PeakPrev = -1, PeakAcceleration = -1;

void setup() {
  Serial.begin(9600); // initialize Serial communication
  //while (!Serial);    // wait for the serial port to open

  // initialize device
  //Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // Set the accelerometer range
  CurieIMU.setAccelerometerRange(AccRange);
  CurieIMU.setAccelerometerRate(1600);

  //Initialize LCD:
  lcd.begin(16,2);
  
  //Run calibration
  calibrate();
}

void loop() {
  int axRaw, ayRaw, azRaw;         // raw accelerometer values
  float ax, ay, az;

  // read raw accelerometer measurements from device
  CurieIMU.readAccelerometer(axRaw, ayRaw, azRaw);

  // convert the raw accelerometer data to G's
  ax = convertRawAcceleration(axRaw,xCalibration);
  ay = convertRawAcceleration(ayRaw,yCalibration);
  az = convertRawAcceleration(azRaw,zCalibration);

  // Calculate total acceleration based on 3-axis inputs, and factoring out force of gravity
  TotalAcceleration = sqrt(sq(ax) + sq(ay) + sq(az));

  // If we get a new peak, update the peak values
  if (TotalAcceleration > PeakAcceleration) {
    PeakAcceleration = TotalAcceleration;
    PeakTimer = 0;
    //convert to absolute values for accurate comparison 
    ax = abs(ax);
    ay = abs(ay);
    az = abs(az);
    axPeak = ax;
    ayPeak = ay;
    azPeak = az;
    if (TotalAcceleration > 6) {                // Note on Arduino orientation: With diplay upright, Y axis is up-down
      lcd.setCursor (15,0);
      if (ay > 2  &&  ay > ax &&  (2*ay) > az) {   //if Y axis is a signifant vector, call it an uppercut
        lcd.print("U");
      }
      else if (az > ax  &&  az > (2*ay)) {        //if Z axis is the largest vector, call is a straight/jab
        lcd.print("S");
      }              
      else if (ax > az  &&  ax > ay) {            //if X axis is the largest vector, call is a hook
        lcd.print("H");
      }
      else {
        lcd.print("?");                         //it's a good punch, but we don't know what type it is       
      }   
    }
    else {
      lcd.print(" ");                           //too small to register; clear the display segment  
    }
  }
  // If this is not a new peak, keep displaying the old peak values
  else {
    ax = axPeak;
    ay = ayPeak;
    az = azPeak;
  }

  //format text for display. Note we can get rid of display variables as long as actual variables display correctly.
  /*float axDisplay = ax;
  float ayDisplay = ay;
  float azDisplay = az;
  float PeakDisplay = PeakAcceleration;*/

  // display accelerometer x/y/z values
  if (abs (ax - axPrev) > 0.02) {  //Udpate display if x measurement has changed
    lcd.setCursor (0,0);
    lcd.print("X:");
    lcd.print(ax);
  }
  if (abs (ay - ayPrev) > 0.02) {  //Udpate display if y measurement has changed
    lcd.setCursor (7,0);
    lcd.print(" Y:");
    lcd.print(ay);
  }
  if (abs (az - azPrev) > 0.02) {  //Udpate display if z measurement has changed
    lcd.setCursor (0,1);
    lcd.print("Z:");
    lcd.print(az);
  }
  if (abs (PeakAcceleration - PeakPrev) > 0.1) {  //Udpate display if peak acceleration has changed
    lcd.setCursor (8,1);
    lcd.print(PeakAcceleration);
    lcd.print("g");
  }
  lcd.setCursor (15,1);
  lcd.print(int(PeakTimer/1000));

  // remember values for comparison with next round
  axPrev = ax;
  ayPrev = ay;
  azPrev = az;
  PeakPrev = PeakAcceleration;  
  
  // wait for next read cycle
  delay(ReadInterval);

  // zero out peak display if more time than the peak interval has passed
  PeakTimer = PeakTimer + ReadInterval;
  if (PeakTimer > PeakHoldTime) {
    PeakTimer = 0;
    PeakPrev = -1;
    axPrev = -1;
    ayPrev = -1;
    azPrev = -1;   
    lcd.clear();
  }
}

float convertRawAcceleration(int aRaw, int aCalibration) {
  // Maximum negative g maps to a raw value of -32768
  // Maximum positive g maps to a raw value of 32767
  
  float a = ((aRaw - aCalibration) * AccRange) / 32768.0;

  return a;
}

void calibrate() {
  /* Update this later to detect lack of motion before starting, and restart if values change by >1% between readings */
  int axRaw, ayRaw, azRaw;         // raw accelerometer values
  long xCalSum = 0, yCalSum = 0, zCalSum = 0;

  lcd.setCursor (0,0);
  lcd.print("CALIBRATING...");

  for(int test = 5; test > 0 ; test--){
    lcd.setCursor (14,0);
    lcd.print(test);

    // read raw accelerometer measurements from device
    CurieIMU.readAccelerometer(axRaw, ayRaw, azRaw);

    lcd.setCursor (0,1);
    lcd.print(axRaw);
    lcd.print(" ");
    lcd.print(ayRaw);
    lcd.print(" ");
    lcd.print(azRaw);

    // add meaurements to previous test for averaging at end of test
    xCalSum = xCalSum + axRaw;
    yCalSum = yCalSum + ayRaw;
    zCalSum = zCalSum + azRaw;

    // wait some time
    delay(1000);
  }
  
  // testing done; calculate calibration values
  xCalibration = xCalSum / 5;
  yCalibration = yCalSum / 5;
  zCalibration = zCalSum / 5;  

  lcd.clear();
}


