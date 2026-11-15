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
static const uint32_t LED_PIN = 16;

// Вывод (вход) кнопки переключения режима работы светодиода
static const uint32_t BUTTON_SWITCH_PIN = 15;
// Вывод (вход) кнопки переключения частоты мигания светодиода
static const uint32_t BUTTON_FREQUENCY_PIN = 11;

// Список пинов кнопки (в дальнейшем обращение только к нему)
static uint32_t platform_buttons[BUTTONS_COUNT];


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

// Запоминаем пин кнопки и настраиваем кнопку с питанием
static void button_init(platform_button_t btn, uint32_t btn_pin)
{
    // Задаём пин для вывода кнопки
    platform_buttons[btn] = btn_pin;

    // Инициализация вывода     // отдать вывод модулю GPIO
    // Вывод работает на вход   // задать направление «вход»
    // Подключаем к подтягивающему регистру
    gpio_init   (platform_buttons[btn]);          // инициализируем пин кнопки
    gpio_set_dir(platform_buttons[btn], GPIO_IN); // настраиваем пин кнопки на вход
    gpio_pull_up(platform_buttons[btn]);          // подтягиваем кнопку к питанию

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

    // настраиваем кнопку 0
    button_init(BUTTON_SWITCH   , BUTTON_SWITCH_PIN   );
    // настраиваем кнопку 1
    button_init(BUTTON_FREQUENCY, BUTTON_FREQUENCY_PIN);
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
bool platform_button_read(platform_button_t btn){
    return gpio_get(platform_buttons[btn]);
}

// возвращает номер пина
uint32_t platform_button_pin(platform_button_t btn) {
    return platform_buttons[btn];
}
