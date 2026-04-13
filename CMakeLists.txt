cmake_minimum_required(VERSION 3.19)
project(StudyCards LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_translations(
    TARGETS StudyCards
    TS_FILES StudyCards_ru_RU.ts
)

include(GNUInstallDirs)

# Подключение подкаталогов
add_subdirectory(src)

# Копирование Pandoc при сборке
if(WIN32)
    add_custom_command(TARGET StudyCards POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:StudyCards>/utils/pandoc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/utils/pandoc/pandoc.exe"
            $<TARGET_FILE_DIR:StudyCards>/utils/pandoc/
    )
endif()