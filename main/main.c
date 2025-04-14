#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_GPIO GPIO_NUM_14

// GPIO numbers for 5 LEDs (modify as needed)
gpio_num_t led_gpios[5] = {
    GPIO_NUM_32,
    GPIO_NUM_33,
    GPIO_NUM_25,
    GPIO_NUM_26,
    GPIO_NUM_27,
};

void app_main(void)
{
    // Button configuration
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&btn_conf);

    // LED configuration
    for (int i = 0; i < 5; i++) {
        gpio_reset_pin(led_gpios[i]);
        gpio_set_direction(led_gpios[i], GPIO_MODE_OUTPUT);
        gpio_set_level(led_gpios[i], 0); // Initially OFF
    }

    int last_button_state = 1;
    int current_led = -1; // No LED is ON at the beginning

    while (1) {
        int btn = gpio_get_level(BUTTON_GPIO);

        if (btn == 0 && last_button_state == 1) {
            // Turn OFF the previous LED
            if (current_led >= 0) {
                gpio_set_level(led_gpios[current_led], 0);
            }

            // Move to the next LED (wrap around from 4 to 0)
            current_led = (current_led + 1) % 5;

            // Turn ON the new LED
            gpio_set_level(led_gpios[current_led], 1);

            // Debounce and prevent multiple triggers during long press
            while (gpio_get_level(BUTTON_GPIO) == 0) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            vTaskDelay(pdMS_TO_TICKS(50)); // Additional delay
        }

        last_button_state = btn;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
