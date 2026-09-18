//#ifndef LED_H
//#define LED_H
//Защита от повторного включения (англ. include guard) 
#pragma once

// заголовочные файлы для sleep_ms, stdio_init_all
#include "pico/stdlib.h"

// Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
void led_init(void);
// выставляет уровень и запоминает состояние
void led_set(bool on);
// вызывает led_set() с обратным значением
void led_toggle(void);
// возвращает состояние светодиода (выставленный уровень)
bool led_is_on(void);

//#endif