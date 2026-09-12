#include "os_port.h"
#include "stm32h7xx_hal.h"

void os_delay_ms(uint32_t milliseconds)
{
    HAL_Delay(milliseconds);
}
