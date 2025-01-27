cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 23)

# x-release-please-start-version
project(
    evlist
    VERSION 0.2.0
    DESCRIPTION "list input event devices"
    HOMEPAGE_URL "https://github.com/mmalenic/evlist"
)
# x-release-please-end

# ~~~
# Configurable options. These are normally set by `conan build`.
# set(EVLIST_BUILD_BINARY TRUE)
# set(EVLIST_RUN_CLANG_TIDY FALSE)
# set(EVLIST_CLANG_TIDY_EXECUTABLE ./path/to/clang-tidy)
# set(BUILD_TESTING FALSE)
# ~~~

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
# Fetch content from this repo.
FetchContent_Declare(
    toolbelt
    GIT_REPOSITORY https://github.com/mmalenic/cmake-toolbelt
    GIT_TAG v0.3.0
)
FetchContent_MakeAvailable(toolbelt)

list(APPEND CMAKE_MODULE_PATH "${toolbelt_SOURCE_DIR}/src")
include(toolbelt)

if(EVLIST_RUN_CLANG_TIDY)
    if(DEFINED EVLIST_CLANG_TIDY_EXECUTABLE)
        set(CMAKE_CXX_CLANG_TIDY "${EVLIST_CLANG_TIDY_EXECUTABLE}")
    else()
        set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
    endif()
endif()

set(LIBRARY_NAME libevlist)
add_library(
    ${LIBRARY_NAME}
    include/evlist/cli.h
    include/evlist/device.h
    include/evlist/evlist.h
    include/evlist/list.h
    src/cli.cpp
    src/device.cpp
    src/list.cpp
)
target_include_directories(${LIBRARY_NAME} PUBLIC include)
toolbelt_add_dep(${LIBRARY_NAME} CLI11 LINK_COMPONENTS CLI11::CLI11)

file(STRINGS "LICENSE" LICENSE)
list(GET LICENSE 0 LICENSE_NAME)
list(GET LICENSE 2 LICENSE_COPYRIGHT)
target_compile_definitions(${LIBRARY_NAME} PUBLIC EVLIST_LICENSE="${LICENSE_NAME}")
target_compile_definitions(${LIBRARY_NAME} PUBLIC EVLIST_COPYRIGHT="${LICENSE_COPYRIGHT}")
target_compile_definitions(${LIBRARY_NAME} PUBLIC EVLIST_VERSION="0.1.0")

if(EVLIST_BUILD_BINARY)
    add_executable(${PROJECT_NAME} src/main.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARY_NAME})
endif()

if(BUILD_TESTING)
    enable_testing()
    set(TEST_EXECUTABLE_NAME evlisttest)

    add_executable(
        ${TEST_EXECUTABLE_NAME} tests/list_test.cpp tests/device_test.cpp tests/common/common.h tests/common/common.cpp
    )
    target_include_directories(${TEST_EXECUTABLE_NAME} PUBLIC tests)

    toolbelt_setup_gtest(${TEST_EXECUTABLE_NAME} ADD_LIBRARIES ${LIBRARY_NAME})
endif()
