cmake_minimum_required(VERSION 3.19)
project(homeworkdemo LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(homeworkdemo
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

qt_add_resources(homeworkdemo "resources"
    PREFIX "/"
    FILES
        images/minish_woods.png
)

target_link_libraries(homeworkdemo
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS homeworkdemo
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET homeworkdemo
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
