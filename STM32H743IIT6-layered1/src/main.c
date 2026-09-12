#include "app_main.h"
#include "bsp_init.h"

int main(void)
{
    bsp_init();
    app_main();

    for (;;)
    {
    }
}
