#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#include "pico/cyw43_arch.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

const uint LED_PIN = 16;

int main()
{
    // Perform initialisation
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Perform initialisation
    // For Pico W devices we need to initialise the driver etc
    int rc = cyw43_arch_init();
    hard_assert(rc == PICO_OK);
    while (1)
    {
        // Just set the GPIO on or off
        gpio_put(LED_PIN, 1); 
        // Ask the wifi "driver" to set the GPIO on or off
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(LED_DELAY_MS);
        gpio_put(LED_PIN, 0);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(LED_DELAY_MS);
        gpio_put(LED_PIN, 1);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(LED_DELAY_MS);
        gpio_put(LED_PIN, 0);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(LED_DELAY_MS*4);
    }
}