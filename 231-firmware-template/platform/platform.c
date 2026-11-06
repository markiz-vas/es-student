// заголовочный файл Модуль работы с железом (gpio_, stdio_)
#include "platform.h"
// заголовочные файлы для stdio_init_all
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint
#include "pico/stdlib.h"
// заголовочные файлы для gpio_init
// заголовочные файлы для gpio_set_dir, gpio_put 
#include "hardware/gpio.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined // pico_w //
#if defined(CYW43_WL_GPIO_LED_PIN)
#include "pico/cyw43_arch.h"
#endif


// Вывод светодиода
static const uint LED_PIN = 16;
// Вывод (вход) кнопки
static const uint BUTTON_PIN = 15;



// Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
static void led_init(void)
{
    // Perform initialisation
    // For Pico W devices we need to initialise the driver etc // pico_w //
    #if defined(CYW43_WL_GPIO_LED_PIN)  
    int rc = cyw43_arch_init();         
    hard_assert(rc == PICO_OK);         
    #endif                              

    // Initialisation GPIO
    gpio_init(LED_PIN);                 // инициализируем пин светодиода
    gpio_set_dir(LED_PIN, GPIO_OUT);    // настраиваем пин светодиода на выход
}

static void button_init(void)
{
    // Инициализация вывода     // отдать вывод модулю GPIO
    // Вывод работает на вход   // задать направление «вход»
    // Подключаем к подтягивающему регистру
    gpio_init   (BUTTON_PIN);           // инициализируем пин кнопки
    gpio_set_dir(BUTTON_PIN, GPIO_IN ); // настраиваем пин кнопки на вход
    gpio_pull_up(BUTTON_PIN);           // подтягиваем кнопку к питанию

    // ТЕОРИЯ //
    // Пока кнопка не нажата, BUTTON_PIN ни к чему не подключён —
    // на нём нет ни нуля, ни единицы, и читаться будет случайный мусор.
    // Чтобы этого не было, внутри микроконтроллера есть подтягивающий резистор:
    // он мягко притягивает вывод к питанию, и незанятый вход читается как единица
}

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void)
{
    // обязательно для работы com-порта
    stdio_init_all();

    // Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
    led_init();

    // настраиваем кнопку
    button_init();
}

// включить светодиод
void platform_led_set(bool on){
    gpio_put(LED_PIN, on);
    
    // pico_w //   
    #if defined(CYW43_WL_GPIO_LED_PIN)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !on);
    #endif
}

// включен ли светодиод
bool platform_led_get(void){
    return gpio_get(LED_PIN);
}

// возвращает номер пина
uint32_t platform_led_pin(void) {
    return LED_PIN;
}

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(void){
    return gpio_get(BUTTON_PIN);
}

// возвращает номер пина
uint32_t platform_button_pin(void) {
    return BUTTON_PIN;
}
