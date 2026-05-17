cmake_minimum_required(VERSION 3.19)
project(StudyCards LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)
set(CMAKE_AUTORCC ON)

# Поиск Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)


qt_standard_project_setup()

include(GNUInstallDirs)

# Подключение подкаталогов (здесь создаётся target StudyCards)
add_subdirectory(src)

# ВРЕМЕННО ОТКЛЮЧЕНО - переводы
# Раскомментируй когда всё будет работать
# qt_add_translations(
#     TARGETS StudyCards
#     TS_FILES StudyCards_ru_RU.ts
# )
