target_sources(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/src/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/app_main.c"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}"
)
