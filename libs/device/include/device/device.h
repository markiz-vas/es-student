//device.h // заголовочный файл лога информации о приборе #include "device\device.h"
#pragma once

// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Название модуля и его версия
// #define DEVICE_NAME "es-led-module" // для 12 байт имя "es-led-module" великовато (нужно 14 байт)
#define DEVICE_NAME "es-cmd-usb"
#define FIRMWARE_VERSION "1.2.0"

// Информация о проекте
#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/markiz-vas/es-student"
#define TASK_INFO "п2.3.5 Данные вместо печати (TASK p.2.3.5)"

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

// тип со сведениями о проекте: Имя, сайт, задание, время и др.
typedef struct
{
    char *device_name       ; // Имя проекта      // "es-cmd-usb"
    char *firmware_version  ; // Версия проекта   // "1.0.0"
    char *device_project    ; // Название проекта (папка)
    char *device_repo       ; // Расположение проекта (сайт, github)
    char *task_info         ; // Информация о задании (ant-lab)
    char *date_build_info   ; // Дата сборки проекта
    char *time_build_info   ; // Время сборки проекта
    char *log_level_info    ; // Уровень лога (LOG_LEVEL) в проекте
} project_info_t;

// Возвращает информацию о проекте
void project_get_info(project_info_t *info);


