// заголовочный файл задача кнопки: нажатие, переключение состояния светодиода
#include "button-task.h"
// заголовочный файл состояния светодиода: выключен, горит или мигает
#include "led-task.h"
// заголовочные файлы для stdio_init_all
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint
#include "pico/stdlib.h"
// заголовочные файлы для gpio_init
// заголовочные файлы для gpio_set_dir, gpio_put 
#include "hardware/gpio.h"

// Нормер выхода
static const uint BUTTON_PIN = 15;

// Константа 20mc для борьбы с дребезгом
static const uint DEBOUNCE_MS = 20;

// Состояние кнопки
static button_state_t button_state;
// Ожидание для кнопки (mks, микросекунды, 10-6)
static uint64_t debounce_us = DEBOUNCE_MS * 1000;
// Последнее время (mks, микросекунды, 10-6)
static uint64_t last_toggle_us;

// Количество нажатий кнопки
static uint32_t button_count;


// Инициализация, настраивает вывод на вход
void button_task_init(void) {
    // Инициализация вывода     // отдать вывод модулю GPIO
    // Вывод работает на вход   // задать направление «вход»
    // Подключаем к подтягивающему регистру
    gpio_init   (BUTTON_PIN);           // инициализируем пин кнопки
    gpio_set_dir(BUTTON_PIN, GPIO_IN ); // настраиваем пин кнопки на вход
    gpio_pull_up(BUTTON_PIN);           // подтягиваем кнопку к питанию

    // ТЕОРИЯ //
    // Пока кнопка не нажата, BUTTON_PIN ни к чему не подключён —
    // на нём нет ни нуля, ни единицы, и читаться будет случайный мусор.
    // Чтобы этого не было, внутри микроконтроллера есть подтягивающий резистор:
    // он мягко притягивает вывод к питанию, и незанятый вход читается как единица

    // Последнее время (mks, микросекунды, 10-6)
    last_toggle_us = 0;
    // Количество нажатий кнопки
    button_count = 0;
    // Статус кнопки
    button_state = BUTTON_STATE_OFF;
}

// Отработка задачи кнопки
void button_task_handle(void)
{
    // читаем уровень: нажатая кнопка замыкает вывод на землю и даёт ноль
    // Если кнопка нажата (false - нажата, true - отжата)
    bool state = gpio_get(BUTTON_PIN);     // Считали значение
    
    // читаем часы // Замер монотонного времени
    uint64_t now_us = time_us_64();

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
                    led_task_next_state();
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
    return BUTTON_PIN;
}



// // Отработка задачи кнопки
// void button_task_handle(void) {
//     // Проверяйте не состояние, а его смену. Суперцикл крутится тысячи раз в секунду.
//     // Если переключать светодиод, пока кнопка нажата, он будет мигать всё время нажатия.
//     // Переключать нужно в момент, когда вывод перешёл из единицы в ноль, то есть когда кнопку нажали:
//     // bool current = gpio_get(BUTTON_PIN);
//     // Замена прямого чтения вывода (gpio_get) вызовом функции (get_but_||ton_debounce)
//     bool current = get_button_debounce(BUTTON_PIN); // читаем состояние пина кнопки с задержкой
//     // Если кнопка нажата (false - нажата, true - отжата)
//     if (previous == true && current == false)       // проверяем, нужно ли переключить светодиод
//     {
//         // Переключить светодиод
//         led_toggle();
//         // Вывод сообщения через кабель usb в виртуальный COM-port
//         LOG_INF("led %s\n", led_is_on() ? "on" : "off");
//     }
//     // Запоминаем, что нажато было раньше (чтобы не мигала)
//     previous = current;     // запоминаем текущее состояние пина кнопки, как предыдущее
// }



