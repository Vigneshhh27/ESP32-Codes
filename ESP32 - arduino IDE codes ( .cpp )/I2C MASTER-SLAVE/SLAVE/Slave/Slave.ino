#include <Wire.h>

#define I2C_DEV_ADDR 0x55


#define PIR_PIN 15


#define MOSFET_AMBIENCE_RED_LED 19  //GPIO_NUM_13  //rgb pins // change to ORANGE **
#define MOSFET_AMBIENCE_BLUE_LED 
#define MOSFET_AMBIENCE_GREEN_LED
#define MOSFET_AMBIENCE_WARM_LED //GPIO_NUM_2  // warm light pin



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
  } else if (commandReceived.equalsIgnoreCase("pir")) {
        int pirValue = digitalRead(PIR_PIN);  // Read the PIR sensor value
      
        // Convert the sensor reading into a string
        if (pirValue == HIGH) {
          response = "yes";
        } else {
          response = "no";
        }
  } else if (commandReceived.equalsIgnoreCase("EM1_ON")) {
    digitalWrite(MOSFET_AMBIENCE_RED_LED, HIGH);
    response = "EM1_ON_STATE";



  } else if (commandReceived.equalsIgnoreCase("EM1_OFF")) {
    digitalWrite(MOSFET_AMBIENCE_RED_LED, LOW);
    response = "EM1_OFF_STATE";



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

  pinMode(PIR_PIN, INPUT); // Set PIR_PIN as input

  pinMode(MOSFET_AMBIENCE_RED_LED, OUTPUT);
  //pinMode(MOSFET_AMBIENCE_WARM_LED, OUTPUT);

  digitalWrite(MOSFET_AMBIENCE_RED_LED, LOW);

}

void loop() {
  // No additional processing required in loop
  delay(100);
}
