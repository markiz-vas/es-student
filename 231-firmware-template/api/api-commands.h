// api-commands.h //Заголовочный файл информации Структура с названием и ссылкой на функцию команды

//Защита от повторного включения (англ. include guard) 
#pragma once

// заголовочный файл протокола обмена текстовыми командами
#include "stdio-text-protocol.h"
// ради типа uint: uint — не язык C, а имя из Pico SDK.
#include "pico/types.h"

// обработчик команды: получает имя и аргументы
typedef void (*api_callback_t)(const command_t *command);

// Тип - Структура
// команда прибора: имя, справка для help и обработчик
typedef struct {
    const char *name;           // name — имя, по которому команда ищется в таблице (правило 2);
    const char *help;           // help — краткая справка, которую печатает команда help (правило 7);
    api_callback_t callback;    // callback — обработчик, который выполняет команду и печатает ответ (правила 5 и 6).
} api_command_t;


// Массив команд для декодирования COM-порта
extern const api_command_t api_commands[];
// Общее количество команд (вычисляется и сохраняется в константу)
extern const uint api_command_count;