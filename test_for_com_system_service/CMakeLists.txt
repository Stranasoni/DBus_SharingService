cmake_minimum_required(VERSION 3.16.3)

project(text_editor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "../external/gcc_64")

find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt_standard_project_setup()

qt_add_executable(text_editor 
    main.cpp
    text_editor.h
    text_editor.cpp
)

target_link_libraries(text_editor PRIVATE Qt6::Core Qt6::DBus)