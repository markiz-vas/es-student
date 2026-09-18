// заголовочный файл светодиода
#include "led.h"
// заголовочные файлы для gpio_init
// заголовочные файлы для gpio_set_dir, gpio_put 
#include "hardware/gpio.h"
// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#include "pico/cyw43_arch.h"


// Нормер выхода светодиода
const uint LED_PIN = 16;

// Слово static перед переменной означает, что она видна только внутри этого файла. 
// Состояние светодиода — дело модуля, и лезть в него снаружи никто не должен; 
// спросить можно функцией led_is_on().
static bool led_state = false;

// Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
void led_init(void)
{
    // Perform initialisation
    // For Pico W devices we need to initialise the driver etc
    int rc = cyw43_arch_init();
    hard_assert(rc == PICO_OK);
    // Initialisation GPIO
    gpio_init(LED_PIN);                 // инициализируем пин светодиода
    gpio_set_dir(LED_PIN, GPIO_OUT);    // настраиваем пин светодиода на выход
    led_set(led_state);                 // задаем начальное значение светодиода
}

// выставляет уровень и запоминает состояние
void led_set(bool on)
{
    led_state = on;                 // запоминаем состояние (led_state - static)
    gpio_put(LED_PIN, led_state);   // выставляем заданный уровень
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !led_state);
}

// вызывает led_set() с обратным значением
void led_toggle(void)
{
    led_set(!led_state);
}

// возвращает состояние светодиода (выставленный уровень)
bool led_is_on(void)
{
    return led_state;
}



