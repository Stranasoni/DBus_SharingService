# cmake_minimum_required(VERSION 3.16.3)
# project(TestApp)

# # Настройка стандартов C++
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)



# set(CMAKE_PREFIX_PATH "/opt/Qt/6.7.2/gcc_64")

# find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# add_library(sharing_service SHARED 
#     /home/ubuntu/Documents/dbusold/my_lib/SharingService.cpp
# )

# # Указание пути к заголовочным файлам библиотеки
# target_include_directories(sharing_service  PUBLIC
#     /home/ubuntu/Documents/dbusold/my_lib
# )
# target_link_libraries(sharing_service PRIVATE Qt6::Core Qt6::DBus)
# # Создание исполняемого файла
# add_executable(TestApp main.cpp)
# # Указание пути к библиотеке
# # target_link_directories(TestApp PUBLIC
# #     /home/ubuntu/Documents/dbusold/my_lib/build
# # )
# #link_directories(/home/ubuntu/Documents/dbusold/my_lib/build)

# # Подключение библиотеки
# target_link_libraries(TestApp PRIVATE sharing_service Qt6::Core Qt6::DBus)


cmake_minimum_required(VERSION 3.16.3)
project(TestApp)

# Установим C++ стандарт
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка пути для поиска пакетов Qt
set(CMAKE_PREFIX_PATH "/opt/Qt/6.7.2/gcc_64")

# Поиск необходимых библиотек Qt
find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Создание исполняемого файла
add_executable(TestApp main.cpp)

# Указание пути к заголовочным файлам библиотеки
target_include_directories(TestApp PUBLIC
    /home/ubuntu/Documents/dbusold/my_lib
)

# Указание пути к скомпилированной библиотеке
target_link_directories(TestApp PRIVATE 
    /home/ubuntu/Documents/dbusold/my_lib/build
)
set(CMAKE_AUTOMOC ON)
# Подключение библиотек
target_link_libraries(TestApp PRIVATE
    libSharingService.so
    Qt6::Core
    Qt6::DBus
)
