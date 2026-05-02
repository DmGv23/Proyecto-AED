cmake_minimum_required(VERSION 3.16)
project(PROYECTOAED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(include)

# ─────────────────────────────────────────────
# Ejecutable de consola (tests / demo)
# ─────────────────────────────────────────────
add_executable(PROYECTOAED_console
    src/main.cpp
    src/SparseMatrix.cpp
)

# ─────────────────────────────────────────────
# Ejecutable con interfaz gráfica Qt6
# ─────────────────────────────────────────────
find_package(Qt6 COMPONENTS Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets QUIET)
endif()

if(Qt6_FOUND)
    add_executable(PROYECTOAED_gui
        src/main_gui.cpp
        src/SparseMatrix.cpp
        src/MainWindow.cpp
    )
    target_link_libraries(PROYECTOAED_gui Qt6::Widgets)
    message(STATUS "Qt6 encontrado — se compilará la interfaz gráfica")
elseif(Qt5_FOUND)
    add_executable(PROYECTOAED_gui
        src/main_gui.cpp
        src/SparseMatrix.cpp
        src/MainWindow.cpp
    )
    target_link_libraries(PROYECTOAED_gui Qt5::Widgets)
    message(STATUS "Qt5 encontrado — se compilará la interfaz gráfica")
else()
    message(WARNING "Qt no encontrado — solo se compilará la versión consola")
endif()
