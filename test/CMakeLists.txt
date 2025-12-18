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

# Тест стиля (если есть clang-tidy)
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY AND ENABLE_STYLE_TEST)
    add_executable(style_test style_test.cpp)
    target_include_directories(style_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
    
    set_target_properties(style_test PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY};-checks=*;-warnings-as-errors=*"
    )
    
    add_test(NAME style_test COMMAND style_test)
endif()
