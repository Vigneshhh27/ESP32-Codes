#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define PERIPHERAL_NAME "Smart-Mat_esp32"
#define SERVICE_UUID "fd34f551-28ea-4db1-b83d-d4dc4719c508"
#define CHARACTERISTIC_INPUT_UUID "fad2648e-5eba-4cf8-9275-68df18d432e0"
#define CHARACTERISTIC_OUTPUT_UUID "142F29DD-B1F0-4FA8-8E55-5A2D5F3E2471"

#define I2C_DEV_ADDR 0x55

// Global pointer for the BLE output characteristic
BLECharacteristic *pOutputChar;

// Function to request data from the slave based on a command
String requestSlaveData(String command) {
  // Send the command to the slave via I2C.
  Wire.beginTransmission(I2C_DEV_ADDR);
  Wire.write((const uint8_t*)command.c_str(), command.length());
  uint8_t error = Wire.endTransmission();
  if (error != 0) {
    Serial.printf("I2C Transmission error: %u\n", error);
    return "";
  }
  
  // Allow the slave time to process the command.
  delay(100);
  
  int expectedLength = 8; // adjust this if your responses change in length
  
  // === Dummy Read: Flush the first (stale) response ===
  int dummyBytes = Wire.requestFrom(I2C_DEV_ADDR, expectedLength);
  Serial.printf("Dummy read bytes: %d\n", dummyBytes);
  while (Wire.available()) {
    Wire.read(); // discard all bytes from the dummy read
  }
  
  // Short delay between the dummy read and the actual request.
  delay(100);
  
  // === Actual Read: Get the valid response ===
  int bytesReceived = Wire.requestFrom(I2C_DEV_ADDR, expectedLength);
  Serial.printf("I2C bytes received (requested): %d\n", bytesReceived);
  
  int availableBytes = Wire.available();
  Serial.printf("I2C bytes available: %d\n", availableBytes);
  
  String response = "";
  while (Wire.available()) {
    char c = Wire.read();
    if (c != '\0') {  // ignore null terminators
      response += c;
    }
  }
  Serial.printf("Received from slave: %s\n", response.c_str());
  return response;
}

// BLE callback for when the input characteristic is written
class InputReceivedCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    // Use Arduino String type for convenience.
    String value = pCharacteristic->getValue();
    Serial.printf("BLE Received: %s\n", value.c_str());
    
    value.trim();
    String slaveData = "";
    
    if (value.equalsIgnoreCase("red")) {
      slaveData = requestSlaveData("red");
    } else if (value.equalsIgnoreCase("blue")) {
      slaveData = requestSlaveData("blue");
    } else if (value.equalsIgnoreCase("green")) {
      slaveData = requestSlaveData("green");
    } else {
      Serial.println("Invalid command received via BLE");
      return;
    }
    
    // Update the BLE output characteristic and notify the client.
    pOutputChar->setValue(slaveData.c_str());
    pOutputChar->notify();
  }
};

// BLE server callbacks
class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("BLE Client Connected");
  }
  void onDisconnect(BLEServer* pServer) {
    Serial.println("BLE Client Disconnected");
    BLEDevice::startAdvertising();
  }
};

void initBLE() {
  BLEDevice::init(PERIPHERAL_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // Create input characteristic for BLE writes.
  BLECharacteristic *pInputChar = pService->createCharacteristic(
    CHARACTERISTIC_INPUT_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pInputChar->setCallbacks(new InputReceivedCallbacks());
  
  // Create output characteristic for BLE reads/notifications.
  pOutputChar = pService->createCharacteristic(
    CHARACTERISTIC_OUTPUT_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pOutputChar->addDescriptor(new BLE2902());
  
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE Initialized and Advertising");
}

void setup() {
  Serial.begin(115200);
  // Initialize I2C as master.
  Wire.begin();
  // Initialize BLE.
  initBLE();
}

void loop() {
  // The main loop does nothing; BLE callback triggers I2C requests when data is written.
  delay(1000);
}
