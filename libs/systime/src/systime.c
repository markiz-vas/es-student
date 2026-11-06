// заголовочный файл работы со временем
#include "systime/systime.h"

// заголовочные файлы для stdio_init_all
// ради типов SDK, [getchar_timeout_us], [PICO_ERROR_TIMEOUT], NULL, uint, time_us_64()
#include "pico/stdlib.h"

// Возвращает монотонное время в микросекундах (mks, микросекунды, 10-6)
uint64_t systime_us(void) {
    return time_us_64();
}

// Возвращает монотонное время в миллисекундах (ms, миллисекунды, 10-3)
uint64_t systime_ms(void) {
    return time_us_64() / 1000;
}

//  time_us_32() возвращает младшие 32 разряда тех же часов, что time_us_64(). 
// Возвращает младшие 32 разряда монотонное время в микросекундах (mks, микросекунды, 10-6)
uint32_t systime_us_32(void) {
    return time_us_32();
}

// Возвращает младшие 32 разряда монотонное время в миллисекундах (ms, миллисекунды, 10-3)
uint32_t systime_ms_32(void) {
    return time_us_32() / 1000;
}


// Проверяет, прошло ли время period_us со времени last_us
// Времена в микросекундах
// Если Время прошло, то обновляем последнее время
// true - необходимое время прошло, false - время ещё не прошло
bool systime_period_elapsed(uint64_t *last_us, uint64_t period_us)
{
    // Замер монотонного времени
    uint64_t now_us = systime_us();

    // Проверяет, прошло ли время period_us со времени last_us
    if (now_us - *last_us < period_us) {
        return false; // ранний выход, если не прошло
    }

    // Следующее срабатывание отсчитывается от прошлого, а не от «сейчас»
    *last_us += period_us;

    // Отстали больше чем на период: не догонять, а начать отсчёт заново
    if (now_us - *last_us >= period_us) {
        // сохраняем текущее время
        *last_us = now_us;
    }

    // Время прошло
    return true;
}
