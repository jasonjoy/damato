/*
   This sketch example partially implements the standard Bluetooth Low-Energy Heart Rate service.
   For more information: https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
*/

void sendToBluetooth() {
  /* Convert PeakAcceleration to faux heartRate value, so we can take advantage of the 
     already-defined Heart Rate BLE service
  */
  int heartRate = map(TotalAcceleration, 0, 20, 0, 200);
    const unsigned char heartRateCharArray[2] = { 0, (char)heartRate };
    heartRateChar.setValue(heartRateCharArray, 2);  // and update the heart rate measurement characteristic
}
