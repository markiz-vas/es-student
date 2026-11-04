// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// ради функции isdigit()
#include <ctype.h>
// ради errno и ERANGE
#include <errno.h>
// ради strtoul
#include <stdlib.h>

// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint, time_us_64()
#include "pico/stdlib.h"
// заголовочный файл протокола команд
#include "api-commands.h"
// заголовочный файл лога информации о приборе #include "device\device.h"
#include "device.h"
// заголовочный файл модуля профилирования (время суперцикла)
#include "profiling.h"
// заголовочный файл состояния светодиода: выключен, горит или мигает
#include "led-task.h"
// заголовочный файл задача кнопки: нажатие, переключение состояния светодиода
#include "button-task.h"
// Заголовочный файл задачи вычисления π (пи) с прерыванием на другие процессы
#include "pi-task.h"


// Число без знака из десятичной записи. false — в записи не только цифры
// или число не помещается в uint32_t.
static bool parse_u32(const char *text, uint32_t *value)
{
    // strtoul приняла бы и знак: из "-1" получилось бы 4294967295
    if (!isdigit((unsigned char)text[0])) return false;

    char *end;  // Сюда возвращается символ после числа
    errno = 0;  // #include <errno.h> // Если число не поместилось в тип
    unsigned long number = strtoul(text, &end, 10); // #include <stdlib.h> // Перевод в число

    // после числа остались символы или число не поместилось
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX) return false;

    // Приведение к типу число и возврат его
    *value = (uint32_t)number;
    // Это число, всё хорошо
    return true;

    /// ТЕОРИЯ ///
    // Запись числа обработчик принимает при трёх условиях:
    //      запись начинается с цифры;
    //      после цифр в строке ничего нет;
    //      число помещается в uint32_t.

    // Если хотя бы одно условие нарушено, прибор отвечает отказом с подсказкой и период не меняет. 
    // Так должны заканчиваться led_period без аргумента, led_period 5x, led_period -1 и led_period 99999999999.

    // Число из строки читает стандартная функция strtoul(). 
    //  Через второй параметр она сообщает, где кончилось число, 
    //  поэтому второе условие проверить легко. 
    //  Первое и третье она не проверяет. 
    //  Знак минус strtoul() принимает и обращает результат по правилам беззнакового типа: 
    //  из -1 получится 4294967295, и прибор согласится мигать с периодом почти в 50 суток. 
    //  Число, которое не поместилось в тип, она заменяет наибольшим значением 
    //  и сообщает об этом через переменную errno. 
    //  Обе проверки приходится делать самим.
}




/// CALC_PI /// SATART ///
// Старт вычисления  π 
// аргумент команды не обязателен! Должен быть больше 0! И Числом до 4 294 967 295
static void cmd_pi_start(const command_t *command) {
    uint32_t members_qty;   // Сюда возвращается число
    // Проверка на число и перевод строки в число
    bool isNumber = parse_u32(command->argv[0], &members_qty);

    // Это число? или введена только команда calc_pi (argc = 1)
    if (isNumber || command->argc == 1) {
        // Значение по умолчанию (аргумент не задан)
        if (command->argc == 1) members_qty = pi_task_calc_terms();
        // Число больше нуля?
        if (members_qty > 0) {
            // Запуск вычисления π 
            bool isRes = pi_task_start(members_qty);

            // Вывод информации о старте или ошибки, если вычисление уже идёт            
            if (isRes)  printf("pi: started, %u terms\n", (unsigned)members_qty);
            else        printf("error: pi is busy\n");
        } else {
            // Точность должна быть больше нуля
            printf("error: terms must be gerater than 0\n");
        }
    } else {
        // Второй аргумент должен быть числом (число больше нуля)
        printf("error: usage pi_start [%s]\n", command->argv[0]);
        // printf("error: usage pi_start [terms]\n");
    }
}

