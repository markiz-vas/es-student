#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#include "pico/cyw43_arch.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

//varALL// // Нормер выхода
const uint LED_PIN = 16;
const uint BUTTON_PIN = 15;

// Константа 20mc для борьбы с дребезгом
const uint DEBOUNCE_MS = 20;

// Функция определяет, был ли дребезг
// Считывает два значение, возвращает true, если оба true
bool get_button_debounce(uint pin)
{
    bool state = gpio_get(pin);     // Считали первое значение
    sleep_ms(DEBOUNCE_MS);          // подождали DEBOUNCE_MS
    return state && gpio_get(pin);  // Считали второе значение
}



int main()
{
    gpio_init(LED_PIN);                 // инициализируем пин светодиода
    gpio_set_dir(LED_PIN,    GPIO_OUT); // настраиваем пин светодиода на выход
        
    gpio_init(BUTTON_PIN);              // инициализируем пин кнопки
    gpio_set_dir(BUTTON_PIN, GPIO_IN ); // настраиваем пин кнопки на вход
    gpio_pull_up(BUTTON_PIN);           // подтягиваем кнопку к питанию


    // led - светит ли светодиод, 
    // previous - что вывод показывал на прошлом шаге цикла
    bool led = false;
    bool previous = false;

    // Perform initialisation
    // For Pico W devices we need to initialise the driver etc
    int rc = cyw43_arch_init();
    hard_assert(rc == PICO_OK);

    // Бесконечный цикл (Суперцикл) - это обязательное условие,
    // Так как все программы запускаются на Земле 1 раз
    while (1)
    {
        // Проверяйте не состояние, а его смену. Суперцикл крутится тысячи раз в секунду. 
        // Если переключать светодиод, пока кнопка нажата, он будет мигать всё время нажатия. 
        // Переключать нужно в момент, когда вывод перешёл из единицы в ноль, то есть когда кнопку нажали:
        // bool current = gpio_|_get(BUTTON_PIN);

        // Замена прямого чтения вывода (gpio_|_get) вызовом функции (get_button_debounce)
        bool current = get_button_debounce(BUTTON_PIN); // читаем состояние пина кнопки с задержкой
        
        // Если кнопка нажата (false - нажата, true - отжата)
        if (previous == true && current == false)       // проверяем, нужно ли переключить светодиод
        {
            // Переключить светодиод
            led = !led; 
        }
        // Запоминаем, что нажато было раньше (чтобы не мигала)
        previous = current;     // запоминаем текущее состояние пина кнопки, как предыдущее

        // Моргание на встроенном светодиоде или на внешнем
        if (false) {
            if (led) {
                gpio_put(LED_PIN, 1);
                sleep_ms(LED_DELAY_MS);
                gpio_put(LED_PIN, 0);
                sleep_ms(LED_DELAY_MS);
                gpio_put(LED_PIN, 1);
                sleep_ms(LED_DELAY_MS);
                gpio_put(LED_PIN, 0);
                sleep_ms(LED_DELAY_MS*4);
            } else {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
                sleep_ms(LED_DELAY_MS);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
                sleep_ms(LED_DELAY_MS);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
                sleep_ms(LED_DELAY_MS);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            sleep_ms(LED_DELAY_MS*4);
            }   
        }// выбор моргаем на плате или по порту LED_PIN

        // Горит светодиод либо на плате, било на порту LED_PIN
        if (true) {
            gpio_put(LED_PIN, led);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !led);
        }
    }
}