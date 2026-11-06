# Минимальная версия CMake
cmake_minimum_required(VERSION 3.15)

# Строка project — типовая, она задаёт имя проекта.
project(device)
# device # подключение библиотеки паспорта устройства

# PROJECT_NAME (англ. project name) — переменная CMake,
# в которую попадает имя, объявленное командой project().
# Запись ${PROJECT_NAME} подставляет имя проекта в любом месте
# описания сборки, это позволяет задавать имя в файле ровно один раз.

# Из чего собирать.
# Типовая команда: создать цель сборки с именем ${PROJECT_NAME}
# из перечисленных исходных файлов. (*.c)
# add_library устроена так же, как add_executable, но описывает библиотеку; 
# слово STATIC означает, что её код при сборке войдёт в образ прошивки. 
# В описании прошивки от модуля остаётся только имя:
# add_library(device STATIC
add_library(${PROJECT_NAME} STATIC
    src/device.c
)

# Указываем где искать заголовочные файлы (*.h)
# Слово PRIVATE означает, что путь нужен только при сборке этой цели и никуда дальше не передаётся.
# PUBLIC — что она нужна и её пользователям, и те получают её вместе с библиотекой.
# target_include_directories(device PUBLIC
target_include_directories(${PROJECT_NAME} PUBLIC
   include
)

# С какими библиотеками. Тоже типовая: указать, на что опирается ваш код.
# pico_stdlib — основная библиотека SDK,
# из неё берутся gpio_init, gpio_put, sleep_ms.
# pico_unique_id — читает серийный номер платы pico_get_unique_board_id_string()
# target_link_libraries(device PRIVATE
target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib             # основная библиотека SDK
    pico_unique_id          # читает серийный номер платы
    logging                 # подключение библиотеки журнала
    
    # pico_cyw43_arch_none    # pico_w #    # we need Wifi to access the GPIO, but we don't need anything else
)

# Команда создаёт макрос DEVICE_BOARD со строкой из PICO_BOARD — как если бы вы написали:
#     #define DEVICE_BOARD "pico" в коде, только пишет его сборка.
# Определение макросов (#define <name> <значение>), формат: <name>=<значение> [ВНИМАНИЕ: пробелы недопустимы]
target_compile_definitions(${PROJECT_NAME} PRIVATE
    DEVICE_BOARD="${PICO_BOARD}" # Имя проекта
)