// button-task.h // задача кнопки: нажатие, переключение состояния светодиода
//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Инициализация, настраивает вывод на вход
void button_task_init(void);
// Отработка задачи кнопки по состоянию
void button_task_handle(void);

// Нажата ли кнопка
// Кнопка нажата и в состоянии «нажата», и в «отпускание?»: 
// пока отпускание не подтвердилось, кнопку считают нажатой.
bool button_task_is_pressed(void);

// Количество нажатий кнопки
uint32_t button_task_get_press_count(void);

// возвращает номер вывода кнопки
uint32_t button_pin(void); 