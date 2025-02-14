cmake_minimum_required(VERSION 3.14)

project(QQ-Client VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED Widgets WebSockets)


file(GLOB PROJECT_SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB PROJECT_SOURCES_QRC ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

add_subdirectory(LoginWidget)
add_subdirectory(MainWidget)

#添加宏定义
#add_compile_definitions(QT_NO_DEBUG_OUTPUT)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES_CPP} ${PROJECT_SOURCES_QRC})

target_link_libraries(${PROJECT_NAME} PRIVATE  Qt6::Widgets Qt6::WebSockets  LoginWidget MainWidget Util)

