// заголовочный файл лога информации о приборе #include "device\device.h"
#include "device.h"
// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// для подключения макроса offsetof
#include <stddef.h>

// Читаем серийный номер платы
#include "pico/unique_id.h"
// Читаем версию SDK
#include "pico/version.h"

// В этих файлах лежат адреса регистров
#include "hardware/regs/addressmap.h"   // SYSINFO_BASE
#include "hardware/regs/sysinfo.h"      // смещение регистра и имена масок


// Глобальная переменная с информацией о приборе
struct info_t device_card = { 
    .revision = 2,              // Заполнено от балды, с "uint32_t revision" не связано
    .version  = 0x00010000,     // FIRMWARE_VERSION "1.0.0", по байту на версию
    .name     = DEVICE_NAME     // Взято из device.h (имя ограничено 12 символами + '\0')
};

// Печатает в консоль информацию о плате
void device_info(void)
{
    // читаем серийный номер платы функцией SDK
    char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    pico_get_unique_board_id_string(board_id, sizeof(board_id));

    // Документация // rp2040-datasheet //
    // SYSINFO_BASE 0x40000000
    // The sysinfo registers start at a base address of 0x40000000 (defined as SYSINFO_BASE in SDK).
    // 0x00 CHIP_ID       - JEDEC JEP-106 compliant chip identifier.
    // 0x04 PLATFORM      - Platform register. Allows software to know what environment it is running in.
    // 0x40 GITREF_RP2040 - Git hash of the chip source. Used to identify chip version.
    // Слово volatile здесь по той же причине, что и в первом задании занятия 1.2: за адресом не переменная, а регистр устройства.

    // читаем регистр CHIP_ID по адресу и разбираем три поля
    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    uint32_t id = *chip_id;

    // получаем значение из регистра
    // Читается так: маска оставляет от слова только разряды нужного поля, сдвиг переносит их в младшие разряды. 
    uint32_t manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    uint32_t revision     = (id & SYSINFO_CHIP_ID_REVISION_BITS)     >> SYSINFO_CHIP_ID_REVISION_LSB    ;
    uint32_t part         = (id & SYSINFO_CHIP_ID_PART_BITS)         >> SYSINFO_CHIP_ID_PART_LSB        ;

    // печатаем пять строк паспорта (уже больше)
    printf("project: %s\n"  , DEVICE_PROJECT);
    printf("repo: %s\n"     , DEVICE_REPO   );
    printf("board: %s\n"    , DEVICE_BOARD  );
    printf("serial: %s\n"   , board_id      );
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", manufacturer, part, revision);
    printf("pico-sdk: %s\n" , PICO_SDK_VERSION_STRING);
    printf("TASK: %s\n"     , TASK_INFO);
}



// Печатает в консоль информацию о структуре info_t (раскладка в памяти)
void dev_info(void) 
{
    // шапка: объект, адрес, значение
    printf("%-15s %-10s %5s %6s %-10s\n", 
            "struct", "address", "size", "offset", "value");

    // Адрес переменной
    printf("%-15s 0x%08x %5u\n",
           "device_card", &device_card, sizeof(device_card));

    // ТЕОРИЯ // TASK 2.1.5 //
    // При последовательности revision, version, name размер структуры 24
    // При последовательности version, name, revision размер структуры 20

    // Поле версия (version)
    printf("- %-13s 0x%08x %5u %6u 0x%08x\n",
           "version",                           // имя поля
           &device_card.version,                // адрес поля
           sizeof(device_card.version),         // размер поля
           offsetof(struct info_t, version),    // Размер в структуре
           device_card.version);                // Значение поля
    
    // Поле версия (name)
    printf("- %-13s 0x%08x %5u %6u %s\n",
           "name",                              // имя поля
           device_card.name,                    // адрес поля
           sizeof(device_card.name),            // размер поля
           offsetof(struct info_t, name),       // Размер в структуре
           device_card.name);                   // Значение поля

    // Поле версия (revision)
    printf("- %-13s 0x%08x %5u %6u %u\n",
           "revision",                          // имя поля
           &device_card.revision,               // адрес поля
           sizeof(device_card.revision),        // размер поля
           offsetof(struct info_t, revision),   // Размер в структуре
           device_card.revision);               // Значение поля
    

    // ТЕОРИЯ // TASK 2.1.5 //
    // offsetof(тип, поле) возвращает расстояние в байтах от начала структуры до поля. 
    //  Считать его вручную — верный способ ошибиться: компилятор вправе расставить поля не так, 
    //  как вы предполагаете, и offsetof говорит, как он это сделал на самом деле.
    // ТЕОРИЯ // TASK 2.1.5 //
    //  У двух полей из трёх адрес берётся оператором &, а у массива — нет: 
    //  device_card.name и есть его адрес, & к нему не нужен. 
    //  Зато sizeof(device_card.name) даёт тринадцать — полный размер массива, 
    //  а не размер указателя и не длину строки внутри него. 
    //  Это две разные величины, и путать их дорого: strlen() вернул бы десять.

    // Размеры структуры (info_t)
    unsigned size_info_t = sizeof(device_card);
    unsigned size_fields = sizeof(device_card.version) + sizeof(device_card.name) + sizeof(device_card.revision);
    // Печать данных о памяти и размещении полей в структуре
    printf("%s %u, %s %u, %s %u\n",
           "fields", size_fields, "sizeof", size_info_t, "padding", size_info_t - size_fields);
}