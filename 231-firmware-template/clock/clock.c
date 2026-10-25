// заголовочный файл модуля часов
#include "clock.h"
// заголовочный файл для лога ошибок #include "logging\log.h"
#include "log.h"
// ради printf()
#include <stdio.h>
// ради типов SDK
#include "pico/stdlib.h"
// ради функций работы с тактовыми сигналами 
//  (clk_ref, clk_sys, clk_peri, clk_usb и clk_adc)
#include "hardware/clocks.h"



// Каждая строка таблицы состоит из имени сигнала и двух чисел
static void row(const char *name, uint32_t set_khz, uint32_t measured_khz)
{
    printf("%-8s %9u %12u\n", name, (unsigned)set_khz, (unsigned)measured_khz);
}

// Модуль часов (инфо)
void clk_info(void)
{
    // Функция возвращает частоту в герцах
    uint32_t  ref_hz = clock_get_hz(clk_ref );
    uint32_t  sys_hz = clock_get_hz(clk_sys );
    uint32_t peri_hz = clock_get_hz(clk_peri);
    uint32_t  usb_hz = clock_get_hz(clk_usb );
    uint32_t  adc_hz = clock_get_hz(clk_adc );
    // ТЕОРИЯ // TASK 2.2.1 //
    // Настроенную частоту SDK запоминает и отдаёт функцией clock_get_hz()
    // Настроенная частота записана, а не измерена
    //  clock_get_hz() не смотрит на сигнал. Она возвращает число, 
    //  которое SDK записал, когда настраивал сигнал. 
    //  Если настройка по какой-то причине не удалась, число останется прежним. 
    //  Поэтому в следующем шаге частоту измеряют.

    // Блок измерения частоты считает периоды выбранного сигнала за интервал, 
    // отмеренный по clk_ref. SDK запускает счёт функцией frequency_count_khz(). 
    // Она возвращает частоту в килогерцах
    uint32_t  ref_khz = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_REF );
    uint32_t  sys_khz = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS );
    uint32_t peri_khz = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint32_t  usb_khz = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB );
    uint32_t  adc_khz = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC );
    uint32_t rosc_khz = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    // ТЕОРИЯ // TASK 2.2.1 //
    // Аргумент выбирает измеряемый сигнал. Это код из регистра FC0_SRC. 
    //  Откройте описание регистра в RP2040 Datasheet, раздел «2.15.7. List of Registers», 
    //  таблица 244 и найдите коды пяти сигналов и кольцевого генератора.
    //  В SDK у каждого кода есть константа с тем же значением. Они лежат в файле hardware/regs/clocks.h. 
    // Поиск по файлу в PowerShell:
    //  Select-String FC0_SRC_VALUE "$env:PICO_SDK_PATH\src\rp2040\hardware_regs\include\hardware\regs\clocks.h"
    // #define CLOCKS_FC0_SRC_VALUE_NULL _u(0x00)
    // #define CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY _u(0x01)
    // #define CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY _u(0x02)
    // #define CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC _u(0x03)
    // #define CLOCKS_FC0_SRC_VALUE_CLK_REF _u(0x08)
    // #define CLOCKS_FC0_SRC_VALUE_CLK_SYS _u(0x09)
    // #define CLOCKS_FC0_SRC_VALUE_CLK_PERI _u(0x0a)
    // #define CLOCKS_FC0_SRC_VALUE_CLK_USB _u(0x0b)
    // #define CLOCKS_FC0_SRC_VALUE_CLK_ADC _u(0x0c)
    //  У ROSC строка без настроенной частоты. Это не тактовый сигнал кристалла, 
    //  а генератор. SDK его частоту не настраивает и не хранит.


    // шапка таблицы: сигнал, настроенная частота, измеренная частота
    printf("%-8s %9s %12s\n", "signal", "set_khz", "measured_khz");

    // clk_ref, clk_sys, clk_peri, clk_usb, clk_adc — clock_get_hz() в кГц и frequency_count_khz()
    row("clk_ref" ,  ref_hz / 1000,  ref_khz);
    row("clk_sys" ,  sys_hz / 1000,  sys_khz);
    row("clk_peri", peri_hz / 1000, peri_khz);
    row("clk_usb" ,  usb_hz / 1000,  usb_khz);
    row("clk_adc" ,  adc_hz / 1000,  adc_khz);

    // rosc — прочерк вместо настроенной частоты и измеренная частота
    printf("%-8s %9s %12u\n", "rosc", "-", (unsigned)rosc_khz);

    // TASK т2.2.2// 
    // // Время с момента сброса в микросекундах
    // uint64_t uptime_us = time_us_64();
    // printf("uptime: %llu us\n", uptime_us);
}


