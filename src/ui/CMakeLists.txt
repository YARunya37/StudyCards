# Здесь создаются модули, отвечающие за пользовательский интерфейс

find_package(Qt6 REQUIRED COMPONENTS Widgets)


# Объединяем Toolbar и RichTextEdit, так как они работают вместе
add_library(text_editor STATIC
    textformattingtoolbar.h
    textformattingtoolbar.cpp
    richtextedit.h
    richtextedit.cpp
)

target_link_libraries(text_editor
    PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets
)

target_include_directories(text_editor
    PUBLIC ${CMAKE_SOURCE_DIR}/src/ui
)

# Библиотека main_ui_module
add_library(main_ui_module STATIC
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    filetreewidget.h
    filetreewidget.cpp
)

# Подключаем Qt к модулю
target_link_libraries(main_ui_module PUBLIC
    Qt6::Widgets
    core_module
    file_manager
    text_editor
    study_cards
    groups_ui
    temp_cards
    document_ui
    document_manager
)

# Для доступа к header-ам
target_include_directories(main_ui_module PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
)

# Библиотека для управления временными билетами
add_library(temp_cards STATIC
    tempcardswidget.h tempcardswidget.cpp
)

target_link_libraries(temp_cards PRIVATE
    Qt6::Widgets
    study_cards
    groups_managament
)

target_include_directories(temp_cards PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui/
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
    # text_editor
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
    test_window
    text_editor
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

# Библиотека для карточек-вопросов
add_library(questions STATIC
    base_question.h base_question.cpp
    SimpleQuestion.h SimpleQuestion.cpp

)

target_link_libraries(questions PRIVATE
    Qt6::Widgets
    study_cards
)

target_include_directories(questions PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui/
)

# Библиотека окна теста
add_library(test_window STATIC
    testwindow.h testwindow.cpp
)

target_link_libraries(test_window PRIVATE
    Qt6::Widgets
    testing_sys_core
)

target_include_directories(test_window PUBLIC
    ${CMAKE_SOURCE_DIR}/src/ui/
    ${CMAKE_SOURCE_DIR}/src/core/
)
