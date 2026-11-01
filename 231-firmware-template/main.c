// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// для работы со строками, strcmp()
#include <string.h>
// заголовочный файл светодиода #include "led\led.h"
#include "led.h"
// заголовочный файл для лога ошибок #include "logging\log.h"
#include "log.h"
// заголовочный файл лога информации о приборе #include "device\device.h"
#include "device.h"
// заголовочный файл модуля профилирования (время суперцикла)
#include "profiling.h"
//command.h// // заголовочный файл информации Структура с названием и ссылкой на функцию команды
//command.h// #include "command.h"
// заголовочный файл протокола обмена текстовыми командами
#include "stdio-text-protocol.h"
// заголовочный файл протокола api
#include "api.h"


// TASK 2.3.1// dell memory, clock //
//memory// // заголовочный файл информации о памяти устройства #include "memory\memory.h"
//memory// #include "memory.h"
//clock// // // заголовочный файл модуля часов "clock\clock.h"
//clock// // #include "clock.h"

//button// // Нормер выхода
//button// const uint BUTTON_PIN = 15;

//button// // Константа 20mc для борьбы с дребезгом
//button// const uint DEBOUNCE_MS = 20;

//button// // Функция определяет, был ли дребезг
//button// // Считывает два значение, возвращает true, если оба true
//button// bool get_button_debounce(uint pin)
//button// {
//button//     bool state = gpio_get(pin);     // Считали первое значение
//button//     sleep_ms(DEBOUNCE_MS);          // подождали DEBOUNCE_MS
//button//     return state && gpio_get(pin);  // Считали второе значение
//button// }

/// COMAND READER /// BEGEN ///
// Буфер и его длина (32 символа) (глобальная переменная)
//command.h// #define LINE_SIZE 32
//command.h// char line[LINE_SIZE];
//command.h// uint line_length = 0;

//button// // включаем светодиод и сообщаем новое состояние
//button// void cmd_enable(void) {
//button//     led_set(true);  // включить
//button//     LOG_INF("led %s\n", led_is_on() ? "on" : "off");
//button// }

//button// // выключаем светодиод и сообщаем новое состояние
//button// void cmd_disable(void) {
//button//     led_set(false); // погасить
//button//     LOG_INF("led %s\n", led_is_on() ? "on" : "off");
//button// }

//log// // печатаем строку журнала о версии прошивки
//log// void cmd_version(void) {
//log//     log_version();
//log// }

//memory// // Информация о внутреннем устройстве памяти устройства
//memory// void cmd_mem_info(void) {
//memory//     mem_info();
//memory// }

//memory// // Информация о динамической памяти устройства (RAM)
//memory// void cmd_fw_info(void){
//memory//     fw_info();
//memory// }

//memory// // Печатает в консоль информацию о структуре info_t (раскладка в памяти)
//memory// void cmd_dev_info(void){
//memory//     dev_info();
//memory// }

//memory// // Прибор читает память по числам, взятым из таблицы векторов (флеш-память)
//memory// void cmd_boot_info(void){
//memory//     boot_info();
//memory// }

//clock// // Модуль часов (инфо)
//clock// void cmd_clk_info(void){
//clock//     clk_info();
//clock// }

//clock// // Печатает монотонное время прибора, в миллисекундах
//clock// void cmd_uptime(void){
//clock//     uptime();
//clock// }


//command.h// /// DEVICE /// Информация о приборе ///
//command.h// // печатаем паспорт устройства
//command.h// void cmd_info(void) {
//command.h//     device_info();
//command.h// }

//command.h// /// CALC_PI /// Объявление, описание функции перед main() ///
//command.h// // печатает значение π и время расчёта в миллисекундах
//command.h// void cmd_calc_pi(void);


//command.h// /// PROFILING ///
//command.h// // Печатает монотонное время прибора, в миллисекундах
//command.h// //  Монотонное время прибора ведёт таймер. Он прибавляет единицу раз в микросекунду, 
//command.h// //  начиная с момента сброса. Показание счёта возвращает функция time_us_64()
//command.h// void cmd_uptime(void){
//command.h//     // Печатает монотонное время прибора, в миллисекундах
//command.h//     printf("uptime: %llu ms\n", time_us_64() / 1000);
//command.h//     //  time_us_64() возвращает uint64_t. Такое число печатают через %llu. 
//command.h//     //  Деление на 1000 переводит микросекунды в миллисекунды.
//command.h// }

