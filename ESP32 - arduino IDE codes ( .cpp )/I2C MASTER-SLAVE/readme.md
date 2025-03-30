# ESP32 I2C & BLE Master-Slave Communication

This project demonstrates communication between two ESP32 devices using I2C and BLE. The master device hosts a BLE server that listens for string commands ("red", "blue", "green") from a BLE client (e.g., a smartphone). Based on the received BLE command, the master sends an I2C request to the slave device, which then responds with a corresponding message (e.g., "colour1", "colour2", "colour3"). This response can later be replaced with real sensor data or actuation commands.

## Overview

- **I2C Communication:** The master and slave devices communicate using the I2C protocol. The master sends a command string via I2C to the slave and then requests a response.
- **BLE Integration:** The master initializes a BLE server with an input and output characteristic. When a command is received via BLE, it triggers the I2C request.
- **Dynamic Data Handling:** The slave processes the received I2C command and returns a corresponding response. In this example, the responses are hard-coded strings, but they could be sensor data or control signals in a real application.
- **Buffer Management:** The master performs a dummy I2C read to flush any stale data from the I2C buffer before reading the valid response from the slave.

## How It Works

1. **BLE on the Master:**  
   - The master initializes a BLE server with a writeable input characteristic and a readable/notify output characteristic.
   - A BLE client sends a command (e.g., "red") to the master.
   - The BLE callback triggers the `requestSlaveData()` function.

2. **I2C Communication:**  
   - The master sends the received command string over I2C to the slave.
   - To ensure that stale data is cleared, the master performs a dummy I2C read before making the actual data request.
   - The slave listens for the I2C command in its `onReceive()` callback and stores the command.
   - When the master requests data (triggering `onRequest()` on the slave), the slave checks the stored command and sends back the appropriate response:
     - `"red"` → `"colour1"`
     - `"blue"` → `"colour2"`
     - `"green"` → `"colour3"`
     - Otherwise, it sends `"unknown"`.

3. **BLE Data Update:**  
   - The master receives the I2C response and updates its BLE output characteristic, notifying the connected BLE client of the new data.

## Requirements

- Two ESP32 boards.
- Arduino IDE with ESP32 board support.
- Basic I2C wiring (connect SDA, SCL, and GND between master and slave).
- A BLE-capable device (e.g., a smartphone with a BLE scanner app like nRF Connect).

## Setup and Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/yourusername/esp32-i2c-ble-master-slave.git
   cd esp32-i2c-ble-master-slave
