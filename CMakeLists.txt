# CMakeLists.txt for the static library
cmake_minimum_required (VERSION 3.2...4.2)

set(LIB_NAME wstusb)

add_library(${LIB_NAME} STATIC wstusb.c)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${LIB_NAME} PRIVATE wheap stm32cubemx)

target_link_libraries(${LIB_NAME} PRIVATE interface)