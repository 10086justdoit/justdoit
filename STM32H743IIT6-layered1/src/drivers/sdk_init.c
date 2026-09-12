#include "sdk_init.h"
#include "main.h"
#include "stm32h7xx_hal.h"

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc_config = {0};
    RCC_ClkInitTypeDef clock_config = {0};

    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    osc_config.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_config.HSEState = RCC_HSE_ON;
    osc_config.PLL.PLLState = RCC_PLL_ON;
    osc_config.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc_config.PLL.PLLM = 5;
    osc_config.PLL.PLLN = 192;
    osc_config.PLL.PLLP = 2;
    osc_config.PLL.PLLQ = 2;
    osc_config.PLL.PLLR = 2;
    osc_config.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    osc_config.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    osc_config.PLL.PLLFRACN = 0;

    if (HAL_RCC_OscConfig(&osc_config) != HAL_OK)
    {
        Error_Handler();
    }

    clock_config.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
        | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    clock_config.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clock_config.SYSCLKDivider = RCC_SYSCLK_DIV1;
    clock_config.AHBCLKDivider = RCC_HCLK_DIV2;
    clock_config.APB3CLKDivider = RCC_APB3_DIV2;
    clock_config.APB1CLKDivider = RCC_APB1_DIV2;
    clock_config.APB2CLKDivider = RCC_APB2_DIV2;
    clock_config.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&clock_config, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MPU_Config(void)
{
    MPU_Region_InitTypeDef mpu_config = {0};

    HAL_MPU_Disable();
    mpu_config.Enable = MPU_REGION_ENABLE;
    mpu_config.Number = MPU_REGION_NUMBER0;
    mpu_config.BaseAddress = 0x0;
    mpu_config.Size = MPU_REGION_SIZE_4GB;
    mpu_config.SubRegionDisable = 0x87;
    mpu_config.TypeExtField = MPU_TEX_LEVEL0;
    mpu_config.AccessPermission = MPU_REGION_NO_ACCESS;
    mpu_config.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    mpu_config.IsShareable = MPU_ACCESS_SHAREABLE;
    mpu_config.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    mpu_config.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    HAL_MPU_ConfigRegion(&mpu_config);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void sdk_init(void)
{
    MPU_Config();
    HAL_Init();
    SystemClock_Config();
}

void sdk_deinit(void)
{
}

void Error_Handler(void)
{
    __disable_irq();
    for (;;)
    {
    }
}
