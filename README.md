# STM32F103C8T6 分层工程

这是一个基于 **CMake + Ninja + ARM GCC + STM32 HAL + CMSIS** 的 STM32F103C8T6 裸机工程模板。

## 目录结构

```text
TEMPLATE_STM32F103C8T6-layered/
├── CMakeLists.txt                 # 顶层构建配置
├── cmake/
│   └── cortex_m3.cmake            # ARM GCC 工具链和 Cortex-M3 编译参数
├── stm32f103c8t6.ld               # Flash、RAM、栈和代码段链接布局
├── src/
│   ├── main.c                     # 系统入口
│   ├── app/                       # 业务层
│   ├── config/                    # 芯片、板级和应用配置
│   ├── middleware/                # 协议、日志、环形缓冲区等中间件
│   ├── os/                        # 操作系统抽象层
│   ├── bsp/                       # 当前开发板的硬件封装
│   └── drivers/                   # 启动文件、系统文件和底层适配
└── third_party/
    ├── arm/                       # ARM CMSIS 和编译器适配代码
    └── st/                        # ST STM32F1xx HAL/LL 驱动库
```

## 各层功能

### `app`：业务层

实现产品功能，不直接操作 STM32 HAL。例如 LED 闪烁、通信任务和设备状态处理。业务代码通过 BSP、OS 或中间件接口使用底层能力。

### `config`：配置层

集中管理芯片宏、板级引脚、应用参数和 HAL 配置。例如 STM32F103xB、LED 所在 GPIO 端口和闪烁周期。

### `middleware`：中间件层

存放与具体芯片业务无关的通用组件，例如通信协议、日志、环形缓冲区、文件系统或 GUI。目前目录仅保留 CMake 接口，后续可按模块添加。

### `os`：操作系统抽象层

为业务提供统一的延时、任务、锁和消息接口。当前使用 `HAL_Delay()` 实现 `os_delay_ms()`，以后可以替换为 FreeRTOS 或 RT-Thread，而不修改业务代码。

### `bsp`：板级支持层

封装当前开发板的硬件资源，例如 LED、按键、串口和传感器。BSP 内部可以调用 STM32 HAL，但上层不需要了解具体 GPIO 和引脚配置。

### `drivers`：项目底层适配层

包含启动文件、系统初始化、中断处理和项目对 CMSIS/HAL 的整合代码。它连接项目代码与芯片底层库。

### `third_party`：第三方库

保存 ARM 和 ST 提供的原始代码，通常不直接修改：

- `arm/cmsis-core`：Cortex-M 内核和 CMSIS 定义。
- `arm/cmsis-compiler`：不同编译器的适配代码。
- `st/STM32F1xx_HAL_Driver`：GPIO、UART、SPI、I2C、定时器等 STM32 外设驱动。

## 依赖方向

```text
app
 ↓
middleware / os / bsp
 ↓
src/drivers
 ↓
third_party/arm + third_party/st
```

上层可以调用下层，下层不要反向依赖业务层。`CMakeLists.txt`、`cmake/cortex_m3.cmake` 和链接脚本属于工程构建配置，不属于运行时业务层。

## CMakeLists 调用关系

本工程使用一个可执行目标 `DEMO`。顶层 `CMakeLists.txt` 先创建这个目标，然后通过 `add_subdirectory()` 依次执行各层目录中的 `CMakeLists.txt`：

```text
CMakeLists.txt
├── add_subdirectory(src/config)
├── add_subdirectory(src/drivers)
├── add_subdirectory(src/os)
├── add_subdirectory(src/bsp)
├── add_subdirectory(src/middleware)
├── add_subdirectory(src/app)
└── add_subdirectory(third_party/st/STM32F1xx_HAL_Driver)
```

各层并不会单独生成可执行文件，而是通过 `target_sources()` 和 `target_include_directories()` 把文件追加到同一个 `${PROJECT_NAME}` 目标，也就是 `DEMO`。

### 顶层 `CMakeLists.txt`

顶层负责工程总装：

1. 设置 C/C++ 标准和工具链文件。
2. 执行 `project(DEMO ...)` 并创建 `add_executable(DEMO)`。
3. 加入 CMSIS、CMSIS 编译器适配和系统调用源文件。
4. 使用 `add_subdirectory()` 调用各层 CMakeLists。
5. 使用 `target_link_options()` 加入 `stm32f103c8t6.ld`。
6. 链接完成后使用 `objcopy` 生成 HEX 和 BIN 文件。

### `cmake/cortex_m3.cmake`

它不是通过 `add_subdirectory()` 调用的，而是由顶层的 `CMAKE_TOOLCHAIN_FILE` 指定，在 CMake 配置早期自动加载。它负责：

