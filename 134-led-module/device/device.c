// заголовочный файл лога информации о приборе #include "device\device.h"
#include "device.h"
// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>

// Читаем серийный номер платы
#include "pico/unique_id.h"
// В этих файлах лежат адреса регистров
#include "hardware/regs/addressmap.h"   // SYSINFO_BASE
#include "hardware/regs/sysinfo.h"      // смещение регистра и имена масок


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

    // печатаем пять строк паспорта
    printf("project: %s\n"  , DEVICE_PROJECT);
    printf("repo: %s\n"     , DEVICE_REPO   );
    printf("board: %s\n"    , DEVICE_BOARD  );
    printf("serial: %s\n"   , board_id      );
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", manufacturer, part, revision);
}