cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 23)
project(evlist)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
# Fetch content from this repo.
FetchContent_Declare(
  toolbelt
  GIT_REPOSITORY https://github.com/mmalenic/cmake-toolbelt
  GIT_TAG v0.3.0)
FetchContent_MakeAvailable(toolbelt)

list(APPEND CMAKE_MODULE_PATH "${toolbelt_SOURCE_DIR}/src")
include(toolbelt)

if(RUN_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

set(LIBRARY_NAME libevlist)
add_library(
  ${LIBRARY_NAME} include/evlist/device.h include/evlist/list.h
                  include/evlist/cli.h src/device.cpp src/list.cpp src/cli.cpp)
target_include_directories(${LIBRARY_NAME} PUBLIC include)
toolbelt_add_dep(${LIBRARY_NAME} CLI11 LINK_COMPONENTS CLI11::CLI11)
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARY_NAME})

file(STRINGS "LICENSE" LICENSE)
list(GET LICENSE 0 LICENSE_NAME)
list(GET LICENSE 2 LICENSE_COPYRIGHT)
target_compile_definitions(${LIBRARY_NAME}
                           PUBLIC EVLIST_LICENSE="${LICENSE_NAME}")
target_compile_definitions(${LIBRARY_NAME}
                           PUBLIC EVLIST_COPYRIGHT="${LICENSE_COPYRIGHT}")
target_compile_definitions(${LIBRARY_NAME} PUBLIC EVLIST_VERSION="0.1.0")

if(BUILD_TESTING)
  enable_testing()
  set(TEST_EXECUTABLE_NAME evlisttest)

  add_executable(
    ${TEST_EXECUTABLE_NAME}
    tests/list_test.cpp tests/device_test.cpp tests/utils/evlist_test.h
    tests/utils/evlist_test.cpp)
  target_include_directories(${TEST_EXECUTABLE_NAME} PUBLIC tests)

  toolbelt_setup_gtest(${TEST_EXECUTABLE_NAME} ADD_LIBRARIES ${LIBRARY_NAME})
endif()
