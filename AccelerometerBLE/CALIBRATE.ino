/*
   This sketch calibrates the IMU accelerometer
*/

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
