#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Нормер выхода
const uint LED_PIN = 16;

int main()
{
    // Инициализация вывода // отдать вывод модулю GPIO
    gpio_init(LED_PIN);
    // Вывод работает на выход // задать направление «выход»
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1)
    {
        // Изменение уровня напряжения: 1 - 3.3В, 0 - 1.1В
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(1000);
    }
}