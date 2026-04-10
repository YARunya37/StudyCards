# Модуль FileLoader (конвертация документов)

# Исходники
set(FILELOADER_SOURCES
    FileLoader.cpp
    pch.cpp
)

# Заголовки
set(FILELOADER_HEADERS
    common.h
    FileLoader.h
    pch.h
)

# Создаём статическую библиотеку
add_library(core_module STATIC
    ${FILELOADER_SOURCES}
    ${FILELOADER_HEADERS}
)

# Предкомпилированные заголовки
target_precompile_headers(core_module PRIVATE pch.h)

# Стандарт C++17
target_compile_features(core_module PRIVATE cxx_std_17)