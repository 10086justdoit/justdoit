set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(WIN32)
    set(COMPILER_SUFFIX ".exe")
else()
    set(COMPILER_SUFFIX "")
endif()

find_program(ARM_GCC "arm-none-eabi-gcc${COMPILER_SUFFIX}")

if(NOT ARM_GCC)
    message(FATAL_ERROR "Unable to find arm-none-eabi-gcc")
endif()

get_filename_component(ARM_TOOLCHAIN_PATH "${ARM_GCC}" DIRECTORY)

set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc${COMPILER_SUFFIX}")
set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
set(CMAKE_LINKER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc${COMPILER_SUFFIX}")
set(CMAKE_OBJCOPY "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-objcopy${COMPILER_SUFFIX}")
set(CMAKE_SIZE "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-size${COMPILER_SUFFIX}")

set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")
set(COMMON_FLAGS "-ffunction-sections -fdata-sections -Wall -fstack-usage")
set(CMAKE_C_FLAGS "${MCU_FLAGS} ${COMMON_FLAGS} --specs=nano.specs")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -Wl,--gc-sections")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")