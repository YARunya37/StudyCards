# Здесь создаются модули, отвечающие за пользовательский интерфейс

find_package(Qt6 REQUIRED COMPONENTS Widgets)


# Библиотека 1: text_formatting_toolbar
add_library(text_formatting_toolbar STATIC
    textformattingtoolbar.h
    textformattingtoolbar.cpp
)

target_link_libraries(text_formatting_toolbar
    PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets
)

target_include_directories(text_formatting_toolbar
    PUBLIC ${CMAKE_SOURCE_DIR}/src/ui
)

# Библиотека 2: main_ui_module
add_library(main_ui_module STATIC
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    scaledtextedit.h scaledtextedit.cpp
    filetreewidget.h filetreewidget.cpp
)

# Подключаем Qt к модулю
target_link_libraries(main_ui_module PUBLIC
    Qt6::Widgets
    core_module
    file_manager
    text_formatting_toolbar
)

# Для доступа к header-ам
target_include_directories(main_ui_module PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
)