// Печатает монотонное время прибора, в миллисекундах
//  Монотонное время прибора ведёт таймер. Он прибавляет единицу раз в микросекунду, 
//  начиная с момента сброса. Показание счёта возвращает функция time_us_64()
void uptime(void) 
{
    // Печатает монотонное время прибора, в миллисекундах
    printf("uptime: %llu ms\n", time_us_64() / 1000);
    //  time_us_64() возвращает uint64_t. Такое число печатают через %llu. 
    //  Деление на 1000 переводит микросекунды в миллисекунды.
}



/// TASK п2.2.5 ///
// Частота 62,5 МГц вдвое ниже рабочей. 
// Системная PLL получает её так же, как 125 МГц, только делит на 6 и ещё на 4. 
// Как подбираются эти делители, сказано во врезке «Дополнительно» в т2.2.2.

// Пониженная частота ядра (в килогерцах)
const uint32_t CLK_SYS_LOW_KHZ = 62500;

// Функция установки частоты ядра
static void clk_sys_set(uint32_t khz)
{
    // Системная функция смены частоты ядра (в килогерцах)
    if (set_sys_clock_khz(khz, false))
    {   // Успешная смена частоты ядра
        LOG_INF("clk_sys %u kHz\n", (unsigned)khz);
    }
    else
    {   // Неудачная смена частоты ядра
        LOG_ERR("clk_sys %u kHz is not set\n", (unsigned)khz);
    }
    /// ТЕОРИЯ /// TASK 2.2.5 //
    // Частоту ядра меняет функция SDK set_sys_clock_khz(). 
    //  Она подбирает множители системной PLL под нужную частоту. 
    //  Не всякую частоту можно получить из 12 МГц кварца. 
    //  Если подобрать множители нельзя, функция вернёт false и ничего не изменит. 
    //  Второй аргумент false говорит SDK, что работу прошивки из-за этого останавливать не нужно.

    /// ТЕОРИЯ /// TASK 2.2.5 // // clk_peri //
    // clk_peri // объяснение странной смены частоты
    // В Git Bash:
    //  grep -n clk_peri "$PICO_SDK_PATH/src/rp2_common/hardware_clocks/clocks.c"
    // В PowerShell:
    //  Select-String clk_peri "$env:PICO_SDK_PATH\src\rp2_common\hardware_clocks\clocks.c"
    // Смотреть строку 415

    // clk_peri //
    // set_sys_clock_khz() вызывает set_sys_clock_pll(), а та переводит clk_peri
    // на PLL для USB, 48 МГц. Так настроено по умолчанию:
    // PICO_CLOCK_ADJUST_PERI_CLOCK_WITH_SYS_CLOCK равен 0.

    // Почему clk_peri остался на 48 МГц
    //  set_sys_clock_pll() сначала переводит clk_sys на PLL для USB. 
    //  Затем она перестраивает системную PLL и возвращает clk_sys на неё. 
    //  clk_peri функция тоже перенастраивает. 
    //  По умолчанию его источником становится PLL для USB с частотой 48 МГц. 
    //  Так UART и SPI не теряют свою частоту, когда меняется частота ядра. 
    //  Поэтому clk_peri остаётся на 48 МГц и после clk_sys_default. 
    //  Вернуть его к частоте ядра можно параметром сборки PICO_CLOCK_ADJUST_PERI_CLOCK_WITH_SYS_CLOCK.
}

// Установить пониженную частоту процессора
void clk_sys_low(void)
{
    // Используем константу 62.5 МГц (наша)
    clk_sys_set(CLK_SYS_LOW_KHZ);
}

// Установить частоту прочессора по умолчанию
void clk_sys_default(void)
{
    // Используем константу 125.0 МГц (системная)
    clk_sys_set(SYS_CLK_KHZ);
    /// ТЕОРИЯ /// TASK 2.2.5 //
    //  Макрос SYS_CLK_KHZ задан в Pico SDK и равен рабочей частоте 125 000 кГц. 
    //  Функция clk_sys_set() остаётся внутри модуля, 
    //  поэтому объявлена со словом static.
}



