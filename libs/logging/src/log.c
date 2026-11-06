// заголовочный файл для лога ошибок
#include "logging/log.h"

// Печатает дату и время сборбки, уровень лога
void log_version(void) {
    printf("built %s %s, log level %d\n", __DATE__, __TIME__, LOG_LEVEL);
}

// Печатает подпись перед текстом сообщения (в LOG_ERR, LOG_INF, LOG_DBG)
void log_prefix(const char *level, const char *function, int line)
{
    //<уровень> SP <функция>:<строка> SP <текст> CR LF
    printf("%s %s:%d ", level, function, line);
}