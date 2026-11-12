// заголовочный файл задача кнопки: нажатие, переключение состояния светодиода
#include "button-task.h"
// заголовочный файл работы со временем
#include "systime/systime.h"
// заголовочный файл Модуль работы с железом (gpio_, stdio_)
#include "platform/platform.h"
// заголовочный файл состояния светодиода: выключен, горит или мигает
// #include "led-task/led-task.h"
// ради strtoul, NULL
#include <stdlib.h>


// Таблица состояний кнопки (тип)
typedef enum {
    BUTTON_STATE_OFF    ,   // 0 - отпущена
    BUTTON_STATE_OFF_END,   // 1 - нажатие?
    BUTTON_STATE_ON     ,   // 2 - нажата
    BUTTON_STATE_ON_END ,   // 3 - отпускание?
} button_state_t;

// Состояние кнопки
static button_state_t button_state;
// Ожидание для кнопки (mks, микросекунды, 10-6)
// Константа 20mc для борьбы с дребезгом
static const uint64_t debounce_us = 20 * 1000;
// Последнее время (mks, микросекунды, 10-6)
static uint64_t last_toggle_us;

// Количество нажатий кнопки
static uint32_t button_count;

// Функция, срабатывающая при нажатии на кнопку (задается в button_task_init)
button_task_callback_t f_on_press;


// Инициализация, настраивает вывод на вход
void button_task_init(button_task_callback_t on_press)
{
    // Последнее время (mks, микросекунды, 10-6)
    last_toggle_us = 0;
    // Количество нажатий кнопки
    button_count = 0;
    // Статус кнопки
    button_state = BUTTON_STATE_OFF;
    // Привязка функции
    f_on_press = on_press;
}

// Отработка задачи кнопки
void button_task_handle(void)
{
    // читаем уровень: нажатая кнопка замыкает вывод на землю и даёт ноль
    // Если кнопка нажата (false - нажата, true - отжата)
    bool state = platform_button_read();     // Считали значение
    
    // читаем часы // Замер монотонного времени
    uint64_t now_us = systime_us();

    // переходы по таблице; 
    switch (button_state) {
        case BUTTON_STATE_OFF    : { // 0 - отпущена
            if (!state) { // нажата
                // запомнить время, перейти в «нажатие?»
                last_toggle_us = now_us;
                button_state = BUTTON_STATE_OFF_END;
            }
            break;
        }
        case BUTTON_STATE_OFF_END: { // 1 - нажатие?
            if (state) { // отпущена
                // вернуться в «отпущена», это был дребезг или помеха
                button_state = BUTTON_STATE_OFF;
            } else { // нажата (!state)
                // нажата 20 мс
                if (now_us - last_toggle_us >= debounce_us) {
                    // перейти в «нажата», засчитать нажатие
                    button_state = BUTTON_STATE_ON;
                    // увеличиваем счётчик и вызываем led_task_next_state()
                    button_count++;
                    // вызываем смену состояния светодиода
                    //led_task_next_state();
                    if (f_on_press != NULL) f_on_press();
                }
            }//(!state)
            break;
        }
        case BUTTON_STATE_ON     : { // 2 - нажата
            if (state) { // отпущена
                // запомнить время, перейти в «отпускание?»
                last_toggle_us = now_us;
                button_state = BUTTON_STATE_ON_END;
            }
            break;
        }
        case BUTTON_STATE_ON_END : { // 3 - отпускание?
            if (!state) { // нажата
                // вернуться в «нажата»
                button_state = BUTTON_STATE_ON;
            } else { // отпущена (state)
                // отпущена 20 мс
                if (now_us - last_toggle_us >= debounce_us) {
                    // перейти в «отпущена»
                    button_state = BUTTON_STATE_OFF;
                }
            }//(state)
            break;
        }
    }//switch
}

// Нажата ли кнопка
// Кнопка нажата и в состоянии «нажата», и в «отпускание?»: 
// пока отпускание не подтвердилось, кнопку считают нажатой.
bool button_task_is_pressed(void) {
    // состояние 
    switch (button_state) {
        case BUTTON_STATE_OFF    : ; // 0 - отпущена
        case BUTTON_STATE_OFF_END:   // 1 - нажатие?
            return false;   // кнопка не нажата
        case BUTTON_STATE_ON     : ; // 2 - нажата
        case BUTTON_STATE_ON_END :   // 3 - отпускание?
            return true;    // кнопка нажата
    }//switch
    return false;
}

// Количество нажатий кнопки
uint32_t button_task_get_press_count(void) {
    return button_count;
}

// возвращает номер вывода кнопки
uint32_t button_pin(void) {
    return platform_button_pin();
}





