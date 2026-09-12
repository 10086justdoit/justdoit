cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/cortex_m7.cmake" CACHE FILEPATH "ARM GCC toolchain")
message("[ROOT] Build Type: ${CMAKE_BUILD_TYPE}")
message("[ROOT] Toolchain File: ${CMAKE_TOOLCHAIN_FILE}")

project(DEMO LANGUAGES C ASM)

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/third_party/arm/cmsis-core/Include"
    "${CMAKE_SOURCE_DIR}/third_party/arm/cmsis-core/Device/ST/STM32H7xx/Include"
    "${CMAKE_SOURCE_DIR}/third_party/st/STM32H7xx_HAL_Driver/Inc"
    "${CMAKE_SOURCE_DIR}/third_party/st/STM32H7xx_HAL_Driver/Inc/Legacy"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32H743xx
    USE_HAL_DRIVER
    USE_PWR_LDO_SUPPLY
)

add_subdirectory(src/config)
add_subdirectory(src/drivers)
add_subdirectory(src/os)
add_subdirectory(src/bsp)
add_subdirectory(src/middleware)
add_subdirectory(src/app)

add_subdirectory(third_party/st/STM32H7xx_HAL_Driver)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/stm32h743iit6.ld
    -Wl,-Map=${PROJECT_NAME}.map
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
)
