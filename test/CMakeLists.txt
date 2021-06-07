set(COMPILER_FLAGS -Wall -Werror -pedantic -fsanitize=address)
set(LINKER_FLAGS -fsanitize=address)

add_executable(default_map_test default_map_test.cpp)
add_executable(consistent_map_test consistent_map_test.cpp)
add_executable(all_tests default_map_test.cpp consistent_map_test)

add_library(utils STATIC utils.cpp)

target_link_libraries(default_map_test PRIVATE acid_map gtest_main utils)
target_link_libraries(consistent_map_test PRIVATE acid_map gtest_main utils)
target_link_libraries(all_tests PRIVATE acid_map gtest_main utils)

target_compile_options(default_map_test PRIVATE ${COMPILER_FLAGS})
target_link_options(default_map_test PRIVATE ${LINKER_FLAGS})
target_link_options(all_tests PRIVATE ${LINKER_FLAGS})

target_compile_options(consistent_map_test PRIVATE ${COMPILER_FLAGS})
target_link_options(consistent_map_test PRIVATE ${LINKER_FLAGS})
target_link_options(all_tests PRIVATE ${LINKER_FLAGS})

add_test(NAME default_map_test COMMAND default_map_test)
add_test(NAME consistent_map_test COMMAND consistent_map_test)