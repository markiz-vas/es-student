//Защита от повторного включения (англ. include guard) 
#pragma once

// Название модуля и его версия
#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

// Информация о проекте
#define DEVICE_PROJECT "134-led-module"
#define DEVICE_REPO "https://github.com/markiz-vas/es-student"

// Чтобы не было ошибки с макросом (если он не задан в CMakeLists.txt)
#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Печатает в консоль информацию о плате
void device_info(void);

