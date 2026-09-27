// заголовочный файл информации о памяти устройства
#include "memory.h"

// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// ради типа uintptr_t
#include <stdint.h>
// с ним приходит размера флеш памяти конкретной платы
#include "pico/stdlib.h"                // PICO_FLASH_SIZE_BYTES (boards\pico.h)
// ради базовых адресов флеш, ОЗУ и ПЗУ
#include "hardware/regs/addressmap.h"   // XIP_BASE, SRAM_BASE, ROM_BASE, SRAM_END

// Имена с адресами границ памяти (адресов) в устройстве
extern char __flash_binary_start;   // начало образа во флеш
extern char __flash_binary_end;     // конец образа во флеш
extern char __boot2_start__;        // начало 2 загрузчик (настройка XIP для flash)
extern char __boot2_end__;          // конец 2 загрузчик (настройка XIP для flash)
extern char __etext;                // конец .text во flash (начало блока .data)
extern char __data_start__;         // начало .data в ОЗУ (end-start - размер блока .data)
extern char __data_end__;           // конец .data в ОЗУ (end-start - размер блока .data)
extern char __bss_start__;          // начало .bss
extern char __bss_end__;            // конец .bss, здесь же начинается куча
extern char __HeapLimit;            // верхняя граница кучи, конец региона RAM
extern char __StackBottom;          // нижняя граница стека ядра 0
extern char __StackTop;             // вершина стека ядра 0, конец региона SCRATCH_Y

// искал начало блока .rodata - но так и не нашел :(
//extern char __exidx_start;          // Старт блока .ARM.extab (между .rodata и .binary_info)
//extern char __exidx_end;            // Старт блока .ARM.exidx (между .rodata и .binary_info)
//extern char __binary_info_start;    // Старт блока .binary_info (конец блока .rodata)
//extern char __binary_info_end;      // Конец блока .binary_info (__etext; // конец .text во flash )

// Вывод строки с адресом: шапка таблицы: область, начало, конец, размер
static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

// Информация о внутреннем устройстве памяти устройства
void mem_info(void) 
{
    // шапка таблицы: область, начало, конец, размер
    printf("%-10s %-10s %-10s %-8s\n", "area", "start", "end", "size");

    // flash — XIP_BASE и PICO_FLASH_SIZE_BYTES
    uintptr_t flash_start = XIP_BASE;                   // начало блока flash
    uintptr_t flash_size  = PICO_FLASH_SIZE_BYTES;      // размер блока flash
    uintptr_t flash_end   = flash_start + flash_size;   // конец блока flash
    row("flash", (uintptr_t)flash_start, (uintptr_t)flash_end); //+

    // sram — базовый адрес из SDK, размер из datasheet (Static Random-Access Memory)
    uintptr_t sram_start  = SRAM_BASE;                  // начало блока SRAM
    uintptr_t sram_end    = SRAM_END;                   // конец блока SRAM
    row("sram", (uintptr_t)sram_start, (uintptr_t)sram_end); //+

    // rom — базовый адрес из SDK, размер из datasheet (Read-Only Memory)
    #define PICO_ROM_SIZE_BYTES (16*1024) // размер из документации: 16kB (ROM) is at address 0x00000000
    uintptr_t rom_start  = ROM_BASE;                    // начало блока ROM
    uintptr_t rom_size   = PICO_ROM_SIZE_BYTES;         // размер блока ROM
    uintptr_t rom_end    = rom_start + rom_size;        // конец блока ROM
    row("rom", (uintptr_t)rom_start, (uintptr_t)rom_end); //+

    
    // image — от __flash_binary_start до __flash_binary_end
    uintptr_t image_size  = (uintptr_t)&__flash_binary_end - (uintptr_t)&__flash_binary_start; 
    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);//+
    
    // free  — от __flash_binary_end до конца флеш-памяти
    uintptr_t free_size  = (uintptr_t)flash_end - (uintptr_t)&__flash_binary_end; 
    row("free", (uintptr_t)&__flash_binary_end, (uintptr_t)flash_end);//+
    
    // boot2 — от __boot2_start__ до __boot2_end__
    uintptr_t boot2_size  = (uintptr_t)&__boot2_end__ - (uintptr_t)&__boot2_start__; 
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);//+
    
    // text  — от __boot2_end__ до __etext: код и константы
    // text_size = .text + .rodata + .binary_info (ссылки на .rodata найти не удалось)
    uintptr_t text_size  = (uintptr_t)&__etext - (uintptr_t)&__boot2_end__; 
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);//+

    
    // data flash — хранение .data, от __etext, длиной с .data
    uintptr_t data_flash_start = (uintptr_t)&__etext;                   // начало блока .data flash
            // размер не задан, вычисляем из RAM памяти (data_start_end)// размер блока .data flash   
    uintptr_t data_flash_size  = (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__; 
    uintptr_t data_flash_end   = data_flash_start + data_flash_size;    // конец блока .data flash
    row("data flash", (uintptr_t)data_flash_start, (uintptr_t)data_flash_end);//+
    
    // data ram   — работа .data, от __data_start__ до __data_end__
    uintptr_t data_size  = (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__; 
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);//+
    
    // bss        — от __bss_start__ до __bss_end__
    uintptr_t bss_size  = (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__; 
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);//+


    // heap       — от __bss_end__ до __HeapLimit
    uintptr_t heap_size  = (uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__; 
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);//+
    
    // stack      — от __StackBottom до __StackTop
    uintptr_t stack_size  = (uintptr_t)&__StackTop - (uintptr_t)&__StackBottom; 
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);//+


    // ИТОГО
    printf("\ntotal\n");
    // итог: образ во флеш и из чего он сложился
    //uintptr_t flash_image_size  = boot2_size + text_size + data_size;   //test
    //printf("  %-12s %8u\n", "flash image", (unsigned)flash_image_size); //test
    printf("  %-12s %8u = boot2 %u + text %u + data %u\n", 
        "flash image", image_size, boot2_size, text_size, data_size);
    
    // итог: свободно во флеш-памяти из всего её объёма
    printf("  %-12s %8u of %u\n", 
        "flash free", free_size, flash_size);

    // итог: занято в ОЗУ — .data и .bss
    uintptr_t ram_used_size  = data_size + bss_size; 
    printf("  %-12s %8u = data %u + bss %u\n", 
        "ram used", ram_used_size, data_size, bss_size);

    // итог: свободно в ОЗУ — под кучу и под стек
    printf("  %-12s %8u for heap and %u for stack\n", 
        "ram free", heap_size, stack_size);
}