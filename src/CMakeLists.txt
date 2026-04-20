# Подключение всех поддиректроий с кодом
add_subdirectory(ui)
add_subdirectory(core)
add_subdirectory(ai)
add_subdirectory(utils)

# Вход в программу
qt_add_executable(StudyCards
    WIN32 MACOSX_BUNDLE
    main.cpp
)

# Подключаем все необходимые компоненты для РАБОТЫ ПРОГРАММЫ(что включили, то и будет работать)
target_link_libraries(StudyCards
PRIVATE
Qt::Core
main_ui_module
file_manager
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
