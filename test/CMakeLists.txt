set(COMPILER_FLAGS -Wall -pedantic -fsanitize=address)
set(LINKER_FLAGS -fsanitize=address)

add_library(utils STATIC utils.cpp)

set(tests default_map_test consistent_map_test atomic_map_test)
foreach(test IN LISTS tests)
    add_executable(${test} "${test}.cpp")
    target_link_libraries(${test} PRIVATE acid_map utils gtest_main)
    target_compile_options(${test} PRIVATE ${COMPILER_FLAGS})
    target_link_options(${test} PRIVATE ${LINKER_FLAGS})
    add_test(NAME ${test} COMMAND ${test})
endforeach()