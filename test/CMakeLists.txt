add_executable(pure_map_test pure_map_test.cpp)

target_link_libraries(pure_map_test PRIVATE acid_map gtest_main)

target_compile_options(pure_map_test PRIVATE -Wall -Werror -pedantic -fsanitize=address)
target_link_options(pure_map_test PRIVATE -fsanitize=address)

add_test(NAME pure_map_test COMMAND pure_map_test)