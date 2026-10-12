set(CMAKE_AUTOMOC ON)

add_executable(cellsight_gui WIN32
    main.cpp
    Charts.cpp
    Charts.hpp
    LoginDialog.cpp
    LoginDialog.hpp
    MainWindow.cpp
    MainWindow.hpp
)
target_link_libraries(cellsight_gui PRIVATE cellsight_core Qt6::Widgets Qt6::Charts)
target_compile_definitions(cellsight_gui PRIVATE CELLSIGHT_DATA_DIR="${CELLSIGHT_DATA_DIR}")
