# Source files and their filters
include(CMakeSources.cmake)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5 # v1.16.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if (WIN32)
    add_executable(
        Tests
        ${TE_TESTS_SRC}
    )
    set_target_properties(Tests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_COMPILER}.Debug.${PLATFORM_TARGET}")
else ()
    add_executable(
        Tests
        ${TE_TESTS_SRC}
    )
endif ()

# Libraries
target_link_libraries (Tests tef GTest::gtest_main) 

# IDE specific
set_property (TARGET Tests PROPERTY FOLDER Tests)
set_property (TARGET gmock PROPERTY FOLDER Tests)
set_property (TARGET gmock_main PROPERTY FOLDER Tests)
set_property (TARGET gtest PROPERTY FOLDER Tests)
set_property (TARGET gtest_main PROPERTY FOLDER Tests)

include(GoogleTest)
gtest_discover_tests(Tests)
