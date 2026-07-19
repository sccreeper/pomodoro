cmake_minimum_required(VERSION 3.16)

project(pomo_qt_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json 3.12.0 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

qt_add_executable(apppomo_qt_app
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/timer_model.h
    src/timer_model.cpp
    src/utils.h
    src/utils.cpp
    src/window.ui
    src/config.h
    src/config.cpp
)

qt_add_resources(apppomo_qt_app "resources" PREFIX "/" 
    FILES 
        "resources/icon.png"
        "resources/default_config.json"
    )

target_link_libraries(apppomo_qt_app
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        nlohmann_json::nlohmann_json
)

qt_import_plugins(apppomo_qt_app
    INCLUDE 
        Qt6::QWaylandIntegrationPlugin
        Qt6::QWaylandXdgShellIntegrationPlugin
        Qt6::QWaylandEglClientBufferPlugin
        Qt6::QWaylandBradientDecorationPlugin)


set_target_properties(apppomo_qt_app PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE

    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "pomodoro"
)