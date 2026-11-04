// заголовочный файл лога информации о приборе #include "device\device.h"
#include "device.h"
// заголовочный файл для лога ошибок
#include "log.h"
//device// // для подключения макроса offsetof
//device// #include <stddef.h>

// Читаем серийный номер платы
#include "pico/unique_id.h"
// Читаем версию SDK
#include "pico/version.h"

// В этих файлах лежат адреса регистров
#include "hardware/regs/addressmap.h"   // SYSINFO_BASE
#include "hardware/regs/sysinfo.h"      // смещение регистра и имена масок
// // ради PICO_UNIQUE_BOARD_ID_SIZE_BYTES
// #include "pico/unique_id.h"

// Переменная - константа, на неё будет даваться ссылка
static char board_id_info[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1]; // серийный номер платы


// Возвращает информацию о плате
void device_get_info(device_info_t *info)
{
    // Документация // rp2040-datasheet //
    // SYSINFO_BASE 0x40000000
    // The sysinfo registers start at a base address of 0x40000000 (defined as SYSINFO_BASE in SDK).
    // 0x00 CHIP_ID       - JEDEC JEP-106 compliant chip identifier.
    // 0x04 PLATFORM      - Platform register. Allows software to know what environment it is running in.
    // 0x40 GITREF_RP2040 - Git hash of the chip source. Used to identify chip version.
    // Слово volatile здесь по той же причине, что и в первом задании занятия 1.2: за адресом не переменная, а регистр устройства.

    // читаем серийный номер платы функцией SDK
    pico_get_unique_board_id_string(board_id_info, sizeof(board_id_info));
    info->board_id = &board_id_info[0];
       
    // читаем регистр CHIP_ID по адресу и разбираем три поля
    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    uint32_t id = *chip_id;

    // получаем значение из регистра
    // Читается так: маска оставляет от слова только разряды нужного поля, сдвиг переносит их в младшие разряды. 
    info->manufacturer  = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    info->part          = (id & SYSINFO_CHIP_ID_PART_BITS)         >> SYSINFO_CHIP_ID_PART_LSB        ;
    info->revision      = (id & SYSINFO_CHIP_ID_REVISION_BITS)     >> SYSINFO_CHIP_ID_REVISION_LSB    ;

    // Тип платы и версия SDK
    info->board         = DEVICE_BOARD;
    info->pico_sdk      = PICO_SDK_VERSION_STRING;
}


