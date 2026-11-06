// заголовочный файл модуля профилирования (время суперцикла)
#include "profiling.h"
// ради типов SDK
#include "pico/stdlib.h"
// заголовочный файл работы со временем
#include "systime/systime.h"



// ТЕОРИЯ // TASK 2.2.4 //
// Часы считаются один раз за итерацию и в одном и том же месте цикла. 
// Тогда разность с прошлым показанием равна длительности прошлой итерации целиком
// Переменные объявлены со словом static. Их видит только profiling.c. 
// Снаружи максимум доступен через функции модуля.

// // Предыдущее время суперцикла
// static uint32_t previous_us = 0;
// // Максимальное время суперцикла
// static uint32_t max_us = 0;
// // Плавающее среднее время суперцикла
// static uint64_t avg_sum = 0;

// Структура Глобального секундомера (profiling_global_t)
typedef struct {
    uint32_t id         ; // номер секундомера
    char    *name       ; // имя секундомера
    uint64_t avg_sum    ; // Плавающее среднее время суперцикла
    uint32_t max_us     ; // Максимальное время суперцикла (mks, микросекунды, 10-6)
    uint32_t previous_us; // Предыдущее время суперцикла (mks, микросекунды, 10-6)
} profiling_global_t;

// Глобальный секундомер (суперцикл)
static profiling_global_t global_stopwatch;

// итерация около 2 мкс, за секунду их около 500 000;
// ближайшая степень двойки 2^19 = 524 288, среднее помнит около секунды
#define AVG_SHIFT 19
// замер в 1/256 долях микросекунды, чтобы среднее не теряло дробную часть
#define FRACTION_SHIFT 8

// Инициализация глобального секундомера, запоминает начальное показание часов
static void profiling_global_init(uint32_t id, const char *name) {
    // Заполняем начальными значениями
    global_stopwatch.id      = id;
    global_stopwatch.name    = (char *)name;
    global_stopwatch.avg_sum = 0;
    global_stopwatch.max_us  = 0;   
    // возвращает младшие 32 разряда часов
    global_stopwatch.previous_us = systime_us_32();
    // ТЕОРИЯ // TASK 2.2.4 //
    //  time_us_32() возвращает младшие 32 разряда тех же часов, что _time_us_64(). 
    //  Читать их дешевле, а переполняются они раз в 71,6 минуты. 
    //  Разность двух показаний переполнение переживает, и замер остаётся верным. 
}

// вызывается в начале каждой итерации и учитывает прошлую
static void profiling_global_iteration(void) {
    // Текущее время (количество тактов)
    uint32_t now_us = systime_us_32();
    // Сколько прошло от предыдущего замера
    uint32_t iteration_us = now_us - global_stopwatch.previous_us;
    // Новое значение предыдущего замера
    global_stopwatch.previous_us = now_us;

    // Вычисление максимального значения суперцикла (max_us)
    if (iteration_us > global_stopwatch.max_us) {
        global_stopwatch.max_us = iteration_us;
    }

    // Вычисление экспоненциального или плавающего среднего (avg_sum)
    {   
        // // N = 2^19 средних длительностей итерации в 1/256 долях микросекунды
        // uint64_t avg_sum = 0;

        // На каждой итерации: только сдвиги, сложение и вычитание (числа целые)
        global_stopwatch.avg_sum = global_stopwatch.avg_sum - (global_stopwatch.avg_sum >> AVG_SHIFT) + ((uint64_t)iteration_us << FRACTION_SHIFT);

        // // По команде: среднее в микросекундах
        // printf("avg %.2f us\n", (avg_sum >> 19) / 256.0f);

        // ТЕОРИЯ // TASK 2.2.4 //
        //  Среднее обновляется на каждой итерации. Поэтому оно должно стоить единицы тактов. 
        //  Формула экспоненциального среднего и её целочисленная запись выведены в т2.2.5, 
        //  раздел «Экспоненциальное среднее» и в разделе «Эффект наблюдателя» следом за ним. 
        //  Модуль хранит не само среднее, а сумму avg_sum. Она в N раз больше среднего. 
        //  На каждой итерации из неё вычитается N-я часть и прибавляется новый замер.

        //  N выбрано степенью двойки, N = 2^AVG_SHIFT. 
        //  Тогда деление на N превращается в сдвиг вправо на AVG_SHIFT разрядов. 
        //  Замер перед сложением сдвигается влево на FRACTION_SHIFT разрядов. 
        //  Так он переводится в 1/256 доли микросекунды, и среднее не теряет дробную часть.
    }//avg_sum
}

// выдача плавающего среднего
static float profiling_global_avg_us(void) {
    // Среднее в микросекундах модуль отдаёт по запросу. 
    // Здесь деление и float уже допустимы, потому что вызов бывает только по команде
    return (float)(global_stopwatch.avg_sum >> AVG_SHIFT) / (1 << FRACTION_SHIFT);
}

// выдача максимального
static uint32_t profiling_global_max_us(void) {
    // просто возврат
    return global_stopwatch.max_us;
}

// сброс максимального
static void profiling_global_reset_max(void) {
    // просто обнуление
    global_stopwatch.max_us = 0;
}







