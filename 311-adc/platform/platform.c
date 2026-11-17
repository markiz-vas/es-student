// заголовочный файл Модуль работы с железом (gpio_, stdio_)
#include "platform.h"
// заголовочные файлы для stdio_init_all
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint
#include "pico/stdlib.h"
// заголовочные файлы для gpio_init
// заголовочные файлы для gpio_set_dir, gpio_put 
#include "hardware/gpio.h"
// заголовочный файл для считывания напряжения (ADC)
#include "hardware/adc.h"

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
static const uint32_t BUTTON_PERIOD_PIN = 11;

// Список пинов кнопки (в дальнейшем обращение только к нему)
static uint32_t platform_buttons[BUTTONS_COUNT];


// вход АЦП: вывод GP26, канал 0
#define ADC_PIN 26
#define ADC_CHANNEL 0
// опорное напряжение АЦП на плате Pico и число уровней 12-разрядного кода
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096

// встроенный датчик температуры — канал 4
#define ADC_TEMP_CHANNEL 4



// Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
static void f_led_init(void) {
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
static void f_button_init(platform_button_t btn, uint32_t btn_pin) {
    // Защита от неверного регистра
    if (btn == BUTTONS_COUNT) return;

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

// Инициализация модуля ads (напряжение и температура)
static void f_adc_init(void) {
    // Подключаем платформу (запуск АЦП)
    adc_init();                 // сбрасывает модуль и включает его. 
    adc_gpio_init(ADC_PIN);     // отдаёт вывод аналоговому входу. 

    // ТЕОРИЯ п.3.1.1 //
    //  adc_init() сбрасывает модуль и включает его. 
    //  adc_gpio_init() отдаёт вывод аналоговому входу. 
    //  Она отключает у вывода цифровую часть и подтяжки, 
    //  которые иначе исказили бы напряжение.

    // Подключаем встроенный датчик температуры
    adc_set_temp_sensor_enabled(true);  // включить питание датчика
}

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void) {
    // обязательно для работы com-порта
    stdio_init_all();

    // Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
    f_led_init();

    // настраиваем кнопку 0
    f_button_init(BUTTON_SWITCH, BUTTON_SWITCH_PIN);
    // настраиваем кнопку 1
    f_button_init(BUTTON_PERIOD, BUTTON_PERIOD_PIN);

    // Инициализация модуля adc (напряжение и температура
    f_adc_init();
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

// возвращает номер пина led
uint32_t platform_led_pin(void) {
    return LED_PIN;
}



// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(platform_button_t btn){
    if (btn == BUTTONS_COUNT) return false; // Защита от неверного регистра
    return gpio_get(platform_buttons[btn]); // Значение регистра
}

// возвращает номер пина button
uint32_t platform_button_pin(platform_button_t btn) {
    if (btn == BUTTONS_COUNT) return 0; // Защита от неверного регистра
    return platform_buttons[btn];       // Номер пина
}


// напряжение на входе АЦП, выводе GP26, в вольтах
// функция, которая снимает один отсчёт и переводит его в вольты
float platform_voltage_read(void) {
    // Выбирает канал
    adc_select_input(ADC_CHANNEL); 
    // Запускает преобразование, ждёт его конца и возвращает 12-разрядный код от 0 до 4095.
    // Код умножают на опорное напряжение и делят на число уровней.
    // Вычисляем напряжение
    return (float)(adc_read() * ADC_VREF_V / ADC_LEVELS);

    // ТЕОРИЯ 3.1.4 //
    // adc_read() - Прошивка даёт модулю команду снять отсчёт, АЦП 2 мкс преобразует напряжение, 
    //  прошивка дожидается флага готовности и читает результат. Именно так устроена функция adc_read() из Pico SDK.
    //  Цикл ожидания делает adc_read() блокирующим кодом, и вернуть управление функция не может, пока АЦП не закончит.
    //  Нарушает ли adc_read() требование быстро возвращать управление? 
    //  Нет, потому что ожидание короче почти любой итерации суперцикла. 
    //  Около двух микросекунд прибор потратит и на вывод одной строки в порт. 
    //  Поэтому одиночный отсчёт можно брать прямо в обработчике задачи.

    // ТЕОРИЯ 3.1.3 //
    //  Двенадцать разрядов дают 4096 (ADC_LEVELS) уровней. 
    //  При диапазоне 3,3В (ADC_VREF_V) соседние уровни отстоят на 3,3 / 4096, это около 0,8 мВ. 
    //  Значит ли это, что АЦП различает напряжения с точностью до 0,8 мВ? Нет. 
    //  Производитель измерил её для RP2040 и получил около 8,7 разряда (раздел «4.9.3. ADC ENOB»). 
    //  Это значит, что из двенадцати разрядов результата сведения о сигнале несут примерно девять. 
    //  Различимых уровней остаётся около четырёхсот, и шаг между ними ближе к 8 мВ, чем к 0,8 мВ. 
    //  Неровность уровней у RP2040 к тому же собрана в нескольких точках шкалы. 
    //  Около кодов 512, 1536, 2560 и 3584 шаг заметно отличается от остальных, 
    //  и производитель описывает это как известную ошибку кристалла (RP2040-E11).
}

// возвращает номер пина adc
uint32_t platform_adc_pin(void) {
    return ADC_PIN;
}

// возвращает номер канала adc
uint32_t platform_adc_channel(void) {
    return ADC_CHANNEL;
}


// температура кристалла по встроенному датчику, градусы Цельсия
float platform_temp_read(void) {
    // Выбирает канал
    adc_select_input(ADC_TEMP_CHANNEL); 
    // Запускает преобразование, ждёт его конца и возвращает 12-разрядный код от 0 до 4095.
    // Код умножают на опорное напряжение и делят на число уровней.
    // Вычисляем напряжение
    float u = adc_read() * ADC_VREF_V / ADC_LEVELS;
    // Вычисление температуры
    return (float)(27.0f - (u - 0.706f) / 0.001721f);

    // ТЕОРИЯ 3.1.3 //
    // Пятый вход АЦП не связан ни с одним выводом. 
    // К нему подключён диод внутри кристалла, и напряжение на диоде зависит от температуры. 
    // При 27 °C оно около 0,706 В и падает примерно на 1,721 мВ на каждый градус. 
    // Температура вычисляется по формуле:
    // float t = 27.0f - (u - 0.706f) / 0.001721f;
    // u — напряжение на диоде в вольтах
}

// возвращает номер канала температуры
uint32_t platform_adc_temp_channel(void) {
    return ADC_TEMP_CHANNEL;
}