//command.h// // печатает среднее и максимальное время суперцикла
//command.h// void main_time_exec(void){
//command.h//     printf("iteration avg %.2f us, max %u us\n", 
//command.h//             profiling_avg_us(), (unsigned)profiling_max_us());
//command.h// }
//command.h// 
//command.h// // обнуляет максимальное время суперцикла
//command.h// void main_time_reset(){
//command.h//     profiling_reset_max();  // Обнуление
//command.h//     printf("max reset\n");    // Эхо-вывод
//command.h// }



//command.h// // Массив команд для декодирования COM-порта
//command.h// const struct old_command_t old_commands[] = {
//command.h//     //button// { "enable"          , cmd_enable        },  // вкл. светодиод
//command.h//     //button// { "disable"         , cmd_disable       },  // выкл. светодиод
//command.h//     //memory// { "mem_info"        , cmd_mem_info      },  // Информация о внутреннем устройстве памяти устройства
//command.h//     //memory// { "fw_info"         , cmd_fw_info       },  // Информация о динамической памяти устройства (RAM)
//command.h//     //memory// { "dev_info"        , cmd_dev_info      },  // Информация о структуре info_t (раскладка в памяти)
//command.h//     //memory// { "boot_info"       , cmd_boot_info     },  // Прибор читает память по числам, взятым из таблицы векторов (флеш-память)
//command.h//     //clock//  { "clk_info"        , cmd_clk_info      },  // Модуль часов (инфо)
//command.h//     //clock//  { "uptime"          , cmd_uptime        },  // Печатает монотонное время прибора, в миллисекундах
//command.h//     //clock//  { "clk_sys_low"     , clk_sys_low       },  // Установить пониженную частоту процессора
//command.h//     //clock//  { "clk_sys_default" , clk_sys_default   },  // Установить частоту прочессора по умолчанию
//command.h//     //log//    { "version"         , cmd_version       },  // версия программы логирования
//command.h//     { "info"            , cmd_info          },  // паспорт устройства
//command.h//     { "calc_pi"         , cmd_calc_pi       },  // Печатает значение π и время расчёта в миллисекундах
//command.h//     { "uptime"          , cmd_uptime        },  // Печатает монотонное время прибора, в миллисекундах
//command.h//     { "main_time_exec"  , main_time_exec    },  // печатает среднее и максимальное время суперцикла
//command.h//     { "main_time_reset" , main_time_reset   },  // обнуляет максимальное время суперцикла
//command.h// };
//command.h// // Общее количество команд (вычисляется и сохраняется в константу)
//command.h// const uint old_command_count = sizeof(old_commands) / sizeof(old_commands[0]);


//command.h// // Разбирает команду и вызывает функцию по указателю
//command.h// void handle_command(const char *command)
//command.h// {
//command.h//     // Расшифровка команд по массиву команд
//command.h//     for (uint i = 0; i < old_command_count; i++) {          // цикл по массиву
//command.h//         if (strcmp(command, old_commands[i].name) == 0) {   // в массиве есть команда
//command.h//             if (old_commands[i].handler != NULL) {          // ссылка на функцию не пустая
//command.h//                 old_commands[i].handler();                  // вызов функции по указателю
//command.h//             } else {
//command.h//                 LOG_ERR("NULL handler, command: %s\n", command);
//command.h//             }// NULL
//command.h//             return; // Ранний выход из цикла, если нашлась команда в массиве
//command.h//         }// if
//command.h//     }// for
//command.h//     // Правило хорошего тона, вернуть неизвестную команду
//command.h//     LOG_ERR("unknown command: %s\n", command);
//command.h// }

// Функция считывания слова из COM-порта
// void read_line(void)
// {
//     // Считали один символ из строки
//     int symbol = getchar_timeout_us(0);
//     if (symbol == PICO_ERROR_TIMEOUT) {
//         return; // Если пусто - ранний выход
//     }

//     // Проверка на конец строки
//     if (symbol == '\r' || symbol == '\n')    {
//         putchar('\n'); // Печать в консоль
//         line[line_length] = '\0';   // символ конца строки (обязательный)
//         if (line_length > 0) {
//             LOG_DBG("got %s\n", line);  // Печать строки целиком
//             handle_command(line);       // расшифровка команды
//         }
//         line_length = 0;    // После выполнения команды обнуляем строку с командой
//         line[line_length] = '\0';   // символ конца строки (обязательный)
//         return;
//     }

