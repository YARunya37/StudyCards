# Здесь размещаются модули логики прилошения(по типу конвертора данных, системы тестирования и т.д.)
# При появлении кода надо будет добавить его как библлиотеку и после подключить в src

add_library(file_manager STATIC
    filemanager.h
    filemanager.cpp
)

target_link_libraries(file_manager PUBLIC
    Qt6::Core
)

add_library(group_manager STATIC
    groupmanager.h
    groupmanager.cpp
)

target_link_libraries(group_manager PUBLIC
    Qt6::Core
)
