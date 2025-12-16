cmake_minimum_required(VERSION 3.10)
project(ReportBuilder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Основная программа
add_executable(report_builder main.cpp)

# Тесты
add_executable(test_simple test_simple.cpp)
add_executable(final_test final_test.cpp)

# Если нужно подключить библиотеку для std::filesystem
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_libraries(report_builder PRIVATE stdc++fs)
    target_link_libraries(test_simple PRIVATE stdc++fs)
    target_link_libraries(final_test PRIVATE stdc++fs)
endif()

# Установить выходные файлы в папку bin
set_target_properties(report_builder test_simple final_test
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Если нужно, чтобы CMake создавал папку build автоматически
message(STATUS "Проект ReportBuilder настроен. Для сборки выполните:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake ..")
message(STATUS "  cmake --build .")
