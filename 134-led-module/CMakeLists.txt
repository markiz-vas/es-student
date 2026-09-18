# Минимальная версия CMake
cmake_minimum_required(VERSION 3.15)

# Плата и микроконтроллер. 
# Специфика Pico: по этим двум значениям SDK выберет нужные библиотеки 
# и загрузчик под микросхему памяти, распаянную на плате
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

# Подключение SDK. Сначала подключается сценарий поиска, 
# затем объявляется проект, затем вызывается инициализация — 
# после неё становятся доступны библиотеки и команды производителя.
include(pico_sdk_import.cmake)

# Строка project — типовая, она задаёт имя проекта.
project(134_led_module)

# PROJECT_NAME (англ. project name) — переменная CMake, 
# в которую попадает имя, объявленное командой project(). 
# Запись ${PROJECT_NAME} подставляет имя проекта в любом месте 
# описания сборки, это позволяет задавать имя в файле ровно один раз.

# Порядок важен: pico_sdk_init() вызывается после объявления проекта.
pico_sdk_init()

# Из чего собирать. 
# Типовая команда: создать цель сборки с именем ${PROJECT_NAME} 
# из перечисленных исходных файлов.
add_executable(${PROJECT_NAME}
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
   ${CMAKE_CURRENT_SOURCE_DIR}/led/led.c
   ${CMAKE_CURRENT_SOURCE_DIR}/logging/log.c
)

# Указываем где искать заголовочные файлы
# Слово PRIVATE означает, что путь нужен только при сборке этой цели и никуда дальше не передаётся.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/led
    ${CMAKE_CURRENT_SOURCE_DIR}/logging
)

# С какими библиотеками. Тоже типовая: указать, на что опирается ваш код. 
# pico_stdlib — основная библиотека SDK, 
# из неё берутся gpio_init, gpio_put, sleep_ms.
target_link_libraries(${PROJECT_NAME}
   pico_stdlib
)

# По каким правилам размещать. Специфика микроконтроллера: 
# указать скрипт компоновщика, скопированный на втором шаге.
pico_set_linker_script(${PROJECT_NAME}
   ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld
)

# В каких форматах выдать результат. 
# Специфика Pico: кроме основного образа получить файлы .bin и .uf2 — 
# последний понадобится для записи прошивки в плату.
pico_add_extra_outputs(${PROJECT_NAME})

# направить стандартный ввод-вывод в USB, создаем виртуальный COM-port
pico_enable_stdio_usb(${PROJECT_NAME} 1)
# UART как канал не используем, он ведёт на два вывода корпуса, и без переходника читать с них нечего
pico_enable_stdio_uart(${PROJECT_NAME} 0)

