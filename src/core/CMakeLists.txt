# Здесь размещаются модули логики прилошения(по типу конвертора данных, системы тестирования и т.д.)
# При появлении кода надо будет добавить его как библлиотеку и после подключить в src

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

)

target_link_libraries(file_items_managers PUBLIC
    Qt6::Core
)

target_include_directories(groups_managament PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core
)
