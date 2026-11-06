// Заголовочный файл задачи вычисления π (пи) с прерыванием на другие процессы
#include "pi-task.h"
// заголовочные файлы для stdio_init_all
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint 
#include "pico/stdlib.h"
// заголовочный файл работы со временем
#include "systime/systime.h"

// calc_pi // TASK 2.2.3 //
// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с (4,68 с)
// Значение по умолчанию, если команда запуска без второго аргумента
static const uint32_t CALC_PI_TERMS = 1000000;

// 1 расчет - 585 тактов при 125 МГц — около 4,7 мкс (10-6)
// 100 расчетов - около 0,47 мс (10-3)
static const uint32_t PI_TERMS_PER_CALL = 100;


// Таблица состояний вычисления π
typedef enum {
    PI_STATE_NO     ,   // 0 - Вычислений не было       // не запускался
    PI_STATE_COUNT  ,   // 1 - Идут вычисления          // считает
    PI_STATE_RESULT ,   // 2 - Вычисление закончились   // готово
} pi_state_t;

// Состояние вычисления π
static pi_state_t pi_state;


// // ТЕОРИЯ // TASK 2.2.3 //
// // Без слова 'volatile' компилятор вправе перенести расчёт за второе чтение часов. 
// //  Результат от этого не изменится, но замер измерит пустое место. 
// //  Запись в volatile компилятор не переносит, и расчёт остаётся между двумя чтениями. 
static volatile double pi_result;
// Значение π (пи) 
// static double pi_result;
// Старт вычисления (mks, микросекунды, 10-6)
static uint64_t start_us;
// Время вычисления (mks, микросекунды, 10-6)
static uint64_t calc_us;

// Текущее значение итераций
static uint32_t pi_terms;
// Необходимое количество итераций
static uint32_t pi_terms_end;
// Переменные для вычислений
static double pi_sum;
static double pi_sign;

// Возврат промежуточных значений командой 'pi', иначе "Занят"
static bool isCountResult = false;

// Инициализация, настраивает вывод на вход
void pi_task_init(void) {
    // Установка начальных значений
    pi_state    = PI_STATE_NO;
    pi_result   = 0; // вычисляется в pi_task_handle
    start_us    = 0; // устанавливается в pi_task_start
    calc_us     = 0; // меняется в pi_task_handle
    pi_terms    = 0; // меняется в pi_task_handle
    pi_terms_end= 0; // устанавливается в pi_task_start
    pi_sum      = 0; // вычисляется в pi_task_handle
    pi_sign     = 1; // вычисляется в pi_task_handle
}

// Отработка задачи кнопки
void pi_task_handle(void)
{
    // переходы по таблице; 
    switch (pi_state) {
        // Ничего не делаем
        case PI_STATE_NO:     break;
        case PI_STATE_RESULT: break;
        // Порционное вычисление, запоминание времени
        case PI_STATE_COUNT: {
            // Границы цикла
            uint32_t end = pi_terms + PI_TERMS_PER_CALL;
            if (end > pi_terms_end) end = pi_terms_end; // Вычисления подошли к концу
            if (end < pi_terms)     end = pi_terms_end; // Переполнение, больше 4 294 967 295
            
            // Цикл с порционным вычислением
            while (pi_terms < end) {
                // Вычисление
                pi_sum +=  pi_sign / (2.0 * pi_terms + 1.0);
                pi_sign = -pi_sign;
                // Увеличения счетчика итераций
                pi_terms++;
            }//while

            // Возврат промежуточных значений
            if (isCountResult) {
                // Промежуточное (итоговое) сохранение результата
                pi_result = 4.0 * pi_sum;
                // Сохраняем время
                calc_us = systime_us(); // Замер монотонного времени
            }

            // Вычисления закончились    
            if (pi_terms == pi_terms_end) {
                // Промежуточное (итоговое) сохранение результата
                pi_result = 4.0 * pi_sum;
                // Сохраняем время
                calc_us = systime_us(); // Замер монотонного времени
                // Конец вычислений
                pi_state = PI_STATE_RESULT;
            }
            break;
        }
    }//switch
}


// Запустить вычисление (если вычисление идёт - вернуть false)
bool pi_task_start(uint32_t terms) {
    // Вторые вычисления не начинаются, пока не закончились предыдущие
    if (pi_state == PI_STATE_COUNT) return false;

    // Начальные значения для вычисления
    pi_task_init();
    // Количество шагов вычисления
    pi_terms_end = terms;

    // Запомнить время вычисления
    start_us = systime_us(); // Замер монотонного времени
    calc_us  = start_us;

    // Запустить вычисление
    pi_state = PI_STATE_COUNT;
    return true;
}

// Возвращает значение и время, потраченное на расчет (false - расчет на начат)
// pi = 0, t = 0 - идут вычисления (либо промежуточные значения, либо "занят")
bool pi_task_get_result(double *pi, uint32_t *t) {
    // переходы по таблице; 
    switch (pi_state) {
        case PI_STATE_NO: return false;
        case PI_STATE_COUNT: { // Занят
            if (!isCountResult) if (pi) *pi = 0;
            if (!isCountResult) if (t ) *t  = 0;
            if (!isCountResult) return true;
        }
        case PI_STATE_RESULT: {
            // Значение пи
            if (pi) *pi = pi_result;
            // Сремя с момента начала вычисления
            if (t ) *t  = (uint32_t)((calc_us - start_us)/1000);
            return true;
        }
    }//switch
    return false;
}

// Возвращает точность вычислений по умолчанию
uint32_t pi_task_calc_terms(void) {
    return CALC_PI_TERMS;
}
