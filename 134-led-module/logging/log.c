// заголовочный файл для лога ошибок
#include "log.h"

// Печатает, что за прибор перед вами и какая в нём прошивка
void log_version(void)
{
    printf("device: %s %s, built %s %s, log level %d\n",
           DEVICE_NAME, FIRMWARE_VERSION, __DATE__, __TIME__, LOG_LEVEL);
}

// Печатает подпись перед текстом сообщения (в LOG_ERR, LOG_INF, LOG_DBG)
void log_prefix(const char *level, const char *function, int line)
{
    printf("%s %s:%d ", level, function, line);
}