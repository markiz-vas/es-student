//device.h // заголовочный файл лога информации о приборе #include "device\device.h"
#pragma once

// ради типа uintptr_t, uint32_t, uint16_t, uint8_t
#include <stdint.h>

// Название модуля и его версия
// #define DEVICE_NAME "es-led-module" // для 12 байт имя "es-led-module" великовато (нужно 14 байт)
#define DEVICE_NAME "es-cmd-usb"
#define FIRMWARE_VERSION "1.2.0"
#define TASK_INFO "п2.3.4 Расчёт по шагам (TASK p.2.3.4)"

// Информация о проекте
#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/markiz-vas/es-student"

// Чтобы не было ошибки с макросом (если он не задан в CMakeLists.txt)
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Печатает в консоль информацию о плате
void device_info(void);


//device// // Чтобы другие модули могли увидеть переменную, созданную в device.c
//device// extern struct info_t device_card;

//device// // Тестовая структура с данными (TASK 2.1.5)
//device// //  Порядок полей здесь не случайный, а нарочно неудачный: 
//device// //  маленькое поле, за ним большое, за ним массив. 
//device// //  Именно на нём видно то, ради чего задание сделано.
//device// // name - Тринадцать символов: двенадцать на имя и один на завершающий ноль строки.
//device// struct info_t
//device// {
//device//     uint32_t version;
//device//     char name[13];      
//device//     uint8_t revision;
//device// };
//device// // ТЕОРИЯ // TASK 2.1.5 //
//device// // При последовательности revision, version, name размер структуры 24
//device// // При последовательности version, name, revision размер структуры 20

//device// // Печатает в консоль информацию о структуре info_t (раскладка в памяти)
//device// void dev_info(void);