// Возвращает информацию о плате
void project_get_info(project_info_t *info)
{
    // Название модуля и его версия
    info->device_name       = DEVICE_NAME       ; // Имя проекта      // "es-cmd-usb"
    info->firmware_version  = FIRMWARE_VERSION  ; // Версия проекта   // "1.0.0"

    // Информация о проекте
    info->device_project    = DEVICE_PROJECT    ; // Название проекта (папка)
    info->device_repo       = DEVICE_REPO       ; // Расположение проекта (сайт, github)
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




// // Печатает в консоль информацию о плате
// void device_info(void)
// {
//     // читаем серийный номер платы функцией SDK
//     char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
//     pico_get_unique_board_id_string(board_id, sizeof(board_id));

//     // Документация // rp2040-datasheet //
//     // SYSINFO_BASE 0x40000000
//     // The sysinfo registers start at a base address of 0x40000000 (defined as SYSINFO_BASE in SDK).
//     // 0x00 CHIP_ID       - JEDEC JEP-106 compliant chip identifier.
//     // 0x04 PLATFORM      - Platform register. Allows software to know what environment it is running in.
//     // 0x40 GITREF_RP2040 - Git hash of the chip source. Used to identify chip version.
//     // Слово volatile здесь по той же причине, что и в первом задании занятия 1.2: за адресом не переменная, а регистр устройства.

//     // читаем регистр CHIP_ID по адресу и разбираем три поля
//     volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
//     uint32_t id = *chip_id;

//     // получаем значение из регистра
//     // Читается так: маска оставляет от слова только разряды нужного поля, сдвиг переносит их в младшие разряды. 
//     uint32_t manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
//     uint32_t revision     = (id & SYSINFO_CHIP_ID_REVISION_BITS)     >> SYSINFO_CHIP_ID_REVISION_LSB    ;
//     uint32_t part         = (id & SYSINFO_CHIP_ID_PART_BITS)         >> SYSINFO_CHIP_ID_PART_LSB        ;

//     // печатаем пять строк паспорта (уже больше)
//     printf("name: %s\n"     , DEVICE_NAME       );
//     printf("version: %s\n"  , FIRMWARE_VERSION  );
//     printf("project: %s\n"  , DEVICE_PROJECT    );
//     printf("repo: %s\n"     , DEVICE_REPO       );
//     printf("board: %s\n"    , DEVICE_BOARD      );
//     printf("serial: %s\n"   , board_id          );
//     printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", 
//             (unsigned)manufacturer, (unsigned)part, (unsigned)revision);
//     printf("pico-sdk: %s\n" , PICO_SDK_VERSION_STRING);
//     printf("TASK: %s\n"     , TASK_INFO);
// }



//device// // Глобальная переменная с информацией о приборе
//device// struct info_t device_card = { 
//device//     .revision = 2,              // Заполнено от балды, с "uint32_t revision" не связано
//device//     .version  = 0x00010000,     // FIRMWARE_VERSION "1.0.0", по байту на версию
//device//     .name     = DEVICE_NAME     // Взято из device.h (имя ограничено 12 символами + '\0')
//device// };

//device// // Печатает в консоль информацию о структуре info_t (раскладка в памяти)
//device// void dev_info(void) 
//device// {
//device//     // шапка: объект, адрес, значение
//device//     printf("%-15s %-10s %5s %6s %-10s\n", 
//device//             "struct", "address", "size", "offset", "value");
//device// 
//device//     // Адрес переменной
//device//     printf("%-15s 0x%08x %5u\n",
//device//            "device_card", &device_card, sizeof(device_card));
//device// 
//device//     // ТЕОРИЯ // TASK 2.1.5 //
//device//     // При последовательности revision, version, name размер структуры 24
//device//     // При последовательности version, name, revision размер структуры 20
//device// 
//device//     // Поле версия (version)
//device//     printf("- %-13s 0x%08x %5u %6u 0x%08x\n",
//device//            "version",                           // имя поля
//device//            &device_card.version,                // адрес поля
//device//            sizeof(device_card.version),         // размер поля
//device//            offsetof(struct info_t, version),    // Размер в структуре
//device//            device_card.version);                // Значение поля
//device//     
//device//     // Поле версия (name)
//device//     printf("- %-13s 0x%08x %5u %6u %s\n",
//device//            "name",                              // имя поля
//device//            device_card.name,                    // адрес поля
//device//            sizeof(device_card.name),            // размер поля
//device//            offsetof(struct info_t, name),       // Размер в структуре
//device//            device_card.name);                   // Значение поля
//device// 
//device//     // Поле версия (revision)
//device//     printf("- %-13s 0x%08x %5u %6u %u\n",
//device//            "revision",                          // имя поля
//device//            &device_card.revision,               // адрес поля
//device//            sizeof(device_card.revision),        // размер поля
//device//            offsetof(struct info_t, revision),   // Размер в структуре
//device//            device_card.revision);               // Значение поля
//device//     
//device// 
//device//     // ТЕОРИЯ // TASK 2.1.5 //
//device//     // offsetof(тип, поле) возвращает расстояние в байтах от начала структуры до поля. 
//device//     //  Считать его вручную — верный способ ошибиться: компилятор вправе расставить поля не так, 
//device//     //  как вы предполагаете, и offsetof говорит, как он это сделал на самом деле.
//device//     // ТЕОРИЯ // TASK 2.1.5 //
//device//     //  У двух полей из трёх адрес берётся оператором &, а у массива — нет: 
//device//     //  device_card.name и есть его адрес, & к нему не нужен. 
//device//     //  Зато sizeof(device_card.name) даёт тринадцать — полный размер массива, 
//device//     //  а не размер указателя и не длину строки внутри него. 
//device//     //  Это две разные величины, и путать их дорого: strlen() вернул бы десять.
//device// 
//device//     // Размеры структуры (info_t)
//device//     unsigned size_info_t = sizeof(device_card);
//device//     unsigned size_fields = sizeof(device_card.version) + sizeof(device_card.name) + sizeof(device_card.revision);
//device//     // Печать данных о памяти и размещении полей в структуре
//device//     printf("%s %u, %s %u, %s %u\n",
//device//            "fields", size_fields, "sizeof", size_info_t, "padding", size_info_t - size_fields);
//device// }

