# STM32H743IIT6 迁移指南

本文记录如何使用 STM32CubeMX 生成 STM32H743IIT6 最小 HAL 工程，并将底层代码迁移到当前分层工程。

## 1. 总体策略

使用 CubeMX 生成并验证一个最小的 STM32H743IIT6 HAL 工程，然后只替换当前工程的芯片底层部分：

- 保留 `app`、`bsp`、`os` 和 `middleware` 的分层接口。
- 替换 CMSIS Device、STM32H7 HAL、启动文件、系统文件和链接脚本。
- 将 CubeMX 生成的时钟配置合并到当前初始化流程。
- 不要直接在 F1 文件上只修改芯片宏。

## 2. 使用 CubeMX 生成基础工程

在 CubeMX 中：

1. 选择芯片 `STM32H743IIT6`。
2. 在 `System Core -> SYS` 中将 Debug 设置为 `Serial Wire`。
3. 根据开发板原理图配置 LED 对应 GPIO 为输出。
4. 在 `Clock Configuration` 中根据实际晶振配置时钟，并确保 CubeMX 校验通过。
5. 在 `Project Manager` 中生成工程，工具链可以选择 CMake；如果当前 CubeMX 没有 CMake 选项，也可以先选择 STM32CubeIDE。
6. 先独立编译、下载并验证这个 CubeMX 工程能够启动和点灯。

先验证基础工程，可以避免把芯片、时钟、链接地址和启动文件问题带入分层工程。

## 3. CubeMX 文件与当前工程的映射

| CubeMX 文件                          | 当前工程位置                           |
| ------------------------------------ | -------------------------------------- |
| `Core/Inc/stm32h7xx_hal_conf.h`      | `src/config/stm32h7xx_hal_conf.h`      |
| `Core/Src/system_stm32h7xx.c`        | `src/drivers/system_stm32h7xx.c`       |
| `Core/Src/stm32h7xx_it.c`            | `src/drivers/stm32_interrupts.c`       |
| `Core/Startup/startup_stm32h743xx.s` | `src/drivers/startup_stm32h743xx.s`    |
| CMSIS Device Include 目录            | `src/drivers/include/`                 |
| `Drivers/STM32H7xx_HAL_Driver/`      | `third_party/st/STM32H7xx_HAL_Driver/` |
| `STM32H743IITX_FLASH.ld`             | `stm32h743iit6.ld`                     |

需要移除或替换的 F1 底层文件包括：

```text
src/drivers/startup_stm32f103c8t6.c
src/drivers/system_stm32f1xx.c
src/drivers/include/stm32f1xx.h
src/drivers/include/stm32f103xb.h
third_party/st/STM32F1xx_HAL_Driver/
stm32f103c8t6.ld
```

## 4. 修改 CMake

### 4.1 顶层 `CMakeLists.txt`

将工具链、HAL 目录和链接脚本切换为 H7：

```cmake
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/cortex_m7.cmake")

add_subdirectory(third_party/st/STM32H7xx_HAL_Driver)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/stm32h743iit6.ld
    -Wl,-Map=${PROJECT_NAME}.map
)
```

### 4.2 Cortex-M7 工具链

复制：

```text
cmake/cortex_m3.cmake -> cmake/cortex_m7.cmake
```

至少将 CPU 参数改为：

```cmake
set(CPU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")
```

C、C++、汇编和链接参数中的浮点配置必须保持一致。

### 4.3 `src/drivers/CMakeLists.txt`

将芯片宏和底层源文件改为 H743：

```cmake
target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32H743xx
    USE_HAL_DRIVER
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/sdk_init.c
    ${CMAKE_CURRENT_LIST_DIR}/startup_stm32h743xx.s
    ${CMAKE_CURRENT_LIST_DIR}/system_stm32h7xx.c
    ${CMAKE_CURRENT_LIST_DIR}/stm32_interrupts.c
)
```

启动文件扩展名以 CubeMX 实际生成的文件为准。

## 5. HAL 和芯片头文件

将 F1 HAL 配置文件改名并替换为 H7 版本：

```text
src/config/stm32f1xx_hal_conf.h
    -> src/config/stm32h7xx_hal_conf.h
```

修改头文件保护宏，并根据实际使用的外设启用模块。最小 GPIO 工程通常至少需要：

```c
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
```

使用 UART、SPI、I2C、DMA、ETH 等外设时，再启用对应模块。

`board_config.h` 中：

```c
#include "stm32h7xx.h"
```

当前 F1 的 `stm32f1xx.h`、`stm32f103xb.h` 等 Device 头文件不能继续使用，应替换为 CubeMX 对应的：

