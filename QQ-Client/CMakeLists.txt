cmake_minimum_required(VERSION 3.14)

project(QQ-Client VERSION 1.0 LANGUAGES CXX)
#Qt自动moc
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#c++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#外部库(主cmake find 子模块无需find)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebSockets Network)
#子模块
add_subdirectory(UI)
add_subdirectory(NetWork)
add_subdirectory(Util)
add_subdirectory(FriendData)
add_subdirectory(ChatMessage)
#运行程序
add_executable(${PROJECT_NAME}
		main.cpp
		Resource.qrc
)
#链接库（）
target_link_libraries(${PROJECT_NAME} PRIVATE 
	ChatMessage FriendData NetWork UI Util  #内部
	Qt6::Core Qt6::Widgets Qt6::WebSockets Qt6::Network			#外部
)