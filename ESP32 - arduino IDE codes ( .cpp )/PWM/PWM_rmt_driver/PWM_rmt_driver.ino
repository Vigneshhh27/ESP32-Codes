#include <Arduino.h>
#include "driver/rmt.h"

// ---------- USER SETTINGS ----------
static const rmt_channel_t RMT_CHANNEL = RMT_CHANNEL_0; // Use channel 0 (range: 0–7)
static const gpio_num_t    RMT_PIN     = GPIO_NUM_21;   // GPIO pin for output
static const uint32_t      DESIRED_FREQ = 2400000;// 113000;       // Target frequency (113 kHz)
// -----------------------------------

// We use the 80 MHz APB clock with divider=1 => 12.5 ns per RMT tick.
// For a 113 kHz square wave, the period is ~8.85 µs; half-period ~4.425 µs.
// Half-period in ticks = 4.425e-6 / 12.5e-9 = ~354 ticks.
void setup() {
  Serial.begin(115200);
  delay(100);

  // Calculate the half-period in RMT clock ticks.
  // We toggle the pin once per half-period => one RMT item can represent one full period
  // by encoding “high for halfPeriodTicks, then low for halfPeriodTicks”.
  uint32_t halfPeriodTicks = (uint32_t)(40000000ULL / DESIRED_FREQ);
  // Explanation: We have an 80 MHz clock, but each full cycle is 2 toggles => 80e6/2 = 40e6.

  // 1) Configure the RMT peripheral
  rmt_config_t config = {
    .rmt_mode       = RMT_MODE_TX,        // Transmit mode
    .channel        = RMT_CHANNEL,
    .gpio_num       = RMT_PIN,
    .clk_div        = 1,                  // Divide 80 MHz by 1 => 80 MHz RMT clock
    .mem_block_num  = 1,                  // One memory block is enough for a small pattern
    .flags          = 0
  };
  rmt_config(&config);
  rmt_driver_install(RMT_CHANNEL, 0, 0);

  // 2) Prepare one RMT “item” that represents one full cycle:
  //    - HIGH for halfPeriodTicks
  //    - LOW  for halfPeriodTicks
  rmt_item32_t waveItem;
  waveItem.level0    = 1;                       // First level is HIGH
  waveItem.duration0 = halfPeriodTicks;         // for halfPeriodTicks
  waveItem.level1    = 0;                       // Then LOW
  waveItem.duration1 = halfPeriodTicks;         // for halfPeriodTicks

  // 3) Write the item and enable “loop” mode to repeat forever
  rmt_write_items(RMT_CHANNEL, &waveItem, 1, true);
  rmt_set_tx_loop_mode(RMT_CHANNEL, true);

  Serial.print("RMT-based square wave started on GPIO ");
  Serial.print((int)RMT_PIN);
  Serial.print(" at ~");
  Serial.print(DESIRED_FREQ);
  Serial.println(" Hz");
}

void loop() {
  // Nothing else needed; the RMT hardware handles the waveform generation.
  delay(1000);
}
