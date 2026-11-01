// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// для работы со строками, strcmp()
#include <string.h>
// заголовочный файл задача светодиода: выключен, горит или мигает
#include "led-task.h"
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
// заголовочные файлы для sleep_ms, stdio_init_all
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint
#include "pico/stdlib.h"


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






//blink// /// BLINK /// START ///
//blink// // Период мигания светодиода
//blink// const uint BLINK_HALF_PERIOD_MS = 500;
//blink// 
//blink// // Последний замер монотонного времени
//blink// uint64_t last_toggle_us = 0;
//blink// 
//blink// // Функция изменения значения светодиода
//blink// void blink(void)
//blink// {
//blink//     // Замер монотонного времени
//blink//     uint64_t now_us = time_us_64();
//blink// 
//blink//     // Полупериод задан в миллисекундах, а часы идут в микросекундах. Поэтому константа умножается на 1000.
//blink//     if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
//blink//     {
//blink//         last_toggle_us = now_us;    // Храним последнее время вкл. светодиода
//blink//         led_toggle();               // вызывает led_set() с обратным значением
//blink//     }
//blink// }
//blink// /// BLINK /// END ///


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

    // Инициализация, настраивает вывод на выход, устанавливает состояние светодиода
    led_task_init();

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

    // Печатает, что за прибор перед вами и какая в нём прошивка
    log_version();

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
        // Отработка задачи светодиода по состоянию
        led_task_handle();


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
            //DBG// // Печатает структуру команды (command != NULL)
            api_print_debug_command(command);
            
            // Обработка команды в api.h
            api_handle(command);
        }
        if (command == NULL) continue;
    }// Суперцикл
}