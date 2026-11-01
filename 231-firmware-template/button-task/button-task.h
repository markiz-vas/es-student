// button-task.h — задача кнопки: нажатие, переключение состояния светодиода
//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Таблица состояний кнопки (тип)
typedef enum {
    BUTTON_STATE_OFF    ,   // 0 - отпущена
    BUTTON_STATE_OFF_END,   // 1 - нажатие?
    BUTTON_STATE_ON     ,   // 2 - нажата
    BUTTON_STATE_ON_END ,   // 3 - отпускание?
} button_state_t;

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