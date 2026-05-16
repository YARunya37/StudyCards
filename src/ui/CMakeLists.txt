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
    filetreewidget.h filetreewidget.cpp
)

# Подключаем Qt к модулю
target_link_libraries(main_ui_module PUBLIC
    Qt6::Widgets
    core_module
    file_manager
    text_formatting_toolbar
    study_cards
    groups_ui
    document_ui
)

# Для доступа к header-ам
target_include_directories(main_ui_module PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
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
    groupsuicontroller.h groupsuicontroller.cpp
    groupwindow.h groupwindow.cpp
)

target_link_libraries(groups_ui PUBLIC
    Qt6::Widgets
    groups_managament
    dialogs
)

target_include_directories(groups_ui PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui/
    ${CMAKE_SOURCE_DIR}/src/core/
)

# Библиотека для диалогов с польщователем
add_library(dialogs STATIC
    textinputdialog.h textinputdialog.cpp
)

target_link_libraries(dialogs PUBLIC
    Qt6::Widgets
)

target_include_directories(dialogs PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui/
)

# Библиотека для работы с документами в UI
add_library(document_ui STATIC
    documentui.h
    documentui.cpp
)

target_link_libraries(document_ui PUBLIC
    Qt6::Widgets
    file_manager
)

target_include_directories(document_ui PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/core
)
