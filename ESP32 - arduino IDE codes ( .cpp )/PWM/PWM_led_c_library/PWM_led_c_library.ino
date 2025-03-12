#include <Arduino.h>
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO  21 //18           // Use GPIO 18 (adjust as needed)
#define LEDC_DUTY_RES   LEDC_TIMER_8_BIT
#define LEDC_FREQUENCY  2400000 //113000//5000         // PWM frequency in Hertz (5 kHz)

void setup() {
    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configure the LEDC channel for PWM output
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = LEDC_OUTPUT_IO,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 128,    // 50% duty cycle (range 0–255 for 8-bit resolution)
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void loop() {
    // Nothing needed here; PWM is handled by hardware.
    delay(1000);
}
