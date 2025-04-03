cmake_minimum_required(VERSION 3.28)
project(STInt)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

include_directories(include)

file(GLOB stint_src
    "include/stint.h" "src/stint.c"
)

add_executable(basic_example examples/basic-example.cpp ${stint_src})
add_executable(run_unit_tests test/main.cpp
        test/test_config_table.cpp
        ${stint_src}
)
target_link_libraries(run_unit_tests gtest)
add_test(NAME stint_test COMMAND run_unit_tests)