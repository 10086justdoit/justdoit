#include "bsp_init.h"
#include "bsp_led.h"
#include "sdk_init.h"

void bsp_init(void)
{
    sdk_init();
    bsp_led_init();
}
