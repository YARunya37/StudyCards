# Здесь создаются модули, отвечающие за пользовательский интерфейс
# При появлении кода надо будет добавить его как библлиотеку и после подключить в src

find_package(Qt6 REQUIRED COMPONENTS Widgets)
# Создаём библиотеку UI модуля
add_library(main_ui_module STATIC
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    filetreewidget.h filetreewidget.cpp
    studycard.h studycard.cpp
)

# Подключаем Qt к модулю
target_link_libraries(main_ui_module PUBLIC
    Qt6::Widgets
    file_manager
)

# Для доступа к header-ам main_ui_module
target_include_directories(main_ui_module PUBLIC
"C:/Projects/StudyCards/src/ui/"
"C:/Projects/StudyCards/src/core"
"C:/Projects/StudyCards/build/"
)

add_library(groups_ui


)

target_link_libraries(groups_ui PUBLIC
    Qt6::Widgets

)
