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
