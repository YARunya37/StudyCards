# Здесь создаются модули, отвечающие за пользовательский интерфейс

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Создаём библиотеку UI модуля
add_library(main_ui_module STATIC
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    scaledtextedit.h
    scaledtextedit.cpp
)

# Подключаем Qt к модулю
target_link_libraries(main_ui_module PUBLIC
    Qt6::Widgets
    core_module
)

# Для доступа к header-ам
target_include_directories(main_ui_module PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)
