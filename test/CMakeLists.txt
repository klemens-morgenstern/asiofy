file(GLOB ALL_TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(asiofy_tests ${ALL_TEST_FILES})
target_link_libraries(asiofy_tests PUBLIC Boost::system )
target_compile_definitions(asiofy_tests PUBLIC asiofy_SEPARATE_COMPILATION=1)


add_test(NAME asiofy_tests COMMAND asiofy_tests)

