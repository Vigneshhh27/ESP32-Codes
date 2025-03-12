#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define PIR_PIN GPIO_NUM_36 // Change this to your connected GPIO pin

//static const char *TAG = "PIR_Sensor";



/*// ISR to handle the PIT motion detection
void IRAM_ATTR PIR_ISR_handler(void* arg) {
    if (Buck5VPIR == 1 && buck12to5 ==1){

   
    static uint32_t last_PIR_ISR_time = 0;
    uint32_t PIR_ISR_time = xTaskGetTickCountFromISR();

   
    if (PIR_ISR_time - last_PIR_ISR_time > pdMS_TO_TICKS(200)) {
        printf("motion Detected not going to deep sleep");
    }
    last_PIR_ISR_time = PIR_ISR_time;
     }
}





// Function to initialize GPIO for the button
void button_init() {
    
    //  (PIR_PIN)
    io_conf.intr_type = GPIO_INTR_POSEDGE;  // Interrupt on rising edge when motion is deteced HIGH/1
    io_conf.pin_bit_mask = (1ULL << PIR_PIN);  // Set pin of PIR
    io_conf.mode = GPIO_MODE_INPUT;  // Set as input
    io_conf.pull_up_en = 0;  // Enable pull-down resistor
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
          

    

    // Install ISR service and add ISR handler 
    gpio_install_isr_service(0);
    
    gpio_isr_handler_add(PIR_PIN, PIR_ISR_handler, NULL);  // Second button ISR



    
                
}*/


void app_main(void) {
    // Configure the PIR sensor pin as input
    esp_rom_gpio_pad_select_gpio(PIR_PIN);
    gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT);
    int sec=0;
    while (1) {
        // Read the digital value from the PIR sensor
        int pir_value = gpio_get_level(PIR_PIN);
         printf("VALUE= %d ,," , pir_value);
        // Check if motion is detected
        if (pir_value == 1) {
           // ESP_LOGI(TAG, "Motion detected!");
            printf("Motion detected! ,, ");
        } else {
           // ESP_LOGI(TAG, "No motion.");
             printf("No motion. ,,");
        }

        
        sec++;
        printf("secs = %d \n", sec);
        vTaskDelay(100 ); // Adjust the delay as needed



            int pir_value = gpio_get_level(PIR_PIN);
            printf("VALUE= %d ,," , pir_value);
             if (pir_value == 1) {
           
            printf("Motion detected! ,, "); 
        } else {
           
             printf("No motion. ,,");  
        }
        

        }

}

