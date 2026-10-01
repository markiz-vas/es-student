// заголовочный файл информации о памяти устройства // заголовочный файл информации о памяти устройства #include "memory\memory.h"
#include "memory.h"
// заголовочный файл информации Структура с названием и ссылкой на функцию команды
#include "command.h" // #include "..\command.h"
// заголовочный файл лога информации о приборе #include "..\device\device.h"
#include "device.h"
// заголовочный файл светодиода
#include "led.h"

// стандартного ввода-вывода — тот же, что в любой программе на C
#include <stdio.h>
// ради типа uintptr_t
#include <stdint.h>
// ради malloc() и free(), иначе ошибка "implicit declaration of function 'malloc'"
#include <stdlib.h>
// с ним приходит размера флеш памяти конкретной платы
#include "pico/stdlib.h"                // PICO_FLASH_SIZE_BYTES (boards\pico.h)
// ради базовых адресов флеш, ОЗУ и ПЗУ
#include "hardware/regs/addressmap.h"   // XIP_BASE, SRAM_BASE, ROM_BASE, SRAM_END

//var2// // Для прямой манипуляции с адресами регистров
//var2// // Числа выписывать не нужно: и базовый адрес, 
//var2// // и смещения уже объявлены в заголовочных файлах SDK
//#include "hardware/regs/addressmap.h"   // SIO_BASE
#include "hardware/regs/sio.h"          // SIO_GPIO_IN_OFFSET

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


// Глобальные переменные для проверки, где лежат переменные (TASK 2.1.4)
uint32_t data_variable = 100;   // .data in flash and RAM
uint32_t bss_variable;          // .bss in RAM




// TASK 2.1.3 //
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
    #define ROM_SIZE_BYTES (16*1024) // размер из документации: 16kB (ROM) is at address 0x00000000
    uintptr_t rom_start  = ROM_BASE;                    // начало блока ROM
    uintptr_t rom_size   = ROM_SIZE_BYTES;              // размер блока ROM
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






// TASK 2.1.4 //
// печать строки данных для функции
static void fw_row_func(const char *obj_name, uintptr_t address)
{
    // адрес функции со сброшенным признаком Thumb
    // uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    uint16_t *addr = (uint16_t *)(address & ~1u);

    printf("%-15s 0x%08x 0x%04x\n",
           obj_name, (unsigned)address, *(uint16_t *)addr);
}

// печать строки данных для структуры
static void fw_row_struct(const char *obj_name, uintptr_t address, bool is_struct_item)
{
    printf(is_struct_item ? "- %-13s 0x%08x\n" : "%-15s 0x%08x\n",
           obj_name, (unsigned)address);
}

// печать строки данных для константы
static void fw_row_const(const char *obj_name, uintptr_t address)
{
    printf("%-15s 0x%08x %s\n",
           obj_name, (unsigned)address, (char *)(uint32_t *)address);
}

// печать строки данных для переменной
static void fw_row_var(const char *obj_name, uintptr_t address)
{
    if ( (uint32_t *)address != NULL )
        printf("%-15s 0x%08x %u\n",
           obj_name, (unsigned)address, (unsigned)*(uint32_t *)address);
}

// Информация о динамической памяти устройства (RAM)
void fw_info(void)
{
    // считаем вызов: data_variable и bss_variable на единицу больше
    data_variable++;    // глобальные переменные // .data
    bss_variable++;     // глобальные переменные // .bss

    // адреса функций со сброшенным признаком Thumb
    int main(void); // делаем локальную ссылку на основную функцию из main.c
    // uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    // ТЕОРИЯ // (TASK 2.1.4)
    // Если прибор перестал отвечать сразу после fw_info 
    //  и лечится только перезаписью — вы читаете по нечётному адресу. 
    //  Проверьте, что & ~1u стоит до приведения к типу указателя, а не после разыменования.
    // ТЕОРИЯ // (TASK 2.1.4)
    // В младшем разряде адреса функции ядро Cortex-M0+ держит признак набора команд Thumb — единицу. 
    //  Для перехода этот адрес правильный: ядро сбрасывает разряд само. 
    //  Для чтения байт он негоден: адрес получается нечётным, 
    //  а прочитать по нечётному адресу слово или полуслово ядро не может и останавливает программу ошибкой HardFault. 
    //  Прошивка при этом перестаёт отвечать, терминал молчит, и понять причину без этого абзаца почти невозможно. 
    //  Операция & ~1u сбрасывает признак, и адрес становится чётным.


    // локальная переменная и блок из кучи
    uint32_t stack_variable = 1946;                         // Значение в стеке
    uint32_t *heap_variable = malloc(sizeof(uint32_t));     // Значение в куче // ручное выделение памяти
    if (heap_variable != NULL) { *heap_variable = 1951; }   // Защита от пустого адреса
    // ТЕОРИЯ // (TASK 2.1.4)
    // 1946 и 1951 нужны для различия значений переменных (только для проверки)
    // Печатайте адрес блока, а не адрес указателя!
    // heap_variable — указатель, и он сам лежит на стеке, рядом со stack_variable. 
    //  Адрес блока в куче — это его значение: печатать нужно heap_variable, 
    //  не &heap_variable. Иначе в таблице вместо кучи окажется ещё один адрес из стека. 
    // ТЕОРИЯ // (TASK 2.1.4)
    // malloc() без парного free() — это утечка памяти: аллокатор считает блок занятым, 
    //  хотя пользоваться им уже некому. В программе на компьютере утечку прикрывает завершение процесса. 
    //  Прошивка не завершается никогда, и утечка в суперцикле рано или поздно «доест» всю кучу. 

    // шапка: объект, адрес, значение
    printf("%-15s %-10s %-s\n", "object", "address", "value");

    // main, fw_info  — адрес с признаком Thumb и два байта по сброшенному адресу
    fw_row_func("main"   , (uintptr_t)main   ); // .text
    fw_row_func("fw_info", (uintptr_t)fw_info); // .text

    // commands       — адрес массива
    fw_row_struct("commands", (uintptr_t)&commands, false); // .text

    // обработчики    — имя команды и адрес обработчика, строкой на команду
    for (uint i = 0; i < command_count; i++) {  // .text
        fw_row_struct(commands[i].name, (uintptr_t)commands[i].handler, true);
    }

    // константы      — адрес и значение строк паспорта из device.h
    fw_row_const("DEVICE_PROJECT", (uintptr_t)DEVICE_PROJECT); // .rodata
    fw_row_const("DEVICE_BOARD"  , (uintptr_t)DEVICE_BOARD  ); // .rodata
    
    // data_variable  — адрес и значение, секция .data
    fw_row_var("data_variable" , (uintptr_t)&data_variable );   // .data flash
    // bss_variable   — адрес и значение, секция .bss
    fw_row_var("bss_variable"  , (uintptr_t)&bss_variable  );   // .bss flash
    // stack_variable — адрес и значение
    fw_row_var("stack_variable", (uintptr_t)&stack_variable);   // .stack
    // heap_variable  — адрес и значение
    fw_row_var("heap_variable" , (uintptr_t)heap_variable  );   // .heap

    // возвращаем блок кучи
    free(heap_variable);    // ручное освобождение (очистка) памяти в куче
}


