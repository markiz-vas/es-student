//Защита от повторного включения (англ. include guard) 
#pragma once

// заголовочный файл протокола обмена текстовыми командами
#include "stdio-text-protocol.h"

// проверяет команду в том порядке, который разобран в т2.3.8
void api_handle(const command_t *command);

// Печатает команду (command != NULL)
void api_print_debug_command(const command_t *command);