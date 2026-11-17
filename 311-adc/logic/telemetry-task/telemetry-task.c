// заголовочный файл задачи телеметрии
#include "telemetry-task.h"
// заголовочный файл Модуль работы с железом (gpio_, stdio_)
#include "platform/platform.h"
// заголовочный файл работы со временем
#include "systime/systime.h"
// ради strtoul, NULL
#include <stdlib.h>

// Состояние задачи: внутреннее связывание, только для этого файла
// Состояние телеметрии
static telemetry_state_t telemetry_state;
// Период телеметрии (mks, микросекунды, 10-6)
static uint64_t period_us;
// Старт время телеметрии (mks, микросекунды, 10-6)
static uint64_t start_toggle_us;
// Последнее время (mks, микросекунды, 10-6)
static uint64_t last_toggle_us;

// Данные телеметрии (последний замер)
static telemetry_record_t telemetry_record;

// Функция, срабатывающая при телеметрии (задается в telemetry_task_init)
static telemetry_task_callback_t f_on_record;


// Инициализация, привязка функций обратного вызова
void telemetry_task_init(telemetry_task_callback_t on_record) {
    // задаем данные телеметрии
    telemetry_record.time_us   = 0;
    telemetry_record.voltage_v = 0;
    telemetry_record.temp_c    = 0;
    // Устанавливаем период
    period_us = 100 * 1000;
    // Стартовый замер монотонного времени
    start_toggle_us = 0;
    // Последний замер монотонного времени
    last_toggle_us = 0;
    // Устанавливаем состояние
    telemetry_state = TELEMETRY_STATE_IDLE;
    // Привязка функции обработчика телеметрии
    f_on_record = on_record;
}

// Отработка задачи телеметрии по состоянию
void telemetry_task_handle(void) {
    switch (telemetry_state) {
        // Телеметрия выключена
        case TELEMETRY_STATE_IDLE:  break;
        // Телеметрия включена
        case TELEMETRY_STATE_RUN: {
            // Отправить телеметрию, если прошёл период
            if (systime_period_elapsed(&last_toggle_us, period_us)) {
                // Замер монотонного времени
                uint64_t now_us = systime_us(); 
                // Время прошедшее со Старта
                telemetry_record.time_us   = now_us - start_toggle_us;
                // Напряжение
                telemetry_record.voltage_v = platform_voltage_read();
                // Температура
                telemetry_record.temp_c    = platform_temp_read();
                // Обратный вызов функции обработчика телеметрии
                if (f_on_record != NULL) f_on_record(&telemetry_record);   
            }
            break;
        }
    }//switch
}

// Запускаем задачу телеметрии (состоянии TELEMETRY_STATE_RUN)
// и установить период телеметрии (если период не установлен, то задача не запущена)
// Период должен быть больше 10 ms (иначе false)
// false — период не принят, период (ms, милисекунды, 10-3)
bool telemetry_task_start(uint32_t period_ms) {
    // Проверка периода (необходим для запуска)
    if (period_ms < 10) return false;

    // Сохраняем период (mks, микросекунды, 10-6)
    period_us = 1000 * period_ms;
    // Последнее время замера (монотонное время)
    last_toggle_us = systime_us();
    // Запоминаем монотонное время начала телеметрии (если телеметрия не была запущена)
    if (telemetry_state == TELEMETRY_STATE_IDLE)
        start_toggle_us = last_toggle_us;
    // Состояние - замер телеметрии
    telemetry_state = TELEMETRY_STATE_RUN;
    // Телеметрия началась
    return true;
}

// Останавливаем задачу телеметрии (состоянии TELEMETRY_STATE_IDLE)
void telemetry_task_stop(void) {
    // Состояние - СТОП телеметрии
    telemetry_state = TELEMETRY_STATE_IDLE;
}

// Возвращает состояние телеметрии
telemetry_state_t telemetry_task_get_state(void) {
    return telemetry_state;
}

// Возвращает период (ms, милисекунды, 10-3)
uint32_t telemetry_task_get_period_ms(void) {
    return period_us / 1000;
}





