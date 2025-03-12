#include "driver/mcpwm.h"

// Use only one pin: GPIO 21 for MCPWM0A
#define PWM_PIN_A 21
// #define PWM_PIN_B 19   // Complementary output (not used)

void setup() {
  Serial.begin(115200);
  
  // Initialize the MCPWM GPIO pin for channel A on MCPWM_UNIT_0
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWM_PIN_A);
  // The complementary channel is commented out:
  // mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PWM_PIN_B);

  // Configure PWM parameters
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 2400000;  //113000;// 2000;    // PWM frequency: 2 kHz
  pwm_config.cmpr_a = 50; //30;         // Initial duty cycle: 30% on channel A  // Constant duty cycle: 50% on channel A
  pwm_config.cmpr_b = 50; //30;         // Duty cycle for channel B (not used)
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

  // Initialize MCPWM Timer 0 on MCPWM_UNIT_0 with the configuration
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);

  // Dead time configuration is omitted since we are not using the complementary channel
  // mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 10, 10);

  // Start PWM output
  mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
}

void loop() {


  // No changes here: PWM output remains constant at 50% duty cycle.
  delay(1000);



  /* for dim in and  out looping
  // Sweep the duty cycle on channel A from 10% to 90% and back
  for (int duty = 10; duty <= 90; duty += 2) {
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
    // mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty); // Not used
    delay(50);
  }
  for (int duty = 90; duty >= 10; duty -= 2) {
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
    // mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty); // Not used
    delay(50);
  }

  */
}
