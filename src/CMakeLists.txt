# Подключение всех поддиректорий с кодом
add_subdirectory(ui)
add_subdirectory(core)
add_subdirectory(ai)

# Вход в программу
qt_add_executable(StudyCards
    WIN32 MACOSX_BUNDLE
    main.cpp
    # core/Main.cpp
)

target_link_libraries(StudyCards
PRIVATE
    Qt::Core
    Qt::Widgets
    main_ui_module
    core_module
    file_manager
)
target_include_directories(main_ui_module PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/build
)

# Добавление папки resources с копированием всего её содержимого
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

# Очистка папки userfiles от .gitkeep
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/userfiles/.gitkeep"
)

# Очистка папки usergroups от .gitkeep
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/usergroups/.gitkeep"
)

# Копирование Pandoc при сборке
if(WIN32)
    # Путь к pandoc.exe в исходниках
    set(PANDOC_SOURCE "${CMAKE_SOURCE_DIR}/src/utils/pandoc/pandoc.exe")

    # Путь куда копировать (в билд директорию)
    set(PANDOC_DESTINATION "${CMAKE_BINARY_DIR}/utils/pandoc/pandoc.exe")



    # Создаём папку utils/pandoc
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/utils/pandoc")

    # Копируем pandoc.exe
    add_custom_command(TARGET StudyCards POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PANDOC_SOURCE}"
            "${PANDOC_DESTINATION}"
        COMMENT "Copying pandoc.exe to build directory..."
        VERBATIM
    )
endif()

# Хз, что тут. QT сам создал это, я не трогал
install(TARGETS StudyCards
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Тут тоже
qt_generate_deploy_app_script(
    TARGET StudyCards
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

# Ну и здесь
install(SCRIPT ${deploy_script})
