// Защита от повторного включения (англ. include guard)
#pragma once

// ради типа uint: uint — не язык C, а имя из Pico SDK.
#include "pico/types.h"



// Общая сигнатура функции (нужна для создания списка команд)
typedef void (*command_handler_t)(void);
// ТЕОРИЯ //
// Читается запись изнутри наружу: (*command_handler_t) — это указатель, 
//  void (…)(void) — на функцию без аргументов и без возвращаемого значения. 
//  Теперь command_handler_t — такой же тип, как uint32_t, 
//  только переменная этого типа хранит адрес функции.

// Структура с названием и ссылкой на функцию команды
struct old_command_t
{
    const char *name;           // имя команды в консоле
    command_handler_t handler;  // фунфкция обработчик
};

// Ссылка на объявленный в main.c массив команд
extern const struct old_command_t old_commands[];
// Ссылка на объявленный в main.c количество команд
extern const uint old_command_count;


// ТЕОРИЯ //
// Без него const uint old_command_count; в заголовке — не объявление, а определение: 
//  переменная заводится в каждом файле, который заголовок включил, 
//  и компоновщик остановится на multiple definition. 
//  Слово extern говорит «она где-то есть», не отводя памяти.

/// TASK 2.3.1 /// заменил имена <command_t>     на <old_command_t>
/// TASK 2.3.1 /// заменил имена <command_count> на <old_command_count>
/// TASK 2.3.1 /// заменил имена <commands>      на <old_commands>
/// TASK 2.3.1 /// изменения имен затрагивают <main.c>