// Прибор читает память по числам, взятым из таблицы векторов (флеш-память)
void boot_info(void)
{
    // указатель на таблицу векторов и два первых слова из неё
    // #define VECTOR_TABLE 0x10000100     // Адрес фиксирован (uintptr_t)&__boot2_end__)
    // const uint32_t *vectors = (const uint32_t *)VECTOR_TABLE;    // Начало блока .text
    const uint32_t *vectors = (const uint32_t *)(uintptr_t)&__boot2_end__;
    uint32_t stack_top      = vectors[0];
    uint32_t reset_handler  = vectors[1];
    // ТЕОРИЯ // TASK 2.1.5 //
    //  (const uint32_t *)VECTOR_TABLE - Число превращается в указатель приведением типа, 
    //  а дальше с ним работают как с обычным массивом: vectors[0] — первое слово, vectors[1] — второе. 
    //  Тип uint32_t выбран потому, что таблица векторов состоит из адресов, а адрес на этом ядре занимает четыре байта. 
    //  Слово const говорит, что читать будем, а писать не будем: во флеш-память по этому адресу записать всё равно нельзя.
    

    // указатель на регистр GPIO_IN и разряд вывода светодиода
    //var1// // Адреса регистров платы (напрямую без addressmap.h)
    // #define SIO_BASE    0xd0000000
    // #define GPIO_IN     (*(volatile uint32_t *)(SIO_BASE + 0x004))
    //var2// // Адреса регистров платы (с использованием addressmap.h и sio.h)
    volatile uint32_t *gpio_in  = (uint32_t *)(SIO_BASE + SIO_GPIO_IN_OFFSET );
    // В регистре по разряду на каждый вывод, а нужен один — тот, на котором сидит светодиод:
    uint32_t led_level = (*gpio_in >> led_pin()) & 1u;  // Выделение разряда сдвигом и маской
    



    // vector table   — адрес таблицы
    printf("%-15s 0x%08x\n", "vector table", vectors);

    //   stack top    — первое слово
    // printf("%-15s 0x%08x\n", "  stack top", (uintptr_t)&__StackTop);
    printf("%-15s 0x%08x\n", "  stack top", stack_top);

    //   reset        — второе слово
    printf("%-15s 0x%08x\n", "  reset", reset_handler);

    //   reset (even) — оно же со сброшенным признаком Thumb
    uint16_t *reset_handler_Thumb = (uint16_t *)((uintptr_t)reset_handler & ~1u);
    printf("%-15s 0x%08x\n", "  reset (even)", (unsigned)reset_handler_Thumb);


    // gpio in        — адрес регистра
    printf("%-15s 0x%08x\n", "gpio in", gpio_in);

    //   led bit      — разряд из регистра
    printf("%-15s %u\n", "  led bit", led_level);

    //   gpio_get     — то же значение через SDK
    printf("%-15s %u\n", "  led bit", gpio_get(led_pin()));
    
    // ТЕОРИЯ // TASK 2.1.5 //
    // Одно и то же двумя путями.
    // gpio_get() внутри делает ровно то, что вы написали руками: читает GPIO_IN и выбирает нужный разряд. 
    //  Смысл задания не в том, чтобы обойтись без SDK, а в том, чтобы увидеть, 
    //  что за его функциями нет ничего волшебного — только адрес из документации и разыменование указателя. 
    //  Дальше в курсе вы будете читать так регистры, для которых готовой функции в SDK нет вовсе.
}
