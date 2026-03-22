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
