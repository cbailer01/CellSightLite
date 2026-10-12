cmake_minimum_required(VERSION 3.16)
project(CellSightLite VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dossier des donnees synthetiques, connu des tests et de l'application.
set(CELLSIGHT_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

# Avertissements utiles. Les parametres inutilises sont toleres : dans le kit,
# les fonctions a ecrire ne contiennent encore qu'un « throw NotImplemented ».
if(MSVC)
    add_compile_options(/W4 /wd4100 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter)
endif()

add_subdirectory(core)

enable_testing()
add_subdirectory(tests)

# L'interface graphique n'est construite que si Qt 6 est disponible : la
# bibliotheque et les tests fonctionnent sans Qt.
find_package(Qt6 QUIET COMPONENTS Widgets Charts)
if(Qt6_FOUND)
    add_subdirectory(gui)
else()
    message(STATUS "Qt 6 (Widgets + Charts) introuvable : interface graphique non construite.")
endif()