// ТЕОРИЯ // TASK 2.3.5 //
// STOPWATCHES //

// для всех секундомеров, id должен быть найден в списке
// false — секундомер с таким номером не заведён

// число секундомеров (заполняются в main.c), номера с [1-5]
// Секундомер под номером 0 забронирован под глобальный секундомер (суперцикл)
static profiling_result_t stopwatches[STOPWATCH_QTY+1];

// Первая инициализация, необходима, если мы забудем в main.c какой-нибудь секундомер
static bool first_init_profiling = true;
static void profiling_stopwatch_init_first(void) {
    if (first_init_profiling) {
        for (uint32_t id = 0; id <= STOPWATCH_QTY; id++) stopwatches[id].name = NULL;
        first_init_profiling = false;
    }
}

// Создать секундомер
void profiling_stopwatch_init(uint32_t id, const char *name)
{
    // Инициализация все х секундомеров, чтобы не было ошибки в main.c
    profiling_stopwatch_init_first();

    // Инициализация глобального секундомера, запоминает начальное показание часов
    if (id == 0) profiling_global_init(id, name);
    // Заполняем структуру обычного секундомера
    stopwatches[id].name    = (char *)name;
    stopwatches[id].count   = 0;
    stopwatches[id].avg_sum = 0;
    stopwatches[id].max_us  = 0;
    stopwatches[id].start_us= 0;
    stopwatches[id].id      = id;
}

// Запустить замер секундомера
bool profiling_start(uint32_t id)
{
    // вызывается в начале каждой итерации и учитывает прошлую (суперцикл)
    if (id == 0) profiling_global_iteration();
    // Остальные секундомеры
    else {
        // id секундомера не должен превышать STOPWATCH_QTY
        if (id > STOPWATCH_QTY)           return false; // Нет такого секундомера
        if (stopwatches[id].name == NULL) return false; // Секундомер не заполнен в main.c
        
        // Замеры для текущего секундомера
        stopwatches[id].count++;                    // Количество кругов (переполнение через 71 минуту)
        stopwatches[id].start_us = systime_us_32();    // возвращает младшие 32 разряда часов
    }
    // Секундомеры с номерами от 0 до STOPWATCH_QTY
    return true; 
}

// Отчет секундомера
bool profiling_stop(uint32_t id)
{
    // суперцикл не обрабатываем
    if (id != 0) {
        // id секундомера не должен превышать STOPWATCH_QTY
        if (id > STOPWATCH_QTY)           return false; // Нет такого секундомера
        if (stopwatches[id].name == NULL) return false; // Секундомер не заполнен в main.c

        // Вычисление времени (от старта до стопа)
        uint32_t now_us = systime_us_32(); // возвращает младшие 32 разряда часов
        uint32_t iteration_us = now_us - stopwatches[id].start_us;

        // Запоминаем максимальное значение времени
        if (iteration_us > stopwatches[id].max_us)
            stopwatches[id].max_us = iteration_us;

        // Суммируем все интервалы
        stopwatches[id].avg_sum += iteration_us;
    }// >0

    // Секундомеры с номерами от 0 до STOPWATCH_QTY
    return true; 
}

// Вернуть значения секундомера
bool profiling_get(uint32_t id, profiling_result_t **result)
{
    // Заполняем глобальный суперцикл
    if (id == 0) {
        // Максимальное время круга
        stopwatches[id].max_us  =  profiling_global_max_us();
        // Точность знаков после запятой
        #define PRECISION_AVG_US 100
        // Среднее значение круга (переделываем float в целое без потери точности)
        stopwatches[id].avg_sum =  (uint32_t)(profiling_global_avg_us() * PRECISION_AVG_US);
        stopwatches[id].count   =  PRECISION_AVG_US;
    }

    // id секундомера не должен превышать STOPWATCH_QTY
    if (id > STOPWATCH_QTY)           return false; // Нет такого секундомера
    if (stopwatches[id].name == NULL) return false; // Секундомер не заполнен в main.c

    // Возврат секундомера по номеру
    *result = &stopwatches[id];
    return true;
}

// Сбросить все секундомеры
void profiling_reset(void)
{
    // Обнулить все секундомеры
    for (uint32_t id = 0; id <= STOPWATCH_QTY; id++) {
        stopwatches[id].count    = 0;
        stopwatches[id].avg_sum  = 0;
        stopwatches[id].max_us   = 0;
        stopwatches[id].start_us = systime_us_32();
    }
    // Сброс максимального замера для суперцикла
    profiling_global_reset_max();
}

// Сбросить выбранный секундомер
bool profiling_reset_id(uint32_t id) {
    // Сброс максимального замера для суперцикла
    if (id == 0) profiling_global_reset_max();

    // id секундомера не должен превышать STOPWATCH_QTY
    if (id > STOPWATCH_QTY) return false; // Нет такого секундомера

    // Обнулить все секундомеры
    if (id <= STOPWATCH_QTY) {
        stopwatches[id].count    = 0;
        stopwatches[id].avg_sum  = 0;
        stopwatches[id].max_us   = 0;
        stopwatches[id].start_us = systime_us_32();
    }
    return true;
}













