cmake_minimum_required(VERSION 3.14)
project(report_builder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Строгость компиляции
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 2. Опции проекта
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_FORMAT_CHECK "Enable style checking with clang-format" ON)

# Пути
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Основная библиотека
add_subdirectory(src)

# 3. Тесты
if(BUILD_TESTS)
    enable_testing()
    
    find_package(GTest REQUIRED)
    # Если GTest не найден, CMake остановится с ошибкой, поэтому else не нужен.
    add_subdirectory(test)
endif()

# 4. Проверка стиля
if(ENABLE_FORMAT_CHECK) 
    find_program(CLANG_FORMAT "clang-format")
    if(CLANG_FORMAT)
        # Цель для проверки стиля (без изменений файлов)
        add_custom_target(style-check
            COMMAND ${CLANG_FORMAT} --dry-run -Werror --style=file
                    ${CMAKE_SOURCE_DIR}/include/report_builder/*.h
                    ${CMAKE_SOURCE_DIR}/src/*.cpp
                    ${CMAKE_SOURCE_DIR}/test/*.cpp
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Checking code style with clang-format"
        )
        # Цель для автоисправления стиля
        add_custom_target(style-fix
            COMMAND ${CLANG_FORMAT} -i --style=file
                    ${CMAKE_SOURCE_DIR}/include/report_builder/*.h
                    ${CMAKE_SOURCE_DIR}/src/*.cpp
                    ${CMAKE_SOURCE_DIR}/test/*.cpp
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Fixing code style with clang-format"
        )
    else()
        message(WARNING "clang-format not found. Style targets 'style-check' and 'style-fix' will not be available.")
    endif()
endif()
