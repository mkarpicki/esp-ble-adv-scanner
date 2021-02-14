/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

//00:12:56.513 -> Advertised Device: Name: ESP32_2, Address: 24:0a:c4:32:36:2e, txPower: 3 

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

static BLERemoteCharacteristic* pRemoteCharacteristic;
BLEScan* pBLEScan;

/**
 * Static serviceId and characteristicsId used by all advertising devices
 * @todo: think if may need to use many services or at least characterisitcs in long term (per device type maybe)
 */
static BLEUUID serviceUUID("9d319c9c-3abb-4b58-b99d-23c9b1b69ebc");
static BLEUUID charUUID("a869a793-4b6e-4334-b1e3-eb0b74526c14");

/**
 * Varialbe keeps known name prefix of supported adv devices
 * @todo: Think if get read of this logic in long term and filter by serviceId's but that will require connect to all found first 
 */
static String namePattern = "ESP32_";

/**
 * Predefined size of array of expected advertizing devices to support
 * @todo: consider using dynamic list in future
 */
const int numberOfDevices = 20;
String connectedAddresses[numberOfDevices];
//BLEAdvertisedDevice* myDevices[numberOfDevices];

String foundAddressesStr = "";

String addressToStr(BLEAddress address) {
  return address.toString().c_str();
}

String getAddress(BLEAdvertisedDevice device) {
  return addressToStr(device.getAddress());
}

bool isMatchingDevice(BLEAdvertisedDevice advertisedDevice) {
  String deviceName = advertisedDevice.getName().c_str();
  return (deviceName.indexOf(namePattern) == 0);    
}



bool isAddressConnectedAlready(BLEAddress bleAddress) {
  String address = addressToStr(bleAddress);
  return (foundAddressesStr.indexOf(address) != -1);
}

void rememberAddress(BLEAddress bleAddress) {
  String address = addressToStr(bleAddress);
//  if(foundAddressesStr == "") {
//    foundAddressesStr.concat(address);
//  } else {
//
//  }
    foundAddressesStr.concat(",");
    foundAddressesStr.concat(address);  
}

//bool isDeviceAdded(BLEAdvertisedDevice* device) {
//  for (int i = 0; i < numberOfDevices; i++){
//    if (myDevices[i] != nullptr){
//      if (getAddress(*myDevices[i]) == getAddress(*device)) {
//        return true;
//      }
//    }
//  }
//  return false;
//}
//
//void addDevice(BLEAdvertisedDevice* device) {
//  for (int i = 0; i < numberOfDevices; i++){
//    if (myDevices[i] == nullptr){
//      myDevices[i] = new BLEAdvertisedDevice(*device);
//      return;
//    }
//  }
//}
//
//
//void removeDevice(BLEAdvertisedDevice* device) {
//  for (int i = 0; i < numberOfDevices; i++){
//    if (myDevices[i] != nullptr){
//      if (getAddress(*myDevices[i]) == getAddress(*device)) {
//        myDevices[i] = nullptr;
//      }
//    }
//  }
//}

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
    Serial.println(addressToStr(pclient->getPeerAddress()));
    rememberAddress(pclient->getPeerAddress());
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.println("onDisconnect");
    Serial.println(addressToStr(pclient->getPeerAddress()));
  }
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.getName().c_str());
    }
};


void connectToBLE(BLEAdvertisedDevice* device) {

  BLEClient* pClient  = BLEDevice::createClient();

  pClient->setClientCallbacks(new MyClientCallback());  
  pClient->connect(device);

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  
  if (pRemoteService == nullptr) {
    pClient->disconnect();
    return;
  }

  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);

  if (pRemoteCharacteristic == nullptr) {
    pClient->disconnect();
    return;    
  }

//  if(pRemoteCharacteristic->canRead()) {
//    std::string value = pRemoteCharacteristic->readValue();
//    Serial.print("The characteristic value was: ");
//    Serial.println(value.c_str());
//  }
    
  Serial.println("connected");

  pClient->disconnect();


  
  //if(pRemoteCharacteristic->canNotify())
  //  pRemoteCharacteristic->registerForNotify(notifyCallback);
  //}
  
}

void connectToDevices(BLEScanResults foundDevices) {
  int foundDevicesCount = foundDevices.getCount();
  BLEAdvertisedDevice device;
  
  for (int i = 0; i < foundDevicesCount; i++){
    device = foundDevices.getDevice(i);

    if(isMatchingDevice(device)) {
      if (!isAddressConnectedAlready(device.getAddress())) {
        connectToBLE(&device);
        delay(1000);        
      } else {
        Serial.println("this device was conneted already!");
      }

    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  int scanTime = 5; //In seconds
  
  Serial.println("Scanning...");
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");

  connectToDevices(foundDevices);
  
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

  Serial.print("foundAddressesStr: ");
  Serial.println(foundAddressesStr);
  delay(scanTime * 1000);
  
}
