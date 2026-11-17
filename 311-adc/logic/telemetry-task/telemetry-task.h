// telemetry-task.h // задача телеметрии
//  с заданным периодом снимает напряжение и температуру и отдаёт запись функции, переданной при запуске.
// Как запись попадёт на компьютер, задача не знает.

//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Таблица состояний телеметрии (тип)
typedef enum {
    TELEMETRY_STATE_IDLE,   // 0 - ждем (ничего не делаем)
    TELEMETRY_STATE_RUN,    // 1 - снимаем показания
} telemetry_state_t;


// одна запись: время отсчёта по часам прибора и измеренные значения
typedef struct {
    uint64_t time_us  ;     // Время (mks, микросекунды, 10-6)
    float    voltage_v;     // Напряжение в вольтах
    float    temp_c   ;     // Температура в градусах цельсия
} telemetry_record_t;

// тип-шаблон функции, которой задача отдаёт каждую запись
typedef void (*telemetry_task_callback_t)(const telemetry_record_t *record);

// Инициализация, привязка функций обратного вызова
void telemetry_task_init(telemetry_task_callback_t on_record);
// Отработка задачи телеметрии по состоянию
void telemetry_task_handle(void);


// Запускаем задачу телеметрии (состоянии TELEMETRY_STATE_RUN)
// и установить период телеметрии (если период не установлен, то задача не запущена)
// Период должен быть больше 10 ms
// false — период не принят, период (ms, милисекунды, 10-3)
bool telemetry_task_start(uint32_t period_ms);
// Останавливаем задачу телеметрии (состоянии TELEMETRY_STATE_IDLE)
void telemetry_task_stop(void);

// Возвращает состояние телеметрии
telemetry_state_t telemetry_task_get_state(void);
// Возвращает период (ms, милисекунды, 10-3)
uint32_t telemetry_task_get_period_ms(void);



// Проверяет, прошло ли время period_us со времени last_us
// Времена в микросекундах
// Если Время прошло, то обновляем последнее время
// true - необходимое время прошло, false - время ещё не прошло
// bool period_elapsed(uint64_t *last_us, uint64_t period_us);
