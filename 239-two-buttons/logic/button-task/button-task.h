// button-task.h // задача кнопки: нажатие, переключение состояния светодиода
//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// заголовочный файл Модуль работы с железом (gpio_, stdio_)
#include "platform/platform.h"

// функция, которую задача вызывает на каждое нажатие
typedef void (*button_task_callback_t)(void);

// Инициализация, настраивает вывод на вход
void button_task_init(platform_button_t btn, button_task_callback_t on_press);
// Отработка задачи кнопки по состоянию
void button_task_handle(platform_button_t btn);

// Нажата ли кнопка
// Кнопка нажата и в состоянии «нажата», и в «отпускание?»: 
// пока отпускание не подтвердилось, кнопку считают нажатой.
bool button_task_is_pressed(platform_button_t btn);

// Количество нажатий кнопки
uint32_t button_task_get_press_count(platform_button_t btn);

// возвращает номер вывода кнопки
uint32_t button_pin(platform_button_t btn); 