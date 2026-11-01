// заголовочный файл состояния светодиода: выключен, горит или мигает
#include "led-task.h"
// Работа со светодиодом на выводе LED_PIN
//#include "led.h"
// заголовочные файлы для sleep_ms, stdio_init_all
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


// Нормер выхода светодиода
static const uint32_t LED_PIN = 16;
// Период мигания светодиода (миллисекунды)
static const uint32_t BLINK_PERIOD_MS = 1000;

// Слово static перед переменной означает, что она видна только внутри этого файла. 
// Состояние светодиода — дело модуля, и лезть в него снаружи никто не должен; 
// спросить можно функцией led_is_on().
static bool led_state_pin = true;

// Состояние задачи: внутреннее связывание, только для этого файла
// Состояние светодиода
static led_state_t led_state;
// Полупериод светодиода (mks, микросекунды, 10-6)
static uint64_t half_period_us;
// Последнее время (mks, микросекунды, 10-6)
static uint64_t last_toggle_us;


// Вспомогательная функция: из других файлов её не вызвать
// выставляет уровень и запоминает состояние
static void led_set(bool on) {
    led_state_pin = on;                 // запоминаем состояние (led_state_pin - static)
    gpio_put(LED_PIN, led_state_pin);   // выставляем заданный уровень

    // pico_w //   
    #if defined(CYW43_WL_GPIO_LED_PIN)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !led_state_pin);
    #endif
}

// вызывает led_set() с обратным значением
static void led_toggle(void) {
    led_set(!led_state_pin);
}

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
    led_set(led_state_pin);             // задаем начальное значение светодиода
}

// Инициализация, настраивает вывод на выход, устанавливает состояние светодиода
void led_task_init(void)
{
    // настраиваем вывод светодиода, период 1000 мс, состояние «мигает»
    // Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
    led_init(); 
    // Устанавливаем период
    led_task_set_period_ms(BLINK_PERIOD_MS);
    // Последний замер монотонного времени
    last_toggle_us = 0;
    // Устанавливаем состояние
    led_task_set_state(LED_STATE_BLINK);
}

// Отработка задачи светодиода по состоянию
void led_task_handle(void) {
    switch (led_state) {
        // «выключен» и «горит» — ничего не делаем, светодиод установлен при входе
        case LED_STATE_OFF: break;
        case LED_STATE_ON:  break;
        // «мигает» — каждые полпериода переключаем светодиод
        case LED_STATE_BLINK: {
            // Переключить светодиод, если прошёл полупериод мигания
            if (period_elapsed(&last_toggle_us, half_period_us)) {
                led_toggle();   // вызывает led_set() с обратным значением 
            }
            break;
        }//LED_STATE_BLINK
    }//switch
}

// Установить состояние светодиода
void led_task_set_state(led_state_t state) {
    // запоминаем состояние и выполняем действие при входе:
    // «выключен» — гасим, «горит» — зажигаем, «мигает» — первое переключение сразу
    led_state = state;

    // «выключен» и «горит» — ничего не делаем, светодиод установлен при входе
    // «мигает» — каждые полпериода переключаем светодиод
    switch (led_state) {
        case LED_STATE_OFF: {
            // Выключить светодиод
            led_set(false); 
            break;
        }//LED_STATE_OFF
        case LED_STATE_ON: {
            // Включить светодиод
            led_set(true); 
            break;
        }//LED_STATE_ON
        case LED_STATE_BLINK: {
            // Отработка состояния светодиода
            led_task_handle(); //led-task.h
            break;
        }//LED_STATE_BLINK
        // default: {
        //     // Отработка состояния светодиода
        //     led_task_handle(); //led-task.h
        //     break;
        // }//default
    }//switch
}

// Вернуть состояние светодиода
led_state_t led_task_get_state(void) {
    return led_state;
}

// Установить период светодиода в состоянии LED_STATE_BLINK
// Переключение происходит в функции led_task_handle через полупериод
// false — период не принят, период (ms, милисекунды, 10-3)
bool led_task_set_period_ms(uint32_t period_ms) {
    // С нулевым полупериодом светодиод переключался бы
    // на каждой итерации: такой период не принимается
    if (period_ms == 0) return false;
    
    // Установка полупериода в микросекундах
    half_period_us = (uint64_t)period_ms * 1000 / 2;
    return true;
}

// Возвращает период (ms, милисекунды, 10-3)
uint32_t led_task_get_period_ms(void) {
    return (uint32_t)(half_period_us * 2 / 1000);
}

// возвращает состояние светодиода (выставленный уровень)
bool led_is_on(void) {
    return led_state_pin;
}

// возвращает номер вывода светодиода
uint32_t led_pin(void) {
    return LED_PIN;
}





// Проверяет, прошло ли время period_us со времени last_us
// Времена в микросекундах
// Если Время прошло, то обновляем последнее время
// true - необходимое время прошло, false - время ещё не прошло
bool period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    // Замер монотонного времени
    uint64_t now_us = time_us_64();

    // Проверяет, прошло ли время period_us со времени last_us
    if (now_us - *last_us < period_us) {
        return false; // ранний выход, если не прошло
    }

    // Следующее срабатывание отсчитывается от прошлого, а не от «сейчас»
    *last_us += period_us;

    // Отстали больше чем на период: не догонять, а начать отсчёт заново
    if (now_us - *last_us >= period_us) {
        // сохраняем текущее время
        *last_us = now_us;
    }

    // Время прошло
    return true;
}

