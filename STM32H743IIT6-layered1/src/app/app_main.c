#include "app_main.h"
#include "bsp_led.h"
#include "os_port.h"

void app_main(void)
{
    for (;;)
    {
        bsp_led_on();
        os_delay_ms(2000U);

        bsp_led_off();
        os_delay_ms(2000U);
    }
}
