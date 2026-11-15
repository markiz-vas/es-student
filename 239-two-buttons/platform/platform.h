// platform.h // Модуль работы с железом (gpio_, stdio_)
//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

// включить светодиод
void platform_led_set(bool on);
// включен ли светодиод
bool platform_led_get(void);
// возвращает номер пина
uint32_t platform_led_pin(void);

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(void);
// возвращает номер пина
uint32_t platform_button_pin(void);
