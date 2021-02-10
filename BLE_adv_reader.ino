/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

//00:12:56.513 -> Advertised Device: Name: ESP32_2, Address: 24:0a:c4:32:36:2e, txPower: 3 

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
String namePattern = "ESP32_";
BLEAdvertisedDevice advDevices[20];
 
BLEScan* pBLEScan;

bool isMatchingDevice(BLEAdvertisedDevice advertisedDevice) {
  String deviceName = advertisedDevice.getName().c_str();
  return (deviceName.indexOf(namePattern) == 0);    
}

bool deviceFoundAlready(BLEAdvertisedDevice * advDevices, BLEAdvertisedDevice device) {

  int arrLen = sizeof(advDevices) / sizeof(advDevices[0]);

  if (arrLen == 0) {
    return false;
  }
  for (int i = 0; i < arrLen  - 1; i++) {
    Serial.print(i);
    Serial.print("\n");
    if (advDevices[i].getAddress() === device.getAddress()) {
      return true;
    }
  }

  return false;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());

      if (isMatchingDevice(advertisedDevice)) {
        Serial.printf("Advertised Device: %s \n", advertisedDevice.getName().c_str());
        if (!deviceFoundAlready(advDevices, advertisedDevice)) {
          //
        } else {
          Serial.print("Found");
        }
      }
    }
};


void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);
}