//     // Проверяем, что есть место под конечный символ ('\0')
//     if (line_length + 1 < LINE_SIZE)    {
//         line[line_length] = (char)symbol;   // собираем строку из char
//         line_length = line_length + 1;      // увеличиваем длину строки
//         putchar(symbol); // Печать в консоль
//         // ТЕОРИЯ //
//         // Возврат символа в терминал. putchar(symbol) печатает то, что вы набрали. 
//         // Прошивка обязана это делать сама: терминал показывает не ваши нажатия, 
//         // а то, что ответил прибор. Без этой строки набор идёт вслепую.
//     }
// }
/// COMAND READER /// END ///


/// BLINK /// START ///
// Период мигания светодиода
const uint BLINK_HALF_PERIOD_MS = 500;

// Последний замер монотонного времени
uint64_t last_toggle_us = 0;

// Функция изменения значения светодиода
void blink(void)
{
    // Замер монотонного времени
    uint64_t now_us = time_us_64();

    // Полупериод задан в миллисекундах, а часы идут в микросекундах. Поэтому константа умножается на 1000.
    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
    {
        last_toggle_us = now_us;    // Храним последнее время вкл. светодиода
        led_toggle();               // вызывает led_set() с обратным значением
    }
}
/// BLINK /// END ///


//command.h// /// CALC_PI /// SATART ///
//command.h// // calc_pi // TASK 2.2.3 //
//command.h// // прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
//command.h// // 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с (4,68 с)
//command.h// const uint CALC_PI_TERMS = 1000000;
//command.h// 
//command.h// // Вычисление ПИ, суммирует ряд Лейбница
//command.h// double calc_pi(uint terms)
//command.h// {
//command.h//     // сумма ряда и знак очередного члена, оба double
//command.h//     double sum  = 0.0;
//command.h//     double sign = 1.0;
//command.h//     // для k от 0 до terms: прибавить к сумме sign / (2k + 1) и сменить знак
//command.h//     for (int k = 0; k < terms; k++) {
//command.h//         // Прибавить очередной член ряда: ±1 / (2k + 1)
//command.h//         sum += sign / (2.0 * k + 1.0);
//command.h//         sign = -sign;
//command.h//     }
//command.h//     // Сумма ряда равна π/4
//command.h//     return 4.0 * sum;           // вернуть сумму, умноженную на 4
//command.h// }
//command.h// 
//command.h// // ТЕОРИЯ // TASK 2.2.3 //
//command.h// // Без слова 'volatile' компилятор вправе перенести расчёт за второе чтение часов. 
//command.h// //  Результат от этого не изменится, но замер измерит пустое место. 
//command.h// //  Запись в volatile компилятор не переносит, и расчёт остаётся между двумя чтениями. 
//command.h// volatile double pi_result;
//command.h// 
//command.h// // печатает значение π и время расчёта в миллисекундах
//command.h// void cmd_calc_pi(void)
//command.h// {
//command.h//     uint64_t start_us = time_us_64();
//command.h//     pi_result = calc_pi(CALC_PI_TERMS);
//command.h//     uint64_t spent_us = time_us_64() - start_us;
//command.h//     printf("pi: %.8f\n", pi_result);
//command.h//     printf("time: %llu ms\n", spent_us / 1000);
//command.h// }
//command.h// /// CALC_PI /// END ///



