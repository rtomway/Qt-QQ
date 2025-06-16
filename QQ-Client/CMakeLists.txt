cmake_minimum_required(VERSION 3.14)

# 在设置工具链,添加前缀路径
set(CMAKE_TOOLCHAIN_FILE "F:/Vcpkg/vcpkg-master/scripts/buildsystems/vcpkg.cmake")
list(APPEND CMAKE_PREFIX_PATH "F:/Vcpkg/vcpkg-master/installed/x64-windows")

project(QQ-Client VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#c++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vcpkg头文件库路径
include_directories("F:/Vcpkg/vcpkg-master/installed/x64-windows/include")

#外部库(主cmake find 子模块无需find)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebSockets Network)
find_package(fmt REQUIRED)


#子模块
add_subdirectory(Client-UI)
add_subdirectory(Client-NetWork)
add_subdirectory(Client-Util)
add_subdirectory(Client-Core)
add_subdirectory(Client-DataCenter)
add_subdirectory(Client-ChatMessage)
add_subdirectory(Client-Logger)


#运行程序
add_executable(${PROJECT_NAME}
		main.cpp
		Resource.qrc
  "Client-ServiceLocator/NetWorkServiceLocator.h" "Client-ServiceLocator/NetWorkServiceLocator.cpp")


  target_include_directories(Client-DataCenter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ServiceLocator
)

#链接库（）
target_link_libraries(${PROJECT_NAME} PRIVATE 
	Client-ChatMessage Client-DataCenter Client-NetWork   #内部
	Client-UI Client-Util Client-Core
	Qt6::Core Qt6::Widgets Qt6::WebSockets Qt6::Network			#外部
	fmt::fmt
)

# 添加构建后命令：复制所需的 DLL 文件到可执行文件目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<$<CONFIG:DeBug>:F:/Vcpkg/vcpkg-master/installed/x64-windows/debug/bin/fmtd.dll>"
	"$<$<CONFIG:Release>:F:/Vcpkg/vcpkg-master/installed/x64-windows/bin/fmt.dll>"
     $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "复制 fmt DLL 到输出目录"
)