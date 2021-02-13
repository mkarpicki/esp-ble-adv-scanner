/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

//00:12:56.513 -> Advertised Device: Name: ESP32_2, Address: 24:0a:c4:32:36:2e, txPower: 3 

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

static BLEUUID serviceUUID("9d319c9c-3abb-4b58-b99d-23c9b1b69ebc");
static BLEUUID charUUID("a869a793-4b6e-4334-b1e3-eb0b74526c14");


String namePattern = "ESP32_";
//String foundAddressesStr = "";

static BLERemoteCharacteristic* pRemoteCharacteristic;
 
BLEScan* pBLEScan;

//temp
const int numberOfDevices = 20;
BLEAdvertisedDevice* myDevices[numberOfDevices];
BLEAdvertisedDevice* myConnectedDevices[numberOfDevices];

String getAddress(BLEAdvertisedDevice device) {
  return device.getAddress().toString().c_str();
}

bool isMatchingDevice(BLEAdvertisedDevice advertisedDevice) {
  String deviceName = advertisedDevice.getName().c_str();
  return (deviceName.indexOf(namePattern) == 0);    
}

//bool deviceFoundAlready(BLEAdvertisedDevice device) {
//  String address = getAddress(device);
//  return (foundAddressesStr.indexOf(address) != -1);
//}
//
//void rememberAddress(BLEAdvertisedDevice device) {
//  String address = getAddress(device);
//  if(foundAddressesStr == "") {
//    foundAddressesStr.concat(address);
//  } else {
//    foundAddressesStr.concat(",");
//    foundAddressesStr.concat(address);
//  }
//  
//}

bool deviceAdded(BLEAdvertisedDevice device) {
  for (int i = 0; i < numberOfDevices; i++){
    if (myDevices[i] != nullptr){
      if (getAddress(*myDevices[i]) == getAddress(device)) {
        return true;
      }
    }
  }
  return false;
}

void addDevice(BLEAdvertisedDevice device) {
  for (int i = 0; i < numberOfDevices; i++){
    if (myDevices[i] == nullptr){
      myDevices[i] = new BLEAdvertisedDevice(device);
      return;
    }
  }
}

//void addDeviceAsConnected(BLEAdvertisedDevice* device) {
//  for (int i = 0; i < numberOfDevices; i++){
//    if (myConnectedDevices[i] == nullptr){
//      myConnectedDevices[i] = device;
//      return;
//    }
//  }
//}


void connectToDevices() {
  for (int i = 0; i < numberOfDevices; i++){
    if (myDevices[i] != nullptr){
      //Serial.println(getAddress(*myDevices[i]));
      connectToBLE(myDevices[i]);
      delay(1000);
    }
  }
}


//static void notifyCallback(
//  BLERemoteCharacteristic* pBLERemoteCharacteristic,
//  uint8_t* pData,
//  size_t length,
//  bool isNotify) {
//    Serial.print("Notify callback for characteristic ");
//    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
//    Serial.print(" of data length ");
//    Serial.println(length);
//    Serial.print("data: ");
//    Serial.println((char*)pData);
//}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("onConnect");
    
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.println("onDisconnect");
    Serial.println(pclient->getPeerAddress().toString().c_str());
  }
};

void connectToBLE(BLEAdvertisedDevice* device) {

  BLEClient* pClient  = BLEDevice::createClient();

  Serial.println("in");

  pClient->setClientCallbacks(new MyClientCallback());  
  pClient->connect(device);
 
  Serial.println("check");
//
//  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
//  
//  if (pRemoteService == nullptr) {
//    pClient->disconnect();
//    return;
//  }
//
//  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//
//  if (pRemoteCharacteristic == nullptr) {
//    pClient->disconnect();
//    return;    
//  }
//
//  if(pRemoteCharacteristic->canRead()) {
//    std::string value = pRemoteCharacteristic->readValue();
//    Serial.print("The characteristic value was: ");
//    Serial.println(value.c_str());
//  }  
//
  pClient->disconnect();


  
  //if(pRemoteCharacteristic->canNotify())
  //  pRemoteCharacteristic->registerForNotify(notifyCallback);
  //}
  
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());

      if (isMatchingDevice(advertisedDevice)) {
        Serial.printf("Advertised Device: %s \n", advertisedDevice.getName().c_str());

        if (!deviceAdded(advertisedDevice)) {
          addDevice(advertisedDevice);  
        }
        
        //if (!deviceFoundAlready(advertisedDevice)) {
        //  rememberAddress(advertisedDevice);          
          //connectToBLE(advertisedDevice);
        //} 
        //connectToBLE(advertisedDevice);
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
  int scanTime = 5; //In seconds
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

  //Serial.println(foundAddressesStr);
  delay(scanTime * 1000);
  connectToDevices();
  
}
