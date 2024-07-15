
cmake_minimum_required(VERSION 3.16.3)
project(TestApp)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_PREFIX_PATH "../external/gcc_64")
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt_standard_project_setup()


add_executable(TestApp main.cpp)

# Указание пути к заголовочным файлам библиотеки
target_include_directories(TestApp PUBLIC
    ../my_dlib
)

# Указание пути к скомпилированной библиотеке
target_link_directories(TestApp PRIVATE 
    ../my_dlib/build
)

# Подключение библиотек
target_link_libraries(TestApp PRIVATE
    libSharingService.so
    Qt6::Core
    Qt6::DBus
)
