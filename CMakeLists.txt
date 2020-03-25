cmake_minimum_required(VERSION 2.8.12)
project(fitsReader)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/)

add_executable(fitsReader src/main.cpp src/fits.cpp)
TARGET_LINK_LIBRARIES(fitsReader ${CONAN_LIBS})