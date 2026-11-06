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
project(231_firmware_template)

# PROJECT_NAME (англ. project name) — переменная CMake,
# в которую попадает имя, объявленное командой project().
# Запись ${PROJECT_NAME} подставляет имя проекта в любом месте
# описания сборки, это позволяет задавать имя в файле ровно один раз.

# Порядок важен: pico_sdk_init() вызывается после объявления проекта.
pico_sdk_init()

# Команда для компилятора
# Ключ -Wall включает предупреждения о коде, который скорее всего написан с ошибкой. 
# add_compile_options() отдаёт ключ всем целям проекта. # В задании п2.3.6 появятся библиотеки, и он достанется и им.
add_compile_options(-Wall)


# Из чего собирать.
# Типовая команда: создать цель сборки с именем ${PROJECT_NAME}
# из перечисленных исходных файлов. (*.c)
add_executable(${PROJECT_NAME}
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
   ${CMAKE_CURRENT_SOURCE_DIR}/api/api.c
   ${CMAKE_CURRENT_SOURCE_DIR}/api/api-callbacks.c
   ${CMAKE_CURRENT_SOURCE_DIR}/led-task/led-task.c
   ${CMAKE_CURRENT_SOURCE_DIR}/button-task/button-task.c
   ${CMAKE_CURRENT_SOURCE_DIR}/pi-task/pi-task.c
   
   #TASK 2.3.6# # add files in libs
   # ${CMAKE_CURRENT_SOURCE_DIR}/logging/log.c
   # ${CMAKE_CURRENT_SOURCE_DIR}/device/device.c
   # ${CMAKE_CURRENT_SOURCE_DIR}/stdio-text-protocol/stdio-text-protocol.c
   # ${CMAKE_CURRENT_SOURCE_DIR}/profiling/profiling.c
   #TASK 2.3.2# # dellete files from project "231-firmware-template"
   # ${CMAKE_CURRENT_SOURCE_DIR}/led/led.c
   #TASK 2.3.1# # dellete files from project "231-firmware-template"
   # ${CMAKE_CURRENT_SOURCE_DIR}/memory/memory.c
   # ${CMAKE_CURRENT_SOURCE_DIR}/clock/clock.c
)

# Указываем где искать заголовочные файлы (*.h)
# Слово PRIVATE означает, что путь нужен только при сборке этой цели и никуда дальше не передаётся.
target_include_directories(${PROJECT_NAME} PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}            # для поиска *.h в корневой папки и подпапок
   ${CMAKE_CURRENT_SOURCE_DIR}/api
   ${CMAKE_CURRENT_SOURCE_DIR}/led-task
   ${CMAKE_CURRENT_SOURCE_DIR}/button-task
   ${CMAKE_CURRENT_SOURCE_DIR}/pi-task
   
   #TASK 2.3.6# # add files in libs
   # ${CMAKE_CURRENT_SOURCE_DIR}/logging
   # ${CMAKE_CURRENT_SOURCE_DIR}/device
   # ${CMAKE_CURRENT_SOURCE_DIR}/stdio-text-protocol
   # ${CMAKE_CURRENT_SOURCE_DIR}/profiling
   #TASK 2.3.2# # dellete files from project "231-firmware-template"
   # ${CMAKE_CURRENT_SOURCE_DIR}/led
   #TASK 2.3.1# # dellete files from project "231-firmware-template"
   # ${CMAKE_CURRENT_SOURCE_DIR}/memory
   # ${CMAKE_CURRENT_SOURCE_DIR}/clock
)

# Добавляем библиотеки из папки libs
# Добавить к сборке проекта описание из logging/CMakeLists.txt
# add_subdirectory(logging)

# С какими библиотеками. Тоже типовая: указать, на что опирается ваш код.
# pico_stdlib — основная библиотека SDK,
# из неё берутся gpio_init, gpio_put, sleep_ms.
# pico_unique_id — читает серийный номер платы pico_get_unique_board_id_string()
target_link_libraries(${PROJECT_NAME}
   pico_stdlib             # основная библиотека SDK
   pico_unique_id          # читает серийный номер платы
   device                  # подключение библиотеки паспорта устройства
   firmware                # подключение библиотеки паспорта прошивки
   logging                 # подключение библиотеки журнала
   stdio-text-protocol     # подключение библиотеки протокола
   systime                 # подключение библиотеки работы со временем
   profiling               # подключение библиотеки профилирования
   
   # pico_cyw43_arch_none  # pico_w #  # we need Wifi to access the GPIO, but we don't need anything else
)


# libs лежит вне папки проекта: второй аргумент задаёт каталог сборки
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)      # подключение папок библиотеки


# Команда создаёт макрос DEVICE_BOARD со строкой из PICO_BOARD — как если бы вы написали:
#     #define DEVICE_BOARD "pico" в коде, только пишет его сборка.
# Определение макросов (#define <name> <значение>), формат: <name>=<значение> [ВНИМАНИЕ: пробелы недопустимы]
target_compile_definitions(${PROJECT_NAME} PRIVATE
   #DEVICE_BOARD="${PICO_BOARD}" # Имя проекта
    LOG_LEVEL=LOG_LEVEL_INF      # Уровень лога
   #LOG_LEVEL=LOG_LEVEL_DBG      # Уровень лога
)

#TASK 2.3.1# # dellete file "memmap_rp2040.ld" from project "231-firmware-template"
#TASK 1.1.2# # По каким правилам размещать. Специфика микроконтроллера:
#TASK 1.1.2# # указать скрипт компоновщика, скопированный на втором шаге.
#TASK 1.1.2# pico_set_linker_script(${PROJECT_NAME}
#TASK 1.1.2#    ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld
#TASK 1.1.2# )

# В каких форматах выдать результат.
# Специфика Pico: кроме основного образа получить файлы .bin и .uf2 —
# последний понадобится для записи прошивки в плату.
pico_add_extra_outputs(${PROJECT_NAME})

# направить стандартный ввод-вывод в USB, создаем виртуальный COM-port
pico_enable_stdio_usb(${PROJECT_NAME} 1)
# UART как канал не используем, он ведёт на два вывода корпуса, и без переходника читать с них нечего
pico_enable_stdio_uart(${PROJECT_NAME} 0)