```text
stm32h7xx.h
stm32h743xx.h
system_stm32h7xx.h
```

## 6. 初始化流程

当前 `sdk_init.c` 只有 `HAL_Init()`，H743 还必须加入 CubeMX 生成的时钟配置：

```c
#include "stm32h7xx_hal.h"

static void SystemClock_Config(void);

void sdk_init(void)
{
    HAL_Init();
    SystemClock_Config();
}
```

将 CubeMX `main.c` 中的 `SystemClock_Config()` 完整复制到 `sdk_init.c`，或单独放到 `system_clock.c`。

如果 CubeMX 工程生成了以下函数，也要按生成代码的顺序合并：

```c
MPU_Config();
CPU_CACHE_Enable();
```

H743 的时钟树包含多个时钟域和 PLL，不能沿用 F1 的 `RCC->CFGR`、`RCC_CFGR_PLLMULL` 等寄存器配置代码。

## 7. 启动文件和中断

H743 是 Cortex-M7，启动文件和 F103 完全不同，必须使用 CubeMX 生成的 H743 启动文件。

`stm32_interrupts.c` 的 HAL 头文件改为：

```c
#include "stm32h7xx_hal.h"
```

SysTick 处理函数通常可以保留：

```c
void SysTick_Handler(void)
{
    HAL_IncTick();
}
```

其他中断函数名称必须以 H743 启动文件中的向量表为准，不要直接复制 F103 的中断列表。

## 8. BSP LED

`bsp_led.c` 的 HAL 头文件改为：

```c
#include "stm32h7xx_hal.h"
```

HAL 调用通常仍然可以使用：

```c
__HAL_RCC_GPIOx_CLK_ENABLE();
HAL_GPIO_Init();
HAL_GPIO_WritePin();
```

但 `GPIOx`、引脚号、默认电平和输出速度必须以 H743 开发板原理图及 CubeMX 生成结果为准。不能默认继续使用 F103 常见的 `GPIOC` 和 `GPIO_PIN_13`。

## 9. 链接脚本

将 CubeMX 生成的：

```text
STM32H743IITX_FLASH.ld
```

复制并重命名为：

```text
stm32h743iit6.ld
```

不要只修改当前 F103 链接脚本中的 Flash 和 RAM 容量。H743 包含多个内存区域，例如：

- Flash
- DTCM RAM
- AXI SRAM
- SRAM1/SRAM2/SRAM3/SRAM4
- Backup SRAM

应以 CubeMX 生成的 H743 链接脚本为基础，再根据应用需求调整段布局。

## 10. 烧录配置

`.vscode/tasks.json` 中当前使用 F1 目标配置：

```text
 target/stm32f1x.cfg
```

应替换为 OpenOCD 支持的 H7 目标配置，例如：

```text
target/stm32h7x.cfg
```

同时确认 OpenOCD 版本支持 STM32H743IIT6。

## 11. clangd 配置

`.clangd` 和 `.vscode/settings.json` 通常可以继续使用。重新配置 CMake 后，重新生成编译数据库：

```powershell
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -GNinja -Bbuild
```

使用硬件浮点时，`compile_commands.json` 中应能看到：

```text
-mfpu=fpv5-d16
-mfloat-abi=hard
```

## 12. 重新配置和验证

迁移完成后删除旧构建目录，避免残留 F1 的 CMake 缓存和编译参数：

```powershell
Remove-Item -Recurse -Force build
```

重新配置并编译：

```powershell
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -GNinja -Bbuild
cmake --build build --target all
```

确认生成：

```text
build/DEMO.elf
build/DEMO.hex
build/DEMO.bin
build/DEMO.map
```

然后下载到开发板，依次验证：

1. 能否复位并进入 `main()`。
2. `HAL_Init()` 和系统时钟配置是否成功。
3. SysTick 和 `HAL_Delay()` 是否正常。
4. LED GPIO 是否能够正常翻转。
5. 其他外设是否工作正常。

## 13. 推荐迁移顺序

```text
1. CubeMX 生成 H743 最小 HAL 工程
2. 独立编译、下载并验证 CubeMX 工程
3. 替换 CMSIS、H7 HAL、启动文件、系统文件和链接脚本
4. 修改 CMake 和 Cortex-M7 工具链
5. 合并 SystemClock_Config、MPU 和 Cache 初始化
6. 修改 board_config.h 和 bsp_led.c
7. 删除 build 目录并重新配置
8. 编译、下载并验证
```

应用层的 `app_main()`、`bsp_init()`、`os_delay_ms()` 等接口可以继续保留；需要重做的是芯片底层部分。
