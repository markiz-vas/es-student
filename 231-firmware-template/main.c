// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// для работы со строками, strcmp()
#include <string.h>
// заголовочный файл задача светодиода: выключен, горит или мигает
#include "led-task.h"
// заголовочный файл задача кнопки: нажатие, переключение состояния светодиода
#include "button-task.h"
// Заголовочный файл задачи вычисления π (пи) с прерыванием на другие процессы
#include "pi-task.h"
// заголовочный файл для лога ошибок #include "logging\log.h"
#include "log.h"
// заголовочный файл лога информации о приборе #include "device\device.h"
#include "device.h"
// заголовочный файл модуля профилирования (время суперцикла)
#include "profiling.h"
// заголовочный файл протокола обмена текстовыми командами
#include "stdio-text-protocol.h"
// заголовочный файл протокола api
#include "api.h"
// заголовочные файлы для stdio_init_all
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint
#include "pico/stdlib.h"


// TASK 2.3.1// dell memory, clock //
//memory// // заголовочный файл информации о памяти устройства #include "memory\memory.h"
//memory// #include "memory.h"
//clock// // // заголовочный файл модуля часов "clock\clock.h"
//clock// // #include "clock.h"

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


// Перечисляемый тип с секундомеров. 0 - суперцикл, [1 - STOPWATCH_QTY+1] - остальные
enum stopwatch_t {
    STOPWATCH_GLOBAL  = 0,  // global 
    STOPWATCH_LOOP    = 1,  // loop
    STOPWATCH_COMMAND = 2,  // command
    STOPWATCH_LED     = 3,  // led
    STOPWATCH_BUTTON  = 4,  // button
    STOPWATCH_PI      = 5,  // pi
};



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

    // led-task //
    // Инициализация, настраивает вывод на выход, устанавливает состояние светодиода
    led_task_init();

    // button-task //
    // Инициализация, настраивает вывод на вход
    button_task_init();

    // pi-task //
    // Инициализация, задачи вычисления π
    pi_task_init();

    // profiling //
    // Создаём глобальный секундомер, запоминаем начальное показание часов для профилирования
    // profiling_global_init();
    profiling_stopwatch_init(STOPWATCH_GLOBAL, "global"); // Используется для возврата значений (вычислений в нём нет)
    
    // profiling //
    // Создаём Секундомер по количеству STOPWATCH_QTY [1 - STOPWATCH_QTY+1]
    profiling_stopwatch_init(STOPWATCH_LOOP   , "loop"      ); 
    profiling_stopwatch_init(STOPWATCH_COMMAND, "command"   ); 
    profiling_stopwatch_init(STOPWATCH_LED    , "led"       ); 
    profiling_stopwatch_init(STOPWATCH_BUTTON , "button"    ); 
    profiling_stopwatch_init(STOPWATCH_PI     , "pi"        ); 

    // text-protocol //
    // Инициализация протокола через usb
    stdio_text_protocol_init();

    // Печатает, что за прибор перед вами и какая в нём прошивка
    LOG_INF("%s %s\n", DEVICE_NAME, FIRMWARE_VERSION);


    // Бесконечный цикл (Суперцикл) - это обязательное условие,
    // Так как все программы запускаются на Земле 1 раз
    while (1)
    {
        // profiling //
        // учитываем прошлую итерацию (суперцикл, global)
        // profiling_global_iteration();
        profiling_start(STOPWATCH_GLOBAL);
        // Замер от начала и до конца (loog)
        profiling_start(STOPWATCH_LOOP);


        // button-task //
        // Отработка задачи кнопки
        profiling_start(STOPWATCH_BUTTON);
        button_task_handle();
        profiling_stop (STOPWATCH_BUTTON);

        // led-task //
        // Отработка задачи светодиода по состоянию
        profiling_start(STOPWATCH_LED);
        led_task_handle();
        profiling_stop (STOPWATCH_LED);

        // pi-task //
        // Отработка задачи вычисления π
        profiling_start(STOPWATCH_PI);
        pi_task_handle();
        profiling_stop (STOPWATCH_PI);


        // ТЕОРИЯ //
        // Нулевое время ожидания превращает вызов в вопрос «есть ли что-нибудь для меня?»:
        // если символ в буфере есть, функция вернёт его код (int c),
        // если нет — вернёт PICO_ERROR_TIMEOUT и тут же передаст управление дальше
        // int c = getchar_timeout_us(0); // 0 мкс ожидания: забрать символ, только если он уже принят
        // if (c != PICO_ERROR_TIMEOUT) { print_f("got %c\n", (char)c); } // PICO_ERROR_TIMEOUT означает «символов не было»

        profiling_start(STOPWATCH_COMMAND);
        // Считываем строку на вход
        const command_t *command = stdio_text_protocol_handle();

        // принятая строка становится командой, команда — действием прибора
        if (command != NULL) {
            //DBG// // Печатает структуру команды (command != NULL)
            api_print_debug_command(command);

            // Обработка команды в api.h
            api_handle(command);
        }
        //if (command == NULL) continue;
        profiling_stop (STOPWATCH_COMMAND);

        profiling_stop (STOPWATCH_LOOP);
    }// Суперцикл
}