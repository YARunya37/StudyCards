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
