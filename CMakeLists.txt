cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 23)
project(evlist)

include(FetchContent)
# Fetch content from this repo.
FetchContent_Declare(
  toolbelt
  GIT_REPOSITORY https://github.com/mmalenic/cmake-toolbelt
  GIT_TAG v0.3.0)
FetchContent_MakeAvailable(toolbelt)

list(APPEND CMAKE_MODULE_PATH "${toolbelt_SOURCE_DIR}/src")
include(toolbelt)

if(RUN_CLANG_TIDY)
  set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

set(LIBRARY_NAME libevlist)
add_library(
  ${LIBRARY_NAME}
  include/evlist/InputDevice.h include/evlist/InputDeviceLister.h
  src/InputDevice.cpp src/InputDeviceLister.cpp)
target_include_directories(${LIBRARY_NAME} PUBLIC include)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARY_NAME})

if(BUILD_TESTING)
  enable_testing()
  set(TEST_EXECUTABLE_NAME evlisttest)

  add_executable(
    ${TEST_EXECUTABLE_NAME}
    test/InputDeviceListerTest.cpp test/InputDeviceTest.cpp
    test/utils/ListInputDevicesTestUtils.h
    test/utils/ListInputDevicesTestUtils.cpp)
  target_include_directories(${TEST_EXECUTABLE_NAME} PUBLIC utils)

  toolbelt_setup_gtest(${TEST_EXECUTABLE_NAME} ADD_LIBRARIES ${LIBRARY_NAME})
endif()
