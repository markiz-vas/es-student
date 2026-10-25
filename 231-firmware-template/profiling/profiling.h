//Защита от повторного включения (англ. include guard) 
#pragma once

// заголовочные файлы для типов Pico
#include "pico/types.h"

// запоминает начальное показание часов
void profiling_init(void);

// вызывается в начале каждой итерации и учитывает прошлую
void profiling_iteration(void);

// выдача плавающего среднего
float profiling_avg_us(void);

// выдача максимального
uint32_t profiling_max_us(void);

// сброс максимального
void profiling_reset_max(void);

