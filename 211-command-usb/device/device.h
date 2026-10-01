//Защита от повторного включения (англ. include guard) 
#pragma once

// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Название модуля и его версия
// #define DEVICE_NAME "es-led-module" // для 12 байт имя "es-led-module" великовато (нужно 14 байт)
#define DEVICE_NAME "es-cmd-usb"
#define FIRMWARE_VERSION "1.0.0"
#define TASK_INFO "п2.1.5 Память массивов и структур (TASK p.2.1.5)"

// Информация о проекте
#define DEVICE_PROJECT "211-command-usb"
#define DEVICE_REPO "https://github.com/markiz-vas/es-student"

// Чтобы не было ошибки с макросом (если он не задан в CMakeLists.txt)
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Чтобы другие модули могли увидеть переменную, созданную в device.c
extern struct info_t device_card;

// Тестовая структура с данными (TASK 2.1.5)
//  Порядок полей здесь не случайный, а нарочно неудачный: 
//  маленькое поле, за ним большое, за ним массив. 
//  Именно на нём видно то, ради чего задание сделано.
// name - Тринадцать символов: двенадцать на имя и один на завершающий ноль строки.
struct info_t
{
    uint32_t version;
    char name[13];      
    uint8_t revision;
};
// ТЕОРИЯ // TASK 2.1.5 //
// При последовательности revision, version, name размер структуры 24
// При последовательности version, name, revision размер структуры 20


// Печатает в консоль информацию о плате
void device_info(void);

// Печатает в консоль информацию о структуре info_t (раскладка в памяти)
void dev_info(void);