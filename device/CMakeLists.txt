# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(code)

target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/device.crt" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/device.key" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/ca.crt" TEXT)

project(esp-code-recognition)