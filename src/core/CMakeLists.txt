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
)

target_include_directories(groups_managament PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui/
)
