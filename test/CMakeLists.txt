add_executable(default_map_test default_map_test.cpp)

target_link_libraries(default_map_test PRIVATE acid_map gtest_main)

target_compile_options(default_map_test PRIVATE -Wall -Werror -pedantic -fsanitize=address)
target_link_options(default_map_test PRIVATE -fsanitize=address)

add_test(NAME default_map_test COMMAND default_map_test)