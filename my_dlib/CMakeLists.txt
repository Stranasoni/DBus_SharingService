cmake_minimum_required(VERSION 3.16.3)

project(SharingServiceLib VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) 
set(CMAKE_PREFIX_PATH "../external/gcc_64")

find_package(Qt6 REQUIRED COMPONENTS Core DBus)


add_library(SharingService SHARED lib.cpp lib.h)
target_include_directories(SharingService  PUBLIC
${CMAKE_CURRENT_SOURCE_DIR} )


target_link_libraries(SharingService PUBLIC  Qt6::Core Qt6::DBus)



