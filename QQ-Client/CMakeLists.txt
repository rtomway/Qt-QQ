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

add_subdirectory(UI)
add_subdirectory(NetWork)
add_subdirectory(Util)
add_subdirectory(FriendData)
add_subdirectory(ChatMessage)


add_executable(${PROJECT_NAME} ${PROJECT_SOURCES_CPP} ${PROJECT_SOURCES_QRC})

target_include_directories(QQ-Client
		PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/NetWork
)

target_link_libraries(${PROJECT_NAME} PRIVATE  Qt6::Widgets Qt6::WebSockets ChatMessage FriendData NetWork UI Util)