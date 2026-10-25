//Защита от повторного включения (англ. include guard) 
#pragma once

// // заголовочные файлы для sleep_ms, stdio_init_all
// #include "pico/stdlib.h"

// Модуль часов (инфо)
void clk_info(void);

// Печатает монотонное время прибора, в миллисекундах
void uptime(void);

// Установить пониженную частоту процессора
void clk_sys_low(void);

// Установить частоту прочессора по умолчанию
void clk_sys_default(void);