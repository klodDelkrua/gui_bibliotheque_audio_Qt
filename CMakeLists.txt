cmake_minimum_required(VERSION 3.16)

project(bibliotheque_audio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- CONFIGURATION POSTGRESQL FORCÉE ---
# Sur Fedora, libpqxx est dans /usr/include et les libs dans /usr/lib64
set(PQXX_INCLUDE_DIRS "/usr/include")
set(PQXX_LIBRARY_DIRS "/usr/lib64")
set(PQXX_LIBRARIES "pqxx")

# Indique au linker où trouver le fichier libpqxx.so
link_directories(${PQXX_LIBRARY_DIRS})
# ------------------------------------------

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES bibliotheque_audio_ru_RU.ts)

# Liste complète des sources
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        audio_player.h
        audio_player.cpp
        authManager.h
        authManager.cpp
        user_account.h
        user_account.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(bibliotheque_audio
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(bibliotheque_audio
        ${PROJECT_SOURCES}
    )
endif()

# --- CONFIGURATION DES INCLUDES (APRES LA CREATION DE LA CIBLE) ---
# On force l'inclusion de /usr/include spécifiquement pour notre projet
target_include_directories(bibliotheque_audio PRIVATE ${PQXX_INCLUDE_DIRS})

target_link_libraries(bibliotheque_audio PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${PQXX_LIBRARIES}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(bibliotheque_audio)
endif()
