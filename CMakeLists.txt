# CMakeLists.txt for the static library
cmake_minimum_required (VERSION 3.2...4.2)

set(LIB_NAME mystusb)

add_library(${LIB_NAME} STATIC MyStUsb.c)



target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${LIB_NAME} PRIVATE wheap)

target_link_libraries(${LIB_NAME} PRIVATE interface)