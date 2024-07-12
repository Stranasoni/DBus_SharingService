 cmake_minimum_required(VERSION 3.16.3)

 project(dbus VERSION 1.0.0 LANGUAGES CXX)

 set(CMAKE_CXX_STANDARD 17)
 set(CMAKE_CXX_STANDARD_REQUIRED ON)

 set(CMAKE_PREFIX_PATH "/opt/Qt/6.7.2/gcc_64")

find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt_standard_project_setup()

qt_add_executable(helloworld
    src/main.cpp
    src/dbus_service.h
    src/dbus_service.cpp
)

target_link_libraries(helloworld PRIVATE Qt6::Core Qt6::DBus)

