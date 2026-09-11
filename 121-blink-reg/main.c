//varALL//  // заголовочные файлы для sleep_ms
#include "pico/stdlib.h"
//varALL//  // заголовочные файлы для gpio_init
//var0//    // заголовочные файлы для gpio_set_dir, gpio_put 
#include "hardware/gpio.h"

//var2// // Для прямой манипуляции с адресами регистров
//var2// // Числа выписывать не нужно: и базовый адрес, 
//var2// // и смещения уже объявлены в заголовочных файлах SDK
#include "hardware/regs/addressmap.h"   // SIO_BASE
#include "hardware/regs/sio.h"          // SIO_GPIO_OE_SET_OFFSET

//varALL// // Нормер выхода
const uint LED_PIN = 16;

//var1// // Адреса регистров платы (напрямую без addressmap.h)
// #define SIO_BASE      0xd0000000
// #define GPIO_OE_SET   (*(volatile uint32_t *)(SIO_BASE + 0x024))
// #define GPIO_OUT_SET  (*(volatile uint32_t *)(SIO_BASE + 0x014))
// #define GPIO_OUT_CLR  (*(volatile uint32_t *)(SIO_BASE + 0x018))

//var2// // Адреса регистров платы (с использованием addressmap.h и sio.h)
//var2// //Перед суперциклом заведите два указателя на регистры 
volatile uint32_t *gpio_oe_set  = (uint32_t *)(SIO_BASE + SIO_GPIO_OE_SET_OFFSET );
volatile uint32_t *gpio_out_set = (uint32_t *)(SIO_BASE + SIO_GPIO_OUT_SET_OFFSET);
volatile uint32_t *gpio_out_clr = (uint32_t *)(SIO_BASE + SIO_GPIO_OUT_CLR_OFFSET);
//var2// // и маску вывода светодиода
const uint32_t led_mask = 1u << LED_PIN;

// ТЕОРИЯ //
// Указатель здесь — это просто число-адрес, по которому программа будет писать. 
// Маска 1u << LED_PIN ставит единицу в тот разряд, который отвечает за вывод LED_PIN, 
// и нули во все остальные: именно так регистр понимает, о каком выводе идёт речь.
// Слово volatile говорит компилятору, что за этим адресом не обычная переменная, 
// а регистр устройства. Без него компилятор вправе решить, 
// что две записи подряд бессмысленны, и выбросить одну из них.


int main()
{
    //varALL// // Инициализация вывода // отдать вывод модулю GPIO
    gpio_init(LED_PIN);             // через gpio.h
    // Коментарий из практики:
    // Что творится в gpio_init(LED_PIN); понять так и не получилось, 
    // но без этой команды ничего не работает ни по какому варианту

    //var0// // Вывод работает на выход // задать направление «выход»
    // gpio_set_dir(LED_PIN, GPIO_OUT);    // через gpio.h

    //var1// // объявить вывод LED_PIN выходом
    // GPIO_OE_SET  = 1u << LED_PIN;   // напрямую в регистр

    //var2// // объявить вывод LED_PIN выходом
    *gpio_oe_set = led_mask;

    // Бесконечный цикл - это обязательное условие,
    // Так как все программы запускаются на Земле 1 раз
    while (1)
    {
        //var0// Изменение уровня напряжения: 1 - 3.3В, 0 - 1.1В
        // gpio_|_put(LED_PIN, 1);
        // sleep_ms(250);
        // gpio_|_put(LED_PIN, 0);
        // sleep_ms(1000);

        //var1// // выставить на выводе высокий уровень напряжения и очищаем его (низкий уровень)
        // GPIO_OUT_SET = 1u << LED_PIN;   // напрямую в регистр GPIO_OUT
        // sleep_ms(250);
        // GPIO_OUT_CLR = 1u << LED_PIN;   // напрямую в регистр GPIO_OUT
        // sleep_ms(250);
        // GPIO_OUT_SET = 1u << LED_PIN;   // напрямую в регистр GPIO_OUT
        // sleep_ms(250);
        // GPIO_OUT_CLR = 1u << LED_PIN;   // напрямую в регистр GPIO_OUT
        // sleep_ms(1000);

        //var2// // выставить на выводе высокий уровень напряжения и очищаем его (низкий уровень)
        *gpio_out_set = led_mask;   // напрямую в регистр GPIO_OUT через sio.h
        sleep_ms(250);
        *gpio_out_clr = led_mask;   // напрямую в регистр GPIO_OUT через sio.h
        sleep_ms(500);
    }
}