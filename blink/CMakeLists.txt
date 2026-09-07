# Минимальная версия CMake
cmake_minimum_required(VERSION 3.15)

# Плата и микроконтроллер. 
# Специфика Pico: по этим двум значениям SDK выберет нужные библиотеки 
# и загрузчик под микросхему памяти, распаянную на плате
set(PICO_BOARD pico_w)
set(PICO_PLATFORM rp2040)

# Подключение SDK. Сначала подключается сценарий поиска, 
# затем объявляется проект, затем вызывается инициализация — 
# после неё становятся доступны библиотеки и команды производителя.
include(pico_sdk_import.cmake)

# Строка project — типовая, она задаёт имя проекта.
project(blink)

# Порядок важен: pico_sdk_init() вызывается после объявления проекта.
pico_sdk_init()

# Из чего собирать. 
# Типовая команда: создать цель сборки с именем 112_blink 
# из перечисленных исходных файлов.
add_executable(blink
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# С какими библиотеками. Тоже типовая: указать, на что опирается ваш код. 
# pico_stdlib — основная библиотека SDK, 
# из неё берутся gpio_init, gpio_put, sleep_ms.
target_link_libraries(blink
    pico_stdlib              # for core functionality
    pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
)
# if (PICO_CYW43_SUPPORTED)
#     target_link_libraries(blink 
#         pico_cyw43_arch_none)
# endif()

# По каким правилам размещать. Специфика микроконтроллера: 
# указать скрипт компоновщика, скопированный на втором шаге.
pico_set_linker_script(blink
   ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld
)

# В каких форматах выдать результат. 
# Специфика Pico: кроме основного образа получить файлы .bin и .uf2 — 
# последний понадобится для записи прошивки в плату.
pico_add_extra_outputs(blink)



#add_executable(blink blink.c)

# pull in common dependencies
#target_link_libraries(blink pico_stdlib)

#if (PICO_CYW43_SUPPORTED)
#    target_link_libraries(blink pico_cyw43_arch_none)
#endif()

# create map/bin/hex file etc.
#pico_add_extra_outputs(blink)

# add url via pico_set_program_url
#example_auto_set_url(blink)