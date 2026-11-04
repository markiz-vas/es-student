// заголовочный файл протокола api
#include "api.h"
// заголовочный файл протокола команд
#include "api-commands.h"

// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// для работы со строками, strcmp() и memset()
#include <string.h>
// заголовочный файл для лога ошибок #include "logging\log.h"
#include "log.h"

// Локальная функция вызывающая список всех команд
static void help(void)
{
    // printf("help\n");
    printf("%-16s %s\n", "help", "list of commands");

    // Вывод всех команд
    for (uint32_t i = 0; i < api_command_count; i++) {
        printf("%-16s %s\n", api_commands[i].name, api_commands[i].help);
    }
}

// проверяет команду в том порядке, который разобран в т2.3.8
// в main.c идет проверка: if (command != NULL) api_handle(command);
void api_handle(const command_t *command)
{
    // Проверка на переполнение
    if (command->truncated == true) {
        // Было переполнение, данной команде доверять нельзя
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }

    // Вывод справки
    if (strcmp(command->name, "help") == 0) {
        // Печать списка команд
        help(); 
        return;
    }

    // Поиск команды
    for (uint32_t i = 0; i < api_command_count; i++) {
        // Проверка по списку команд
        if (strcmp(command->name, api_commands[i].name) == 0) {
            // Вызов команды
            if (api_commands[i].callback != NULL) { // ссылка на функцию не пустая
                api_commands[i].callback(command);  // вызов функции по указателю
            } else {
                LOG_ERR("NULL callback, command->name: %s\n", command->name);
            }// NULL
            return; // Ранний выход из цикла, если нашлась команда в массиве
        }
    }//for

    // Нераспознаная команда
    printf("error: unknown command '%s', try help\n", command->name);
}


// Печатает структуру команды (command != NULL)
void api_print_debug_command(const command_t *command)
{
    // test //
    LOG_DBG("name      %s\n",           command->name);
    LOG_DBG("argc      %u\n", (unsigned)command->argc);
    LOG_DBG(command->argv[0] ? "argv[0]   %s\n" : "argv[0]   %p\n", command->argv[0]);
    LOG_DBG(command->argv[1] ? "argv[1]   %s\n" : "argv[1]   %p\n", command->argv[1]);
    LOG_DBG(command->argv[2] ? "argv[2]   %s\n" : "argv[2]   %p\n", command->argv[2]);
    LOG_DBG(command->argv[3] ? "argv[3]   %s\n" : "argv[3]   %p\n", command->argv[3]);
    LOG_DBG("truncated %d\n", command->truncated);
    LOG_DBG("\n");
    // test //
}

