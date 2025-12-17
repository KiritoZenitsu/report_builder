# Основная программа
add_executable(report_builder main.cpp)

# Установка
install(TARGETS report_builder DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/report_builder/ DESTINATION include/report_builder)
