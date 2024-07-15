
cmake_minimum_required(VERSION 3.16.3)
project(TestApp)

# Установим C++ стандарт
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка пути для поиска пакетов Qt
set(CMAKE_PREFIX_PATH "/opt/Qt/6.7.2/gcc_64")

# Поиск необходимых библиотек Qt
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

qt_standard_project_setup()

# Создание исполняемого файла
add_executable(TestApp main.cpp)

# Указание пути к заголовочным файлам библиотеки
target_include_directories(TestApp PUBLIC
    ../
)

# Указание пути к скомпилированной библиотеке
target_link_directories(TestApp PRIVATE 
    ../build
)
set(CMAKE_AUTOMOC ON)
# Подключение библиотек
target_link_libraries(TestApp PRIVATE
    libSharingService.so
    Qt6::Core
    Qt6::DBus
)
