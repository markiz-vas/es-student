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
// заголовочный файл информации Структура с названием и ссылкой на функцию команды
#include "command.h"
// заголовочный файл протокола обмена текстовыми командами
#include "stdio-text-protocol.h"


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
#define LINE_SIZE 32
char line[LINE_SIZE];
uint line_length = 0;

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


/// DEVICE /// Информация о приборе ///
// печатаем паспорт устройства
void cmd_info(void) {
    device_info();
}

/// CALC_PI /// Объявление, описание функции перед main() ///
// печатает значение π и время расчёта в миллисекундах
void cmd_calc_pi(void);


/// PROFILING ///
// Печатает монотонное время прибора, в миллисекундах
//  Монотонное время прибора ведёт таймер. Он прибавляет единицу раз в микросекунду, 
//  начиная с момента сброса. Показание счёта возвращает функция time_us_64()
void cmd_uptime(void){
    // Печатает монотонное время прибора, в миллисекундах
    printf("uptime: %llu ms\n", time_us_64() / 1000);
    //  time_us_64() возвращает uint64_t. Такое число печатают через %llu. 
    //  Деление на 1000 переводит микросекунды в миллисекунды.
}

// печатает среднее и максимальное время суперцикла
void main_time_exec(void){
    printf("iteration avg %.2f us, max %u us\n", 
            profiling_avg_us(), (unsigned)profiling_max_us());
}

// обнуляет максимальное время суперцикла
void main_time_reset(){
    profiling_reset_max();  // Обнуление
    printf("max reset\n");    // Эхо-вывод
}



// Массив команд для декодирования COM-порта
const struct old_command_t old_commands[] = {
    //button// { "enable"          , cmd_enable        },  // вкл. светодиод
    //button// { "disable"         , cmd_disable       },  // выкл. светодиод
    //memory// { "mem_info"        , cmd_mem_info      },  // Информация о внутреннем устройстве памяти устройства
    //memory// { "fw_info"         , cmd_fw_info       },  // Информация о динамической памяти устройства (RAM)
    //memory// { "dev_info"        , cmd_dev_info      },  // Информация о структуре info_t (раскладка в памяти)
    //memory// { "boot_info"       , cmd_boot_info     },  // Прибор читает память по числам, взятым из таблицы векторов (флеш-память)
    //clock//  { "clk_info"        , cmd_clk_info      },  // Модуль часов (инфо)
    //clock//  { "uptime"          , cmd_uptime        },  // Печатает монотонное время прибора, в миллисекундах
    //clock//  { "clk_sys_low"     , clk_sys_low       },  // Установить пониженную частоту процессора
    //clock//  { "clk_sys_default" , clk_sys_default   },  // Установить частоту прочессора по умолчанию
    //log//    { "version"         , cmd_version       },  // версия программы логирования
    { "info"            , cmd_info          },  // паспорт устройства
    { "calc_pi"         , cmd_calc_pi       },  // Печатает значение π и время расчёта в миллисекундах
    { "uptime"          , cmd_uptime        },  // Печатает монотонное время прибора, в миллисекундах
    { "main_time_exec"  , main_time_exec    },  // печатает среднее и максимальное время суперцикла
    { "main_time_reset" , main_time_reset   },  // обнуляет максимальное время суперцикла
};
// Общее количество команд (вычисляется и сохраняется в константу)
const uint old_command_count = sizeof(old_commands) / sizeof(old_commands[0]);


// Разбирает команду и вызывает функцию по указателю
void handle_command(const char *command)
{
    // Расшифровка команд по массиву команд
    for (uint i = 0; i < old_command_count; i++) {          // цикл по массиву
        if (strcmp(command, old_commands[i].name) == 0) {   // в массиве есть команда
            if (old_commands[i].handler != NULL) {          // ссылка на функцию не пустая
                old_commands[i].handler();                  // вызов функции по указателю
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


/// CALC_PI /// SATART ///
// calc_pi // TASK 2.2.3 //
// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 1 000 000 членов по 585 тактов при 125 МГц — около 4,7 с (4,68 с)
const uint CALC_PI_TERMS = 1000000;

// Вычисление ПИ, суммирует ряд Лейбница
double calc_pi(uint terms)
{
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

// ТЕОРИЯ // TASK 2.2.3 //
// Без слова 'volatile' компилятор вправе перенести расчёт за второе чтение часов. 
//  Результат от этого не изменится, но замер измерит пустое место. 
//  Запись в volatile компилятор не переносит, и расчёт остаётся между двумя чтениями. 
volatile double pi_result;

// печатает значение π и время расчёта в миллисекундах
void cmd_calc_pi(void)
{
    uint64_t start_us = time_us_64();
    pi_result = calc_pi(CALC_PI_TERMS);
    uint64_t spent_us = time_us_64() - start_us;
    printf("pi: %.8f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}
/// CALC_PI /// END ///



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
        //read_line();
        const command_t *test = stdio_text_protocol_handle();
        if (test == NULL) continue;
        if (test->name == "") continue;

        // test //
        LOG_DBG("got %s\n", test->name);  // Печать строки целиком
        LOG_DBG("name      %s\n", test->name);
        LOG_DBG("argc      %d\n", test->argc);
        LOG_DBG(test->argv[0] ? "argv[0]   %s\n" : "argv[0]   %p\n", test->argv[0]);
        LOG_DBG(test->argv[1] ? "argv[1]   %s\n" : "argv[1]   %p\n", test->argv[1]);
        LOG_DBG(test->argv[2] ? "argv[2]   %s\n" : "argv[2]   %p\n", test->argv[2]);
        LOG_DBG(test->argv[3] ? "argv[3]   %s\n" : "argv[3]   %p\n", test->argv[3]);
        LOG_DBG("truncated %d\n", test->truncated);
        LOG_DBG("\n");

        handle_command(test->name);       // расшифровка команды


    }// Суперцикл
}