/*
This sketch integrates the existing Accelerometer sketch with a BLE sketch.
*/

#include "LiquidCrystal.h"
#include "CurieIMU.h"
#include "CurieBLE.h"


/* VARIABLES YOU SHOULD PLAY WITH */
/* ============================== */
// set Range to 2, 4, 8, or 16, corresponding to # of g's it will detect
int AccRange = 16;
// Frequency of reading acceleration in milliseconds
int ReadInterval = 250;
// Time to remember peak acceleration in milliseconds
int PeakHoldTime = 1000;
int xCalibration, yCalibration, zCalibration;
BLEPeripheral blePeripheral;       // BLE Peripheral Device (the board you're programming)
BLEService heartRateService("180D"); // BLE Heart Rate Service

// BLE Heart Rate Measurement Characteristic"
BLECharacteristic heartRateChar("2A37",  // standard 16-bit characteristic UUID
    BLERead | BLENotify, 2);  // remote clients will be able to get notifications if this characteristic changes
                              // the characteristic is 2 bytes long as the first field needs to be "Flags" as per BLE specifications
                              // https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml

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
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // Set the accelerometer range
  CurieIMU.setAccelerometerRange(AccRange);
  CurieIMU.setAccelerometerRate(1600);

  // Initialize LCD:
  lcd.begin(16,2);
  
  // Run calibration
  calibrate();

  // Do Bluetooth stuff 
  blePeripheral.setLocalName("HeartRateSketch");
  blePeripheral.setAdvertisedServiceUuid(heartRateService.uuid());  // add the service UUID
  blePeripheral.addAttribute(heartRateService);   // Add the BLE Heart Rate service
  blePeripheral.addAttribute(heartRateChar); // add the Heart Rate Measurement characteristic

  /* Now activate the BLE device.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */
  blePeripheral.begin();
  pinMode(13, OUTPUT);   // initialize the LED on pin 13 to indicate when a central is connected

}

void loop() {
  int axRaw, ayRaw, azRaw;         // raw accelerometer values
  float ax, ay, az;

  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();
  // if a central is connected to peripheral:
  if (central) {
    // turn on the LED to indicate the connection:
    digitalWrite(13, HIGH);
    Serial.println(central.address());
  }
  else {
    // when the central disconnects, turn off the LED:
    digitalWrite(13, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
  // read raw accelerometer measurements from device
  CurieIMU.readAccelerometer(axRaw, ayRaw, azRaw);

  // convert the raw accelerometer data to G's
  ax = convertRawAcceleration(axRaw,xCalibration);
  ay = convertRawAcceleration(ayRaw,yCalibration);
  az = convertRawAcceleration(azRaw,zCalibration);

  // Calculate total acceleration based on 3-axis inputs, and factoring out force of gravity
  TotalAcceleration = sqrt(sq(ax) + sq(ay) + sq(az));

  // Print everything to serial console
      Serial.print("ax:");
      Serial.print(ax);
      Serial.print(" ay:");
      Serial.print(ay);
      Serial.print(" az:");
      Serial.print(az);
      Serial.print(" gCalc:");
      Serial.println(TotalAcceleration);
                  
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
    lcd.print(TotalAcceleration);
    lcd.print("g");
    if (central.connected()) {
      sendToBluetooth();
    }
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
    PeakPrev = -2;
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



