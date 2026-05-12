# Модули, отвечающие за внутреннюю логику проекта

# Модуль FileLoader (конвертация документов)

# Исходники
set(FILELOADER_SOURCES
    FileLoader.cpp
)

# Заголовки
set(FILELOADER_HEADERS
    FileLoader.h
)

# Создаём статическую библиотеку
add_library(core_module STATIC
    ${FILELOADER_SOURCES}
    ${FILELOADER_HEADERS}
)

# Стандарт C++17
target_compile_features(core_module PRIVATE cxx_std_17)

target_link_libraries(core_module PRIVATE
    Qt6::Core
)

# Модуль управляющий файлами внутри проекта
add_library(file_manager STATIC
    filemanager.h
    filemanager.cpp
)

target_link_libraries(file_manager PUBLIC
    Qt6::Core
)

# Библиотека для управления документами (сохранение/загрузка)
add_library(document_manager STATIC


)

target_link_libraries(document_manager PUBLIC
    Qt6::Core
    Qt6::Gui
    file_manager
    core_module
)

target_include_directories(document_manager PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core
)

add_library(groups_managament STATIC
    groupmanager.h
    groupmanager.cpp
    group.h group.cpp
)

target_link_libraries(groups_managament PUBLIC
    Qt6::Core
    study_cards
    file_items_managers
)

target_include_directories(groups_managament PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui/
)

# Библиотека для управления файловыми объектами
add_library(file_items_managers STATIC
    ifileitemsmanager.h ifileitemsmanager.cpp
    diritemsmanager.h diritemsmanager.cpp
    namedfileitem.h namedfileitem.cpp
)

target_link_libraries(file_items_managers PUBLIC
    Qt6::Core
)

target_include_directories(groups_managament PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core
)
