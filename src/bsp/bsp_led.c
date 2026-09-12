#include "bsp_led.h"
#include "board_config.h"
#include "stm32f1xx_hal.h"

void bsp_led_init(void)
{
    GPIO_InitTypeDef config = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    config.Pin = BOARD_LED_GPIO_PIN;
    config.Mode = GPIO_MODE_OUTPUT_PP;
    config.Pull = GPIO_NOPULL;
    config.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BOARD_LED_GPIO_PORT, &config);

    bsp_led_off();
}

void bsp_led_on(void)
{
    HAL_GPIO_WritePin(
        BOARD_LED_GPIO_PORT,
        BOARD_LED_GPIO_PIN,
        BOARD_LED_ACTIVE_LEVEL
    );
}

void bsp_led_off(void)
{
    HAL_GPIO_WritePin(
        BOARD_LED_GPIO_PORT,
        BOARD_LED_GPIO_PIN,
        BOARD_LED_INACTIVE_LEVEL
    );
}
