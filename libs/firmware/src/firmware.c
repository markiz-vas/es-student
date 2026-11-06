// заголовочный файл паспорта прошивки
#include "firmware/firmware.h"
// заголовочный файл для лога ошибок
#include "logging/log.h"

// Возвращает информацию о плате
void project_get_info(project_info_t *info)
{
    // Название модуля и его версия
    info->firmware_name     = FIRMWARE_NAME     ; // Имя проекта      // "es-cmd-usb"
    info->firmware_version  = FIRMWARE_VERSION  ; // Версия проекта   // "1.0.0"

    // Информация о проекте
    info->firmware_project  = FIRMWARE_PROJECT  ; // Название проекта (папка)
    info->firmware_repo     = FIRMWARE_REPO     ; // Расположение проекта (сайт, github)
    info->task_info         = TASK_INFO         ; // Информация о задании (ant-lab)

    // Дата и время сборки
    info->date_build_info   = __DATE__; // Дата сборки проекта
    info->time_build_info   = __TIME__; // Время сборки проекта

    // Уровень лога (LOG_LEVEL) в проекте
    switch (LOG_LEVEL) {
        case LOG_LEVEL_ERR: info->log_level_info = "err"; break;
        case LOG_LEVEL_INF: info->log_level_info = "inf"; break;
        case LOG_LEVEL_DBG: info->log_level_info = "dbg"; break;
        default:            info->log_level_info = "unknown"; break;
    }//switch
}

