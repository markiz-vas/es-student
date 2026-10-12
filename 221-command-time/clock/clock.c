// заголовочный файл модуля часов
#include "clock.h"
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

