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
// заголовочный файл информации о памяти устройства #include "memory\memory.h"
#include "memory.h"
// заголовочный файл информации Структура с названием и ссылкой на функцию команды
#include "command.h"
// заголовочный файл модуля часов "clock\clock.h"
#include "clock.h"

//button// // Нормер выхода
//button// const uint BUT_|_TON_PIN = 15;

//button// // Константа 20mc для борьбы с дребезгом
//button// const uint DEB_|_OUNCE_MS = 20;

//button// // Функция определяет, был ли дребезг
//button// // Считывает два значение, возвращает true, если оба true
//button// bool get_but_|_ton_debounce(uint pin)
//button// {
//button//     bool state = gpio_get(pin);     // Считали первое значение
//button//     sl_|_eep_ms(DEB_|_OUNCE_MS);          // подождали DEB_|_OUNCE_MS
//button//     return state && gpio_get(pin);  // Считали второе значение
//button// }

/// COMAND READER /// BEGEN ///
// Буфер и его длина (32 символа) (глобальная переменная)
#define LINE_SIZE 32
char line[LINE_SIZE];
uint line_length = 0;

//button// // включаем светодиод и сообщаем новое состояние
//button// void cmd_ena_|_ble(void) {
//button//     led_set(true);  // включить
//button//     LOG_INF("led %s\n", led_is_on() ? "on" : "off");
//button// }

//button// // выключаем светодиод и сообщаем новое состояние
//button// void cmd_dis_|_able(void) {
//button//     led_set(false); // погасить
//button//     LOG_INF("led %s\n", led_is_on() ? "on" : "off");
//button// }

// печатаем паспорт устройства
void cmd_info(void) {
    device_info();
}

// печатаем строку журнала о версии прошивки
void cmd_version(void) {
    log_version();
}

// ping-pong
void cmd_ping(void) {
    printf("pong\n");
}

// Информация о внутреннем устройстве памяти устройства
void cmd_mem_info(void) {
    mem_info();
}

// Информация о динамической памяти устройства (RAM)
void cmd_fw_info(void){
    fw_info();
}

// Печатает в консоль информацию о структуре info_t (раскладка в памяти)
void cmd_dev_info(void){
    dev_info();
}

// Прибор читает память по числам, взятым из таблицы векторов (флеш-память)
void cmd_boot_info(void){
    boot_info();
}

// Модуль часов (инфо)
void cmd_clk_info(void){
    clk_info();
}

// Печатает монотонное время прибора, в миллисекундах
void cmd_uptime(void){
    uptime();
}

// Массив команд для декодирования COM-порта
const struct command_t commands[] = {
    //button//{ "ena_|_ble"      , cmd_ena_|_ble    },  // вкл. светодиод
    //button//{ "dis_|_able"     , cmd_dis_|_able   },  // выкл. светодиод
    { "info"        , cmd_info      },  // паспорт устройства
    { "version"     , cmd_version   },  // версия программы логирования
    { "ping"        , cmd_ping      },  // ping-pong
    { "mem_info"    , cmd_mem_info  },  // Информация о внутреннем устройстве памяти устройства
    { "fw_info"     , cmd_fw_info   },  // Информация о динамической памяти устройства (RAM)
    { "dev_info"    , cmd_dev_info  },  // Информация о о структуре info_t (раскладка в памяти)
    { "boot_info"   , cmd_boot_info },  // Прибор читает память по числам, взятым из таблицы векторов (флеш-память)
    { "clk_info"    , cmd_clk_info  },  // Модуль часов (инфо)
    { "uptime"      , cmd_uptime    },  // Печатает монотонное время прибора, в миллисекундах
};
// Общее количество команд (вычисляется и сохраняется в константу)
const uint command_count = sizeof(commands) / sizeof(commands[0]);


// Разбирает команду и вызывает функцию по указателю
void handle_command(const char *command)
{
    // Расшифровка команд по массиву команд
    for (uint i = 0; i < command_count; i++) {          // цикл по массиву
        if (strcmp(command, commands[i].name) == 0) {   // в массиве есть команда
            if (commands[i].handler != NULL) {          // ссылка на функцию не пустая
                commands[i].handler();                  // вызов функции по указателю
            } else {
                LOG_ERR("NULL handler, command: %s\n", command);
            }// NULL
            return; // Ранний выход из цикла, если нашлась команда в массиве
        }// if
    }// for
    // Правило хорошего тона, вернуть неизвестную команду
    LOG_ERR("unknown command: %s\n", command);
}

// Функция считывания слова из COM-порта
void read_line(void)
{
    // Считали один символ из строки
    int symbol = getchar_timeout_us(0);
    if (symbol == PICO_ERROR_TIMEOUT) {
        return; // Если пусто - ранний выход
    }

    // Проверка на конец строки
    if (symbol == '\r' || symbol == '\n')    {
        putchar('\n'); // Печать в консоль
        line[line_length] = '\0';   // символ конца строки (обязательный)
        if (line_length > 0) {
            LOG_DBG("got %s\n", line);  // Печать строки целиком
            handle_command(line);       // расшифровка команды
        }
        line_length = 0;    // После выполнения команды обнуляем строку с командой
        line[line_length] = '\0';   // символ конца строки (обязательный)
        return;
    }

    // Проверяем, что есть место под конечный символ ('\0')
    if (line_length + 1 < LINE_SIZE)    {
        line[line_length] = (char)symbol;   // собираем строку из char
        line_length = line_length + 1;      // увеличиваем длину строки
        putchar(symbol); // Печать в консоль
        // ТЕОРИЯ //
        // Возврат символа в терминал. putchar(symbol) печатает то, что вы набрали. 
        // Прошивка обязана это делать сама: терминал показывает не ваши нажатия, 
        // а то, что ответил прибор. Без этой строки набор идёт вслепую.
    }
}
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
    //button// gpio_init(BUT_|_TON_PIN);              // инициализируем пин кнопки
    //button// gpio_set_dir(BUT_|_TON_PIN, GPIO_IN ); // настраиваем пин кнопки на вход
    //button// gpio_pull_up(BUT_|_TON_PIN);           // подтягиваем кнопку к питанию

    //button// // ТЕОРИЯ //
    //button// // Пока кнопка не нажата, BUT_|_TON_PIN ни к чему не подключён — 
    //button// // на нём нет ни нуля, ни единицы, и читаться будет случайный мусор. 
    //button// // Чтобы этого не было, внутри микроконтроллера есть подтягивающий резистор: 
    //button// // он мягко притягивает вывод к питанию, и незанятый вход читается как единица

    //button// // previous - что вывод показывал на прошлом шаге цикла (нужно для кнопки)
    //button// bool previous = false;





    // Бесконечный цикл (Суперцикл) - это обязательное условие,
    // Так как все программы запускаются на Земле 1 раз
    while (1)
    {
        //button// // Проверяйте не состояние, а его смену. Суперцикл крутится тысячи раз в секунду. 
        //button// // Если переключать светодиод, пока кнопка нажата, он будет мигать всё время нажатия. 
        //button// // Переключать нужно в момент, когда вывод перешёл из единицы в ноль, то есть когда кнопку нажали:
        //button// // bool current = gpio_get(BUT_|_TON_PIN);

        //button// // Замена прямого чтения вывода (gpio_get) вызовом функции (get_but_||ton_debounce)
        //button// bool current = get_but_|_ton_debounce(BUT_|_TON_PIN); // читаем состояние пина кнопки с задержкой
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
        read_line();

    }// Суперцикл
}