target_compile_definitions(${PROJECT_NAME} PRIVATE
    STM32H743xx
    USE_HAL_DRIVER
    USE_PWR_LDO_SUPPLY
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/sdk_init.c
    ${CMAKE_CURRENT_LIST_DIR}/startup_stm32h743xx.s
    ${CMAKE_CURRENT_LIST_DIR}/system_stm32h7xx.c
    ${CMAKE_CURRENT_LIST_DIR}/stm32h7xx_it.c
    ${CMAKE_CURRENT_LIST_DIR}/stm32h7xx_hal_msp.c
    ${CMAKE_CURRENT_LIST_DIR}/syscalls.c
    ${CMAKE_CURRENT_LIST_DIR}/sysmem.c
)
