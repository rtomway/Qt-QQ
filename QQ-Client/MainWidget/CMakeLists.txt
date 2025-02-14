cmake_minimum_required(VERSION 3.14)

project(MainWidget VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED Widgets WebSockets)


file(GLOB MAIN_SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB MAIN_SOURCES_QRC ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)
file(GLOB ADDFRIENDWIDGET_CPP ${CMAKE_CURRENT_SOURCE_DIR}/AddFriendWidget/*.cpp)

add_subdirectory(MessagePage)
add_subdirectory(ContactPage)


list(APPEND MAINWIDGET_FILE ${MAIN_SOURCES_CPP} ${MAIN_SOURCES_QRC}${ADDFRIENDWIDGET_CPP})

add_library(${PROJECT_NAME} STATIC ${MAINWIDGET_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} AddFriendWidget MessagePage ContactPage)


target_link_libraries(${PROJECT_NAME} PRIVATE  Qt6::Widgets Qt6::WebSockets MessagePage ContactPage Util)

