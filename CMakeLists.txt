cmake_minimum_required(VERSION 3.14)
project(report_builder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Настройки компилятора
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Опции проекта
option(BUILD_TESTS "Build tests" ON)
option(ENABLE_STYLE_TEST "Enable style checking" ON)

# Пути
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Основная библиотека
add_subdirectory(src)

# Тесты
if(BUILD_TESTS)
    enable_testing()
    
    # Ищем Google Test
    find_package(GTest REQUIRED)
    
    if(GTest_FOUND)
        add_subdirectory(test)
    else()
        message(WARNING "GTest not found, tests will not be built")
    endif()
endif()

# Проверка стиля (опционально)
if(ENABLE_STYLE_TEST AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    find_program(CLANG_FORMAT "clang-format")
    if(CLANG_FORMAT)
        add_custom_target(style-check
            COMMAND ${CLANG_FORMAT} --dry-run -Werror --style=file ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/test/*.cpp
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Checking code style with clang-format"
        )
        add_custom_target(style-fix
            COMMAND ${CLANG_FORMAT} -i --style=file ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/test/*.cpp
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Fixing code style with clang-format"
        )
    endif()
endif()