int main()
{
    // подготовить стандартный ввод-вывод по каналам, включённым в CMake
    // pico_enable_stdio_usb (${PROJECT_NAME} 1) // CMakeLists.txt
    // pico_enable_stdio_uart(${PROJECT_NAME} 0) // CMakeLists.txt
    stdio_init_all();

    // ТЕОРИЯ //
    // Обратите внимание, что канал включается целиком, в обе стороны сразу: 
    // отдельно настраивать приём не нужно. Забыть можно любую из двух строк, 
    // и обе ошибки выглядят одинаково неприятно — проект собирается, плата работает, 
    // светодиод мигает, а терминал молчит. 
    // Никакого сообщения об ошибке при этом не будет: сборке нечего сообщать, 
    // ведь и print_f() на месте, и канал формально существует, 
    // просто он не попал в образ или не был включён. 

    // Инициализация, настраивает вывод на выход и гасит/зажигает светодиод
    led_init();

    //button// // Инициализация вывода     // отдать вывод модулю GPIO
    //button// // Вывод работает на вход   // задать направление «вход»
    //button// // Подключаем к подтягивающему регистру
    //button// gpio_init(BUTTON_PIN);              // инициализируем пин кнопки
    //button// gpio_set_dir(BUTTON_PIN, GPIO_IN ); // настраиваем пин кнопки на вход
    //button// gpio_pull_up(BUTTON_PIN);           // подтягиваем кнопку к питанию

    //button// // ТЕОРИЯ //
    //button// // Пока кнопка не нажата, BUTTON_PIN ни к чему не подключён — 
    //button// // на нём нет ни нуля, ни единицы, и читаться будет случайный мусор. 
    //button// // Чтобы этого не было, внутри микроконтроллера есть подтягивающий резистор: 
    //button// // он мягко притягивает вывод к питанию, и незанятый вход читается как единица

    //button// // previous - что вывод показывал на прошлом шаге цикла (нужно для кнопки)
    //button// bool previous = false;



    // profiling //
    // запоминаем начальное показание часов для профилирования
    profiling_init();


    // text-protocol //
    // Инициализация протокола через usb
    stdio_text_protocol_init();

    // Бесконечный цикл (Суперцикл) - это обязательное условие,
    // Так как все программы запускаются на Земле 1 раз
    while (1)
    {
        // profiling //
        // учитываем прошлую итерацию
        profiling_iteration();

        //button// // Проверяйте не состояние, а его смену. Суперцикл крутится тысячи раз в секунду. 
        //button// // Если переключать светодиод, пока кнопка нажата, он будет мигать всё время нажатия. 
        //button// // Переключать нужно в момент, когда вывод перешёл из единицы в ноль, то есть когда кнопку нажали:
        //button// // bool current = gpio_get(BUTTON_PIN);

        //button// // Замена прямого чтения вывода (gpio_get) вызовом функции (get_but_||ton_debounce)
        //button// bool current = get_button_debounce(BUTTON_PIN); // читаем состояние пина кнопки с задержкой
        //button// // Если кнопка нажата (false - нажата, true - отжата)
        //button// if (previous == true && current == false)       // проверяем, нужно ли переключить светодиод
        //button// {
        //button//     // Переключить светодиод
        //button//     led_toggle();
        //button//     // Вывод сообщения через кабель usb в виртуальный COM-port
        //button//     LOG_INF("led %s\n", led_is_on() ? "on" : "off");
        //button// }
        //button// // Запоминаем, что нажато было раньше (чтобы не мигала)
        //button// previous = current;     // запоминаем текущее состояние пина кнопки, как предыдущее

        //blink//
        // вкл./выкл. светодиод
        blink();

        // ТЕОРИЯ //
        // Нулевое время ожидания превращает вызов в вопрос «есть ли что-нибудь для меня?»: 
        // если символ в буфере есть, функция вернёт его код (int c), 
        // если нет — вернёт PICO_ERROR_TIMEOUT и тут же передаст управление дальше
        // int c = getchar_timeout_us(0); // 0 мкс ожидания: забрать символ, только если он уже принят
        // if (c != PICO_ERROR_TIMEOUT) { print_f("got %c\n", (char)c); } // PICO_ERROR_TIMEOUT означает «символов не было»

        // Считываем строку на вход
        const command_t *command = stdio_text_protocol_handle();

        // принятая строка становится командой, команда — действием прибора
        if (command != NULL) {
            // test //
            LOG_DBG("name      %s\n", command->name);
            LOG_DBG("argc      %d\n", command->argc);
            LOG_DBG(command->argv[0] ? "argv[0]   %s\n" : "argv[0]   %p\n", command->argv[0]);
            LOG_DBG(command->argv[1] ? "argv[1]   %s\n" : "argv[1]   %p\n", command->argv[1]);
            LOG_DBG(command->argv[2] ? "argv[2]   %s\n" : "argv[2]   %p\n", command->argv[2]);
            LOG_DBG(command->argv[3] ? "argv[3]   %s\n" : "argv[3]   %p\n", command->argv[3]);
            LOG_DBG("truncated %d\n", command->truncated);
            LOG_DBG("\n");
            // test //

            // Обработка команды в api.h
            api_handle(command);
        }
        if (command == NULL) continue;
    }// Суперцикл
}