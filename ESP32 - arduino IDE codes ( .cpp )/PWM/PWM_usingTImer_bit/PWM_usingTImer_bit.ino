#include <Arduino.h>

// Global settings: set the PWM output pin and initial frequency (in Hz)
const int pwmPin = 21;  //18;               // PWM output pin (change as needed)
volatile uint32_t pwmFrequency = 2400000;//113000;  // Initial PWM frequency (113 kHz)

// esp_timer handle for our periodic callback
esp_timer_handle_t periodic_timer = NULL;

// Variable to hold the current state of the pin (HIGH or LOW)
volatile bool pinState = false;

// Timer callback that toggles the PWM output
void IRAM_ATTR onTimer(void* arg) {
  // Toggle the pin state
  pinState = !pinState;
  digitalWrite(pwmPin, pinState);
}

// Function to update the timer period according to the current pwmFrequency.
// The timer is configured so that each callback toggles the pin, which is half the PWM period.
// Hence, half_period (in microseconds) = 1e6 / (2 * pwmFrequency)
void updateTimerPeriod() {
  uint64_t half_period_us = 1000000ULL / (2 * pwmFrequency);
  // Ensure we have at least 1 µs period (esp_timer resolution)
  if (half_period_us < 1) {
    half_period_us = 1;
  }
  
  // Stop the timer before updating its period
  esp_timer_stop(periodic_timer);
  // Restart the timer with the new period (in microseconds)
  esp_timer_start_periodic(periodic_timer, half_period_us);
}

void setup() {
  Serial.begin(115200);
  // Initialize the PWM pin as an output
  pinMode(pwmPin, OUTPUT);
  
  // Create the esp_timer that will call onTimer periodically
  const esp_timer_create_args_t timer_args = {
    .callback = &onTimer,
    .arg = NULL,
    .name = "PWM_Timer"
  };
  esp_timer_create(&timer_args, &periodic_timer);
  
  // Set the initial timer period based on pwmFrequency
  updateTimerPeriod();
  
  Serial.print("PWM running on pin ");
  Serial.print(pwmPin);
  Serial.print(" at ");
  Serial.print(pwmFrequency);
  Serial.println(" Hz");
}

void loop() {
  // In this example, we do nothing in loop.
  // If you want to change the frequency at runtime,
  // update 'pwmFrequency' and then call updateTimerPeriod().
  delay(1000);
}
