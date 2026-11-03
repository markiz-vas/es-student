// pi-task.h // Заголовочный файл задачи вычисления π (пи) с прерыванием на другие процессы
//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа bool
#include <stdbool.h>
// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Инициализация, задачи вычисления π
void pi_task_init(void);
// Отработка задачи вычисления π
void pi_task_handle(void);

// Запустить вычисление (если вычисление идёт - вернуть false)
bool pi_task_start(uint32_t terms);

// Возвращает значение и время, потраченное на расчет (false - расчет на начат)
// pi = 0, t = 0 - идут вычисления (либо промежуточные значения, либо "занят")
bool pi_task_get_result(double *pi, uint32_t *t);

// Возвращает точность вычислений по умолчанию
uint32_t pi_task_calc_terms(void);
