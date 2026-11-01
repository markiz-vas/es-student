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
// calc_pi // TASK 2.2.3 //
// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с (4,68 с)
static const uint CALC_PI_TERMS = 1000000;

// ТЕОРИЯ // TASK 2.2.3 //
// Без слова 'volatile' компилятор вправе перенести расчёт за второе чтение часов. 
//  Результат от этого не изменится, но замер измерит пустое место. 
//  Запись в volatile компилятор не переносит, и расчёт остаётся между двумя чтениями. 
static volatile double pi_result;

// Вычисление ПИ, суммирует ряд Лейбница
static double calc_pi(uint terms) {
    // сумма ряда и знак очередного члена, оба double
    double sum  = 0.0;
    double sign = 1.0;
    // для k от 0 до terms: прибавить к сумме sign / (2k + 1) и сменить знак
    for (int k = 0; k < terms; k++) {
        // Прибавить очередной член ряда: ±1 / (2k + 1)
        sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }
    // Сумма ряда равна π/4
    return 4.0 * sum;           // вернуть сумму, умноженную на 4
}

/// CALC_PI /// Объявление, описание функции перед main() ///
// печатает значение π и время расчёта в миллисекундах
// аргумент команды обязателен! Должен быть больше 0!
static void cmd_calc_pi(const command_t *command) {
    uint32_t members_qty;   // Сюда возвращается число
    // Проверка на число и перевод строки в число
    bool isNumber = parse_u32(command->argv[0], &members_qty);

    // Это число? или введена только команда calc_pi (argc = 1)
    if (isNumber || command->argc == 1) {
        // Значение по умолчанию (аргумент не задан)
        if (command->argc == 1) members_qty = CALC_PI_TERMS;
        // Число больше нуля?
        if (members_qty > 0) {
            // Запуск вычисления π, Сохранение времени вычисления
            uint64_t start_us = time_us_64();
            pi_result = calc_pi(members_qty);
            uint64_t spent_us = time_us_64() - start_us;

            // Вывод результатов вычисления
            printf("pi: %.9f\n", pi_result);
            printf("time: %llu ms\n", spent_us / 1000);
        } else {
            // Точность должна быть больше нуля
            printf("error: terms must be gerater than 0\n");
        }
    } else {
        // Второй аргумент должен быть числом (число больше нуля)
        printf("error: usage calc_pi [%s]\n", command->argv[0]);
    }
}
/// CALC_PI /// END ///


/// DEVICE /// Информация о приборе ///
// печатаем паспорт устройства
static void cmd_info(const command_t *command) {
    device_info();
}

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


// Массив команд для декодирования COM-порта
const api_command_t api_commands[] = {
    // "имя команды"    , "описание команды (латиница!)"            , функция-обратный вызов},  // Описание команды (кирилица!)
    {"info"             , "device passport"                         , cmd_info              },  // Паспорт устройства
    {"calc_pi"          , "calculate pi: calc_pi [terms]"           , cmd_calc_pi           },  // Печатает значение π и время расчёта в миллисекундах
    {"uptime"           , "time since reset"                        , cmd_uptime            },  // Печатает монотонное время прибора, в миллисекундах
    {"main_time_exec"   , "superloop iteration: average and maximum", cmd_main_time_exec    },  // Печатает среднее и максимальное время суперцикла
    {"main_time_reset"  , "reset maximum iteration"                 , cmd_main_time_reset   },  // Обнуляет максимальное время суперцикла
    {"led_enable"       , "enable led (on)"                         , cmd_led_enable        },  // Включить светодиод (горит)
    {"led_disable"      , "disable led (off)"                       , cmd_led_disable       },  // Выключить светодиод (потух)
    {"led_blink"        , "make led blink (period in ms)"           , cmd_led_blink         },  // Моргание светодиода (моргает)
    {"led_period"       , "set period blink in ms"                  , cmd_led_period        },  // Установить период светодиода в милисекундах (ms)
};

// Общее количество команд (вычисляется и сохраняется в константу)
const uint api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);



