#include <Wire.h>

#define I2C_DEV_ADDR 0x55

// Global variable to store the command received from the master
String commandReceived = "";

void onReceive(int len) {
  commandReceived = "";
  while (Wire.available()) {
    char c = Wire.read();
    if (c != '\0') {
      commandReceived += c;
    }
  }
  Serial.print("I2C command received: ");
  Serial.println(commandReceived);
}

void onRequest() {
  String response = "";
  if (commandReceived.equalsIgnoreCase("red")) {
    response = "colour1";
  } else if (commandReceived.equalsIgnoreCase("blue")) {
    response = "colour2";
  } else if (commandReceived.equalsIgnoreCase("green")) {
    response = "colour3";
  } else {
    response = "unknown";
  }
  
  Serial.print("I2C sending response: ");
  Serial.println(response);
  
  // Send the response string to the master
  Wire.write((const uint8_t*)response.c_str(), response.length());

  // Clear the command so that response is only sent once per request
  commandReceived = "";
}

void setup() {
  Serial.begin(115200);
  Wire.begin((uint8_t)I2C_DEV_ADDR);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
}

void loop() {
  // No additional processing required in loop
  delay(100);
}
