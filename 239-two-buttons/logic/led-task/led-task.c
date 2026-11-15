// заголовочный файл состояния светодиода: выключен, горит или мигает
#include "led-task.h"
// заголовочный файл Модуль работы с железом (gpio_, stdio_)
#include "platform/platform.h"
// заголовочный файл работы со временем
#include "systime/systime.h"


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

// Список пинов кнопки (в дальнейшем обращение только к нему)
static uint32_t led_periods_ms[LED_PERIODS_COUNT];
// Состояние светодиода
static led_period_state_t led_period_state;


// Вспомогательная функция: из других файлов её не вызвать
// выставляет уровень и запоминает состояние
static void led_set(bool on) {
    led_state_pin = on;                 // запоминаем состояние (led_state_pin - static)
    platform_led_set(led_state_pin);    // выставляем заданный уровень // platform.h //
}

// вызывает led_set() с обратным значением
static void led_toggle(void) {
    led_set(!led_state_pin);
}

// Инициализация, настраивает вывод на выход, устанавливает состояние светодиода
void led_task_init(void)
{
    // Установить состояние периода светодиода = 0 (задан вручную)
    led_period_state = LED_PERIOD_99;
    // Задать времена периодов мигания при смене состояния
    led_periods_ms[LED_PERIOD_0] = 250 ;    // (ms, миллисекунды, 10-3)
    led_periods_ms[LED_PERIOD_1] = 500 ;    // (ms, миллисекунды, 10-3)
    led_periods_ms[LED_PERIOD_2] = 1000;    // (ms, миллисекунды, 10-3)

    // задаем начальное значение светодиода
    led_set(led_state_pin);             
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
            if (systime_period_elapsed(&last_toggle_us, half_period_us)) {
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
            led_task_handle(); 
            break;
        }//LED_STATE_BLINK
        // default: {
        //     // Отработка состояния светодиода
        //     led_task_handle(); 
        //     break;
        // }//default
    }//switch
}

// Вернуть состояние светодиода
led_state_t led_task_get_state(void) {
    return led_state;
}

// Установить следующее состояние светодиода
void led_task_next_state(void) {
    switch (led_task_get_state()) {
        case LED_STATE_OFF: {
            led_task_set_state(LED_STATE_ON); 
            break;
        }//LED_STATE_OFF
        case LED_STATE_ON: {
            led_task_set_state(LED_STATE_BLINK); 
            break;
        }//LED_STATE_ON
        case LED_STATE_BLINK: {
            led_task_set_state(LED_STATE_OFF); 
            break;
        }//LED_STATE_BLINK
    }//switch
}

// Установить следующее состояние период светодиода
void led_task_next_period(void) {
    switch (led_period_state) {
        case LED_PERIOD_0: {
            led_task_set_period_ms(led_periods_ms[LED_PERIOD_1]); // LED_PERIOD_99
            led_period_state = LED_PERIOD_1;
            break;
        }
        case LED_PERIOD_1: {
            led_task_set_period_ms(led_periods_ms[LED_PERIOD_2]); // LED_PERIOD_99
            led_period_state = LED_PERIOD_2;
            break;
        }
        case LED_PERIOD_2:;
        case LED_PERIOD_99: {
            led_task_set_period_ms(led_periods_ms[LED_PERIOD_0]); // LED_PERIOD_99
            led_period_state = LED_PERIOD_0;
            break;
        }
    }//switch
}


// Установить период светодиода в состоянии LED_STATE_BLINK
// Переключение происходит в функции led_task_handle через полупериод
// false — период не принят, период (ms, милисекунды, 10-3)
bool led_task_set_period_ms(uint32_t period_ms) {
    // С нулевым полупериодом светодиод переключался бы
    // на каждой итерации: такой период не принимается
    if (period_ms == 0) return false;
    
    // Установить состояние периода светодиода = 99 (задан вручную)
    // Если переключатель, то после функции необходимо выставить необходимое состояние
    led_period_state = LED_PERIOD_99;

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
    led_state_pin = platform_led_get();
    return led_state_pin;
}

// возвращает номер вывода светодиода
uint32_t led_pin(void) {
    return platform_led_pin();
}




