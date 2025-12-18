# Юнит-тесты
add_executable(unit_tests unit_tests.cpp)
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(unit_tests GTest::gtest GTest::gtest_main)

add_test(NAME unit_tests COMMAND unit_tests)

# Интеграционные тесты
add_executable(integration_tests integration_tests.cpp)
target_include_directories(integration_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(integration_tests GTest::gtest GTest::gtest_main)

add_test(NAME integration_tests COMMAND integration_tests)
