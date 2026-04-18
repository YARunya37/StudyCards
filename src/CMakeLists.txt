# Подключение всех поддиректорий с кодом
add_subdirectory(ui)
add_subdirectory(core)
add_subdirectory(ai)
add_subdirectory(utils)

# Вход в программу
qt_add_executable(StudyCards
    WIN32 MACOSX_BUNDLE
    main.cpp
    # core/Main.cpp
)

# Подключаем все необходимые компоненты для РАБОТЫ ПРОГРАММЫ
target_link_libraries(StudyCards
PRIVATE
    Qt::Core
    Qt::Widgets
    main_ui_module
    core_module
)

# Копирование Pandoc при сборке
if(WIN32)
    add_custom_command(TARGET StudyCards POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:StudyCards>/utils/pandoc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/src/utils/pandoc/pandoc.exe"
            $<TARGET_FILE_DIR:StudyCards>/utils/pandoc/
        COMMENT "Copying pandoc.exe to build directory..."
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
