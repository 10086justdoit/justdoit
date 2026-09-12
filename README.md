# STM32 工程集合

本仓库包含多个独立的 STM32 裸机工程。每个工程位于自己的目录中，拥有独立的 CMake 配置、源码、HAL/CMSIS 依赖和构建目录。

## 工程列表

### STM32F103C8T6 分层工程

目录：`STM32F103C8T6-layered/`

- 芯片：STM32F103C8T6
- 内核：Cortex-M3
- 框架：CMake + Ninja + ARM GCC + STM32 HAL + CMSIS
- 构建脚本：`STM32F103C8T6-layered/CMakeLists.txt`

构建：

```powershell
cmake -S STM32F103C8T6-layered -B STM32F103C8T6-layered/build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build STM32F103C8T6-layered/build --target all
```

### STM32H743IIT6 分层工程

目录：`STM32H743IIT6-layered1/`

- 芯片：STM32H743IIT6
- 内核：Cortex-M7
- 框架：与 F1 工程一致的分层结构
- CubeMX 文件名：保持 CubeMX 生成的原始名称
- 构建脚本：`STM32H743IIT6-layered1/CMakeLists.txt`

构建：

```powershell
cmake -S STM32H743IIT6-layered1 -B STM32H743IIT6-layered1/build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build STM32H743IIT6-layered1/build --target all
```

## 目录约定

每个工程使用独立的分层目录：

```text
工程目录/
├── cmake/              # 工具链和构建配置
├── src/
│   ├── app/            # 应用层
│   ├── bsp/            # 板级支持层
│   ├── config/         # 芯片和板级配置
│   ├── drivers/        # 启动、系统、中断和项目底层适配
│   ├── middleware/     # 中间件
│   └── os/             # OS 抽象层
└── third_party/        # CMSIS 和 ST HAL/LL 库
```

构建产物统一位于各工程自己的 `build/` 目录，不提交到 Git。