- 查找 `arm-none-eabi-gcc` 工具链。
- 设置 `-mcpu=cortex-m3` 和 `-mthumb`。
- 设置编译、汇编和链接参数。
- 指定 `objcopy`、`size` 等 ARM 工具。

### `src/config/CMakeLists.txt`

通过 `target_include_directories()` 将配置目录加入头文件搜索路径。它通常不添加 `.c` 文件，因为配置层主要提供 `.h` 文件和编译配置。

### `src/drivers/CMakeLists.txt`

通过 `target_compile_definitions()` 添加 `STM32F103xB` 芯片宏，通过 `target_include_directories()` 加入底层头文件目录，再通过 `target_sources()` 加入：

- 启动文件。
- 系统时钟文件。
- 项目初始化文件。
- 中断处理文件。

### `src/os/CMakeLists.txt`

加入 `os/include` 头文件目录，并通过 `target_sources()` 加入 `os_port.c`。当前 OS 抽象层内部使用 HAL 的 `HAL_Delay()`，以后可以替换为 FreeRTOS 或 RT-Thread 实现。

### `src/bsp/CMakeLists.txt`

加入 BSP 头文件目录，并通过 `target_sources()` 加入 `bsp_init.c` 和 `bsp_led.c`。BSP 源文件可以调用 HAL，但业务层不需要直接接触 HAL 的 GPIO 配置。

### `src/middleware/CMakeLists.txt`

目前只有占位说明，没有添加源文件。新增中间件时，可以在这里加入对应模块的源文件和头文件目录，例如：

```cmake
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ringbuffer/ringbuffer.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ringbuffer
)
```

### `src/app/CMakeLists.txt`

将系统入口 `src/main.c` 和业务入口 `src/app/app_main.c` 加入 `DEMO`，同时加入 app 头文件目录。`main.c` 负责调用 `bsp_init()` 和 `app_main()`，`app_main.c` 只实现业务逻辑。

### `third_party/st/STM32F1xx_HAL_Driver/CMakeLists.txt`

由顶层直接调用，负责加入 ST HAL/LL 的头文件目录、编译宏和 HAL/LL 源文件。它与 `third_party/arm` 一样属于外部代码，项目层只负责引用，不建议在其中混入业务代码。

### 一次构建时的执行顺序

```text
cmake -B build
  ↓
加载 cmake/cortex_m3.cmake
  ↓
创建 DEMO 目标
  ↓
执行各层 CMakeLists
  ↓
编译所有 target_sources 文件
  ↓
使用 stm32f103c8t6.ld 链接 DEMO.elf
  ↓
生成 DEMO.hex 和 DEMO.bin
```

## `DEMO.code-workspace` 的作用

`DEMO.code-workspace` 是 VS Code 的工程工作区配置文件。它不参与 CMake 编译，也不会生成固件，主要用于让 VS Code 以统一方式打开和管理本工程。

当前工作区使用相对路径：

```json
{
  "folders": [
    {
      "path": "."
    }
  ]
}
```

其中 `"path": "."` 表示当前 `.code-workspace` 文件所在目录，也就是工程根目录。这样工程整体移动到其他位置后，工作区仍然可以正常定位项目文件。

工作区配置还可以保存工程级 VS Code 设置，例如：

- ARM GCC 编译器路径提示。
- VS Code 任务按钮配置。
- CMake 和代码补全相关设置。
- 多目录工程的文件夹列表。

它与其他工程文件的关系如下：

```text
DEMO.code-workspace  → 告诉 VS Code 如何打开和管理工程
.vscode/tasks.json   → 定义配置、编译和烧录命令
CMakeLists.txt       → 组织源文件并生成构建系统
cortex_m3.cmake      → 配置 ARM GCC 工具链
stm32f103c8t6.ld     → 配置最终链接的 Flash/RAM 布局
```

因此，打开工程时可以直接选择 `DEMO.code-workspace`；实际编译仍由 CMake 负责，实际烧录则由 VS Code 任务调用 OpenOCD 完成。

## 构建

在工程根目录执行：

```powershell
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -GNinja -Bbuild
cmake --build build --target all
```

构建成功后，文件位于 `build/`：

```text
DEMO.elf    # ELF 调试和烧录文件
DEMO.hex    # Intel HEX 格式
DEMO.bin    # 原始二进制格式
DEMO.map    # 链接映射文件
```

## 硬件烧录

工程当前配置使用 OpenOCD 和 CMSIS-DAP：

```powershell
openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg `
  -c "program ./build/DEMO.elf verify reset exit"
```

烧录前需要确认调试器、开发板供电以及 `SWDIO`、`SWCLK`、`GND` 和可选 `NRST` 接线正确。
