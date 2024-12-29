CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(Settings VERSION 0.1 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)
SET(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)

SET(SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        connecteth.cpp
        connecteth.h
        connecteth.ui
        ip_validator.h
)

ADD_EXECUTABLE(Settings ${SOURCES})
TARGET_LINK_LIBRARIES(Settings ${QT_LIBS})
