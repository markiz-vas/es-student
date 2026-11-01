// led-task.h — задача светодиода: выключен, горит или мигает
//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Таблица состояний светодиода (тип)
typedef enum {
    LED_STATE_OFF,      // 0 - выключен
    LED_STATE_ON,       // 1 - горит
    LED_STATE_BLINK,    // 2 - мигает
} led_state_t;

// Инициализация, настраивает вывод на выход, устанавливает состояние светодиода
void led_task_init(void);
// Отработка задачи светодиода по состоянию
void led_task_handle(void);

// Установить состояние светодиода
void led_task_set_state(led_state_t state);
// Вернуть состояние светодиода
led_state_t led_task_get_state(void);

// Установить период светодиода в состоянии LED_STATE_BLINK
// Переключение происходит в функции led_task_handle через полупериод
// false — период не принят, период (ms, милисекунды, 10-3)
bool led_task_set_period_ms(uint32_t period_ms);
// Возвращает период (ms, милисекунды, 10-3)
uint32_t led_task_get_period_ms(void);

// возвращает состояние светодиода (выставленный уровень)
bool led_is_on(void);
// возвращает номер вывода светодиода
uint32_t led_pin(void);

// Проверяет, прошло ли время period_us со времени last_us
// Времена в микросекундах
// Если Время прошло, то обновляем последнее время
// true - необходимое время прошло, false - время ещё не прошло
// bool period_elapsed(uint64_t *last_us, uint64_t period_us);
