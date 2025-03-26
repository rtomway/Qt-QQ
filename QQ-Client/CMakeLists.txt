cmake_minimum_required(VERSION 3.14)

project(QQ-Client VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#c++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#外部库(主cmake find 子模块无需find)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebSockets Network)
#子模块
add_subdirectory(Client-UI)
add_subdirectory(Client-NetWork)
add_subdirectory(Client-Util)
add_subdirectory(Client-Core)
add_subdirectory(Client-FriendData)
add_subdirectory(Client-GroupData)
add_subdirectory(Client-ChatMessage)
#运行程序
add_executable(${PROJECT_NAME}
		main.cpp
		Resource.qrc
 )
#链接库（）
target_link_libraries(${PROJECT_NAME} PRIVATE 
	Client-ChatMessage Client-FriendData Client-NetWork   #内部
	Client-UI Client-Util Client-Core  Client-GroupData
	Qt6::Core Qt6::Widgets Qt6::WebSockets Qt6::Network			#外部
)