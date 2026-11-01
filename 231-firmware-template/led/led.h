// led.h — Работа со светодиодом на выводе LED_PIN
//#ifndef LED_H
//#define LED_H
//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
void led_init(void);
// выставляет уровень и запоминает состояние
void led_set(bool on);
// вызывает led_set() с обратным значением
void led_toggle(void);
// возвращает состояние светодиода (выставленный уровень)
bool led_is_on(void);
// возвращает номер вывода светодиода
uint32_t led_pin(void);

//#endif