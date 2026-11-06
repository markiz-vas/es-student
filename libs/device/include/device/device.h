//device.h // заголовочный файл лога информации о приборе #include "device\device.h"
#pragma once

// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Чтобы не было ошибки с макросом (если он не задан в CMakeLists.txt)
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif


// // Печатает в консоль информацию о плате // удалить
// void device_info(void);                 


// тип со сведениями о плате: тип платы, серийный номер, производитель, изделие и ревизия кристалла, версия SDK
typedef struct
{
    char    *board       ;  // Тип платы (pico, pico_w)
    char    *board_id    ;  // серийный номер платы
    char    *pico_sdk    ;  // Версия sdk
    uint32_t manufacturer;  // Код фабрики
    uint32_t part        ;  // Партия
    uint32_t revision    ;  // Проверка (ревизия)
} device_info_t;

// Возвращает информацию о плате
void device_get_info(device_info_t *info);
