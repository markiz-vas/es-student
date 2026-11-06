//firmware.h // заголовочный файл паспорта прошивки
#pragma once

// Название модуля и его версия
#define FIRMWARE_NAME "es-cmd-usb"
#define FIRMWARE_VERSION "1.2.0"

// Информация о проекте
#define FIRMWARE_PROJECT "231-firmware-template"
#define FIRMWARE_REPO "https://github.com/markiz-vas/es-student"
#define TASK_INFO "п2.3.5 Данные вместо печати (TASK p.2.3.5)"

// тип со сведениями о проекте: Имя, сайт, задание, время и др.
typedef struct
{
    char *firmware_name     ; // Имя проекта      // "es-cmd-usb"
    char *firmware_version  ; // Версия проекта   // "1.0.0"
    char *firmware_project  ; // Название проекта (папка)
    char *firmware_repo     ; // Расположение проекта (сайт, github)
    char *task_info         ; // Информация о задании (ant-lab)
    char *date_build_info   ; // Дата сборки проекта
    char *time_build_info   ; // Время сборки проекта
    char *log_level_info    ; // Уровень лога (LOG_LEVEL) в проекте
} project_info_t;

// Возвращает информацию о проекте
void project_get_info(project_info_t *info);


