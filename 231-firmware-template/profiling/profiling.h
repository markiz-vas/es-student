// profiling.h // заголовочный файл модуля профилирования (время суперцикла)
//Защита от повторного включения (англ. include guard) 
#pragma once

// // заголовочные файлы для типов Pico
// #include "pico/types.h"
// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// число секундомеров (заполняются в main.c), номера с [1-5]
// Секундомер под номером 0 забронирован под глобальный секундомер (суперцикл)
#define STOPWATCH_QTY 5

// Структура Секундомер (stopwatch)
typedef struct {
    char    *name       ; // имя секундомера
    uint32_t count      ; // количество замеров 
    uint32_t avg_sum    ; // плавающее среднее время // сумма замеров одного секундомера переполнится через 71 минуту
    uint32_t max_us     ; // максимальное время замера (mks, микросекунды, 10-6)
    uint32_t start_us   ; // начальный замер (mks, микросекунды, 10-6)
    uint32_t id         ; // номер секундомера (определяется при создании секундомера)
} profiling_result_t;

// Создать секундомер
void profiling_stopwatch_init(uint32_t id, const char *name);

// для всех секундомеров, id должен быть найден в списке
// false — секундомер с таким номером не заведён

// Запустить замер секундомера
bool profiling_start(uint32_t id);
// Отчет секундомера
bool profiling_stop(uint32_t id);
// Вернуть значения секундомера
bool profiling_get(uint32_t id, profiling_result_t **result);
// Сбросить все секундомеры
void profiling_reset(void);
// Сбросить выбранный секундомер
bool profiling_reset_id(uint32_t id);




// // запоминает начальное показание часов
// void profiling_global_init(void);

// // вызывается в начале каждой итерации и учитывает прошлую
// void profiling_global_iteration(void);

// // выдача плавающего среднего
// float profiling_global_avg_us(void);

// // выдача максимального
// uint32_t profiling_global_max_us(void);

// // сброс максимального
// void profiling_global_reset_max(void);
