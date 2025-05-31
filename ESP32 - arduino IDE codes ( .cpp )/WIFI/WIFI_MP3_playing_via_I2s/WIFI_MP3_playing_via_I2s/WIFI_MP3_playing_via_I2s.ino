#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

// I2S audio output pins
#define I2S_DOUT 13  // Data 
#define I2S_BCLK 25  // Bit Clock
#define I2S_LRC  26  // Left/Right Clock or WS pin

#define LED_PIN  2   // Built-in LED for status indication

// WiFi credentials
const String ssid     = "Vigneshhh";
const String password = "Vigneshhh";

Audio audio;

// Memory monitoring variables
unsigned long lastMemoryCheck = 0;
unsigned long lastReconnect = 0;
const unsigned long MEMORY_CHECK_INTERVAL = 5000;    // Check every 5 seconds
const unsigned long RECONNECT_INTERVAL = 1800000;    // Reconnect every 30 minutes
const int MIN_FREE_HEAP = 15000;                     // Minimum free heap in bytes

#define WIFI_MAX_TRIES 5

void setupAudio() {
  Serial.println("Setting I2S output pins with memory-safe settings.");
  
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  
  // Conservative settings to prevent memory issues
  audio.setVolume(18);  // Lower volume to reduce processing
  audio.setConnectionTimeout(5000, 1500);  // Shorter timeouts to prevent buffer overflow
  
  Serial.println("Audio configured with memory-safe settings");
}

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  
  WiFi.disconnect(true);  // Full disconnect and clear settings
  delay(100);
  
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  
  // Set specific WiFi configuration for stability
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);  // Don't save WiFi config to flash
  
  WiFi.begin(ssid.c_str(), password.c_str());
  
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < WIFI_MAX_TRIES) {
    tries++;
    Serial.printf("WiFi attempt #%d\n", tries);
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi connected! RSSI: %d dBm\n", WiFi.RSSI());
    Serial.printf("Free heap after WiFi: %d bytes\n", ESP.getFreeHeap());
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println("WiFi FAILED!");
    ESP.restart();  // Restart if WiFi fails
  }
}

void connectStream() {
  Serial.println("Connecting to audio stream...");
  
  // Disconnect any existing stream first
  audio.stopSong();
  delay(100);
  
  //https://res.cloudinary.com/dlx1rzgjq/video/upload/v1748697172/birds-ambiance-204513_cpdig1.mp3
  //https://res.cloudinary.com/dsmvqzs8n/video/upload/v1748699612/birds-chirping-ambiance-26052_nghkuf.mp3
  bool connected = audio.connecttohost("https://res.cloudinary.com/dsmvqzs8n/video/upload/v1748699612/birds-chirping-ambiance-26052_nghkuf.mp3");
  
  if (connected) {
    Serial.println("Audio stream connected.");
  } else {
    Serial.println("Stream connection failed!");
    delay(5000);
    ESP.restart();
  }
}

void checkMemoryAndReconnect() {
  unsigned long currentTime = millis();
  
  // Regular memory monitoring
  if (currentTime - lastMemoryCheck >= MEMORY_CHECK_INTERVAL) {
    int freeHeap = ESP.getFreeHeap();
    Serial.printf("Free Heap: %d bytes\n", freeHeap);
    
    // If memory is critically low, restart
    if (freeHeap < MIN_FREE_HEAP) {
      Serial.println("CRITICAL: Low memory detected! Restarting...");
      delay(1000);
      ESP.restart();
    }
    
    lastMemoryCheck = currentTime;
  }
  
  // Preventive reconnection every 30 minutes to clear buffers
  if (currentTime - lastReconnect >= RECONNECT_INTERVAL) {
    Serial.println("Preventive reconnection to clear buffers...");
    
    audio.stopSong();
    delay(500);
    
    if (WiFi.status() == WL_CONNECTED) {
      connectStream();
    } else {
      connectWiFi();
      connectStream();
    }
    
    lastReconnect = currentTime;
    Serial.printf("Reconnected. Free heap: %d bytes\n", ESP.getFreeHeap());
  }
}

void blinkSOS(int repeats) {
  for (int t = 0; t < repeats; t++) {
    // S
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH); delay(200);
      digitalWrite(LED_PIN, LOW);  delay(200);
    }
    // O
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH); delay(800);
      digitalWrite(LED_PIN, LOW);  delay(200);
    }
    // S
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH); delay(200);
      digitalWrite(LED_PIN, LOW);  delay(200);
    }
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ESP32 Audio Player (Memory-Safe) ===");
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED on during setup
  
  // Set CPU to 240MHz but with memory optimization
  setCpuFrequencyMhz(240);
  Serial.printf("CPU: %d MHz, Initial Heap: %d bytes\n", 
                getCpuFrequencyMhz(), ESP.getFreeHeap());
  
  connectWiFi();
  setupAudio();
  connectStream();
  
  digitalWrite(LED_PIN, LOW);  // LED off when ready
  
  lastMemoryCheck = millis();
  lastReconnect = millis();
  
  Serial.println("Setup complete - monitoring memory...");
}

void loop() {
  // Main audio loop
  audio.loop();
  
  // Memory and connection monitoring
  checkMemoryAndReconnect();
  
  // Check WiFi status periodically
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 10000) {  // Every 10 seconds
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected! Restarting...");
      ESP.restart();
    }
    lastWiFiCheck = millis();
  }
  
  // Small delay to prevent watchdog issues
  delay(1);
}