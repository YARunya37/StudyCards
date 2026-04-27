# Здесь создаются модули, отвечающие за пользовательский интерфейс
# При появлении кода надо будет добавить его как библлиотеку и после подключить в src

find_package(Qt6 REQUIRED COMPONENTS Widgets)
# Создаём библиотеку UI модуля
add_library(main_ui_module STATIC
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    filetreewidget.h filetreewidget.cpp
)

# Подключаем Qt к модулю
target_link_libraries(main_ui_module PUBLIC
    Qt6::Widgets
    file_manager
    study_cards
)

# Для доступа к header-ам main_ui_module
target_include_directories(main_ui_module PUBLIC
"C:/Projects/StudyCards/src/ui/"
"C:/Projects/StudyCards/src/core"
"C:/Projects/StudyCards/build/"
)

# Создаём библиотеку для билетов
add_library(study_cards STATIC
    studycard.cpp
    studycard.h
)
target_link_libraries(study_cards PUBLIC
    Qt6::Widgets
    file_manager
)

target_include_directories(study_cards PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui/
    ${CMAKE_SOURCE_DIR}/src/core/
)


# Библиотека для групп
add_library(groups_ui STATIC

)

target_link_libraries(groups_ui PUBLIC
    Qt6::Widgets
    groups_managament
)

target_include_directories(groups_ui PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui/
    ${CMAKE_SOURCE_DIR}/src/core/
)

