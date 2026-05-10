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

# Библиотека для ядра системы тестирования
add_library(testing_sys_core STATIC
    testgenerator.h testgenerator.cpp
    testdirector.h testdirector.cpp
)

target_link_libraries(testing_sys_core PRIVATE
    Qt6::Core
    groups_managament
    questions
)

target_include_directories(testing_sys_core PRIVATE
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui/
)
