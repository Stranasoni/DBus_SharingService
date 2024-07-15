# Минимальная версия CMake, с которой обеспечивается совместимость
cmake_minimum_required(VERSION 3.16.3)

# Имя вашего проекта
project(SharingServiceLib VERSION 1.0.0 LANGUAGES CXX)

# Настройка стандартов C++ для компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск пакета Qt5DBus
set(CMAKE_PREFIX_PATH "/opt/Qt/6.7.2/gcc_64")

find_package(Qt6 REQUIRED COMPONENTS Core DBus)

# Добавление библиотеки (динамической)
add_library(SharingService SHARED
/home/ubuntu/Documents/dbusold/my_lib/SharingService.cpp
)
target_include_directories(SharingService  PUBLIC
    /home/ubuntu/Documents/dbusold/my_lib
)

# Указание включаетмых каталогов
# target_include_directories(SharingService PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
set(CMAKE_AUTOMOC ON)


# Подключение необходимых библиотек Qt
target_link_libraries(SharingService PUBLIC  Qt6::Core Qt6::DBus)

# Добавление установки для необходимой конфигурации
# include(GNUInstallDirs)
# install(TARGETS SharingService 
#         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

