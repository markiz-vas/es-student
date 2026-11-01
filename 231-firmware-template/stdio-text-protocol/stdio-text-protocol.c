// заголовочный файл протокола обмена текстовыми командами
#include "stdio-text-protocol.h"
// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint
#include "pico/stdlib.h"
// ради команды memset
#include <string.h>
// ради типа uint: uint — не язык C, а имя из Pico SDK.
// #include "pico/types.h"



// Буфер и его длина (64 символа) (статическая переменная)
static char line[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint line_length;

// Локальная статическая переменная, которая хранит считанные из канала команды
// static - одна переменная, адрес её постоянен
static command_t command;

// Очистка структуры команд
void stdio_text_protocol_default_command_t(command_t *clear_command){
    // clear_command->name = NULL;
    // clear_command->argc = 0;
    // clear_command->argv[0] = NULL;
    // clear_command->argv[1] = NULL;
    // clear_command->argv[2] = NULL;
    // clear_command->argv[3] = NULL;
    // clear_command->truncated = false;

    // Обнуление всех байт структуры
    memset(clear_command, 0, sizeof(*clear_command));
}

// Конструктор протокола (инициализация)
void stdio_text_protocol_init(void){
    line[0] = '\0';     // статические переменные
    line_length = 0;    // статические переменные

    // Устанавливаем значения по умолчанию (очистка command)
    stdio_text_protocol_default_command_t(&command);
}

// Читает один символ, не ожидая его. Когда строка закончена, возвращает
// команду, иначе NULL. Команда действительна до следующего вызова.
const command_t *stdio_text_protocol_handle(void)
{
    // читаем символ без ожидания; нет символа — возвращаем NULL
    // Считали один символ из строки
    int symbol = getchar_timeout_us(0);
    if (symbol == PICO_ERROR_TIMEOUT) {
        return NULL; // Если пусто - ранний выход
    }

    // Обнуление структуры команды при нулевом символе в буфере
    if (line_length == 0) stdio_text_protocol_default_command_t(&command);

    // конец строки: пустую строку пропускаем, иначе переводим строку в терминале,
    // Проверка на конец строки
    if (symbol == '\r' || symbol == '\n')    {
        // Пустая строка (нет символов перед CR, LF)
        if (line_length == 0) return NULL;
        
        // Окончание строки
        putchar('\n'); // Печать в консоль
        line[line_length] = '\0';   // символ конца строки (обязательный)

        // делим строку на слова и возвращаем команду
        // к делению приступаем только после CR, CL
        // во всех остальных случаях идет наполнение буфера (line)

        // Начальные значения для анализа
        bool new_arg = true;    // У нас будет новый аргумент
        command.argc = 0;       // Количество аргументов

        // Анализ Буфера
        for (uint i = 0; i <= line_length; i++) 
        {
            // Проверка на SP
            if (line[i] == '\0') {
                // Пропускаем с пометкой, что дальше будет новый аргумент
                new_arg = true;
                continue; // к следующему символу
            } else 
            { // Не SP
                if (new_arg == true) {
                    // Увеличиваем количество аргументов и идем дальше
                    command.argc++;
                    new_arg = false;
                }
            }

            // Игнорируем 0 аргументов
            if (command.argc == 0) continue; // к следующему символу

            // Первый аргумент - команда
            if (command.argc == 1) {
                // Если команда не определена
                if (command.name == NULL) {
                    // Указатель на символ в буфере, с которого начинается команда
                    command.name = &line[i];
                }
            }// команда 
            else 
            {// != 1
                // Количество аргументов (1 -- имя; 2-5 -- параметры; 0 -- пустая строка; >5 -- слишком много аргументов)
                if (command.argc <= STDIO_TEXT_PROTOCOL_MAX_ARGS + 1)
                {// Количество аргументов до 5
                    // argc[2-5] - argv[0-3]
                    if (command.argv[command.argc - 2] == NULL)     //Параметр не определён
                        command.argv[command.argc - 2] = &line[i];  //Ссылка на буфер
                } else {
                    // Количество аргументов больше 5 - переполнение
                    command.truncated = true;
                    // Дальнейший разбор буфера бесполезен
                    break;  // выход из цикла
                }
            }// != 1
        }//for

        // После выполнения команды обнуляем строку с командой
        line_length = 0;    

        // Возвращаем команду если есть хотя бы один аргумент
        if (command.argc > 0) return &command;
        else return NULL;
    }//CR, CL

    // Backspace (0x08 или 0x7f): убираем последний символ и стираем его на экране "\b \b"
    if (symbol == '\x08' || symbol == '\x7f') {
        // Если есть хотя бы один символ
        if (line_length > 0) {
            // Уменьшаем длину строки
            line_length--;

            // Корректировка символов в буфере
            putchar('\b');  // Возврат корректки
            putchar(' ' );  // Перезатираем последний символ пробелом 
            putchar('\b');  // Возврат корректки
        }// >0

        // Проверка на корректность буфера (нет переполнения)
        if (command.truncated) { 
            // Есть переполнение - после Backspace пометка снимается
            command.truncated = false; 
        }

        return NULL;
    }

    // печатный символ: кладём в буфер и повторяем в терминал;
    // Проверяем, что есть место под конечный символ ('\0')
    // Длина строки не больше 62 (63 символ обязательно '\0' иначе переполнение)
    if (line_length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)    {
        // Складываем символ в буфер (line)
        if (symbol == ' ') line[line_length] = '\0';// команда окончания строки в типе char*
        else line[line_length] = (char)symbol;      // собираем строку из char

        line_length++;   // увеличиваем длину строки
        
        // ТЕОРИЯ //
        // Возврат символа в терминал. putchar(symbol) печатает то, что вы набрали. 
        // Прошивка обязана это делать сама: терминал показывает не ваши нажатия, 
        // а то, что ответил прибор. Без этой строки набор идёт вслепую.
        putchar(symbol); // Печать в консоль

        return NULL;
    } else {
        // буфер полон — помечаем строку как переполненную, 
        // пришедший символ игнорируется (теряется)
        command.truncated = true;
        return NULL; 

        // line_length = 0; //stasy - так и не понял, зачем это?
        // return &command; //stasy - так и не понял, зачем это?
    }

    return NULL;
}