// печатает значение π и время расчёта в миллисекундах
static void cmd_pi(const command_t *command) {
    // Получаемые значения
    double   pi   = 0; 
    uint32_t time = 0;
    // получение результата
    bool isRes = pi_task_get_result(&pi, &time);

    // Вычисления не начинались
    if (!isRes) {
        printf("pi: not started\n");
        return; // ранний выход
    }

    // Идут вычисления
    if ((pi == 0) && (time == 0)) {
        printf("pi: running\n");
        return; // ранний выход
    }

    // Вывод результатов вычисления
    printf("pi: %.9f in %u ms\n", pi, (unsigned)time);
}
/// CALC_PI /// END ///

/// DEVICE /// START ///
/// DEVICE /// Информация о приборе ///
// печатаем паспорт устройства
static void device_info(void) {
    // Получаем информацию о приборе
    device_info_t info;
    device_get_info(&info);

    // Печать
    printf("board: %s\n"    ,   info.board      );
    printf("serial: %s\n"   ,   info.board_id   );
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", 
            (unsigned)info.manufacturer, (unsigned)info.part, (unsigned)info.revision);
    printf("pico-sdk: %s\n" ,   info.pico_sdk   );
}

// печатаем паспорт проекта
static void project_info(void) {
    // Получаем информацию о проекте
    project_info_t info;
    project_get_info(&info);

    // Печать
    printf("name: %s\n"         , info.device_name      );
    printf("version: %s\n"      , info.firmware_version );
    printf("project: %s\n"      , info.device_project   );
    printf("repo: %s\n"         , info.device_repo      );
    printf("TASK: %s\n"         , info.task_info        );
    printf("date_build: %s\n"   , info.date_build_info  );
    printf("time_build: %s\n"   , info.time_build_info  );
    printf("log_level: %s\n"    , info.log_level_info   );
}

// Печатаем информацию о приборе и проекте (info)
static void cmd_info(const command_t *command) {
    if (command->argc >= 1) device_info();
    if (command->argc >= 2) project_info();
}
/// DEVICE /// END ///

/// PROFILING ///
// Печатает монотонное время прибора, в миллисекундах
//  Монотонное время прибора ведёт таймер. Он прибавляет единицу раз в микросекунду, 
//  начиная с момента сброса. Показание счёта возвращает функция time_us_64()
static void cmd_uptime(const command_t *command) {
    // Печатает монотонное время прибора, в миллисекундах
    printf("uptime: %llu ms\n", time_us_64() / 1000);
    //  time_us_64() возвращает uint64_t. Такое число печатают через %llu. 
    //  Деление на 1000 переводит микросекунды в миллисекунды.
}

/// PROFILING ///
// печатает среднее и максимальное время суперцикла
static void cmd_main_time_exec(const command_t *command) {
    printf("iteration avg %.2f us, max %u us\n", 
            profiling_avg_us(), (unsigned)profiling_max_us());
}

/// PROFILING ///
// обнуляет максимальное время суперцикла
static void cmd_main_time_reset(const command_t *command) {
    profiling_reset_max();          // Обнуление
    printf("%s", "max reset\n");    // Эхо-вывод
}

/// LED-TASK /// SATART ///
// вывод состояния светодиода
static void led_state_print(void) {
    // Состояние светодиода
    char* led_state_txt = "unnoun";
    switch (led_task_get_state()) {
    case LED_STATE_ON:    led_state_txt = "on"   ;  break;
    case LED_STATE_OFF:   led_state_txt = "off"  ;  break;
    case LED_STATE_BLINK: led_state_txt = "blink";  break;
    }//switch

    // Период светодиода (ms, милисекунды, 10-3)
    uint32_t period_ms = led_task_get_period_ms();

    // Печать справки по светодиду
    printf("led: %s, period %d ms\n", 
            led_state_txt, (unsigned)period_ms);
}

