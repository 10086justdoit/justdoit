target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/bsp_init.c
    ${CMAKE_CURRENT_LIST_DIR}/bsp_led.c
)