// включить светодиод (горит)
static void cmd_led_enable(const command_t *command) {
    // Установить состояние светодиода
    led_task_set_state(LED_STATE_ON);
    led_state_print(); // вывод состояния светодиода
}

// выключить светодиод (погас)
static void cmd_led_disable(const command_t *command) {
    // Установить состояние светодиода
    led_task_set_state(LED_STATE_OFF);
    led_state_print(); // вывод состояния светодиода
}

// моргание светодиода (моргает)
static void cmd_led_blink(const command_t *command) {
    // Установить состояние светодиода
    led_task_set_state(LED_STATE_BLINK);
    led_state_print(); // вывод состояния светодиода
}

// установить период светодиода в милисекундах (ms)
static void cmd_led_period(const command_t *command) {
    // Аргумент <period_ms> обязателен
    if (command->argc != 2) {
        printf("error: usage led_period <period_ms>\n"); //ошибка
        return; //ранний выход с ошибкой
    }

    // Период светодиода
    uint32_t period_ms;   // Сюда возвращается число
    // Проверка на число и перевод строки в число
    bool isNumber = parse_u32(command->argv[0], &period_ms);

    // Это число? или введена только команда calc_pi (argc = 1)
    if (isNumber) {
        // Установить период светодиода (period_ms != 0)
        if (led_task_set_period_ms(period_ms)) {
            led_state_print(); // вывод состояния светодиода (успех)
        }else {
            printf("error: period_ms must be greater than 0\n"); //ошибка
        }    
    } else {
        // Второй аргумент должен быть числом (число больше нуля)
        printf("error: usage led_period <period_ms>\n"); //ошибка
        //printf("error: usage led_period <period_ms> [%s]\n", command->argv[0]);
    }
}
/// LED-TASK /// END ///

/// BUTTON-TASK ///
// Состояние кнопки и число нажатий
// выводит состояние кнопки и число нажатий с момента сброса
static void cmd_button(const command_t *command) {
    // Состояние кнопки
    char* button_txt = button_task_is_pressed() ? "pressed" : "released";
    // Число нажатий
    uint32_t button_count = button_task_get_press_count();
    // Вывод
    printf("button: %s, presses %u\n", button_txt, (unsigned)button_count);
}

// Массив команд для декодирования COM-порта
const api_command_t api_commands[] = {
    // "имя команды"    , "описание команды (латиница!)"            , функция-обратный вызов},  // Описание команды (кирилица!)
    {"info"             , "device passport [project-info] "         , cmd_info              },  // Паспорт устройства (2-й аргумент - паспорт проекта)
    {"uptime"           , "time since reset"                        , cmd_uptime            },  // Печатает монотонное время прибора, в миллисекундах
    {"led_enable"       , "enable led (on)"                         , cmd_led_enable        },  // Включить светодиод (горит)
    {"led_disable"      , "disable led (off)"                       , cmd_led_disable       },  // Выключить светодиод (потух)
    {"led_blink"        , "make led blink (period in ms)"           , cmd_led_blink         },  // Моргание светодиода (моргает)
    {"led_period"       , "set period blink in ms: led_period (ms)" , cmd_led_period        },  // Установить период светодиода в милисекундах (ms)
    {"button"           , "button status and count of presses"      , cmd_button            },  // Состояние кнопки и число нажатий
    {"pi_start"         , "start calculate pi: pi_start [terms]"    , cmd_pi_start          },  // Старт вычисления  π 
    {"pi"               , "print pi and time calc in ms"            , cmd_pi                },  // Печатает значение π и время расчёта в миллисекундах
    {"main_time_exec"   , "superloop iteration: average and maximum", cmd_main_time_exec    },  // Печатает среднее и максимальное время суперцикла
    {"main_time_reset"  , "reset maximum iteration"                 , cmd_main_time_reset   },  // Обнуляет максимальное время суперцикла
};

// Общее количество команд (вычисляется и сохраняется в константу)
const uint api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);



