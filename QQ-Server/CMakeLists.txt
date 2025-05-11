cmake_minimum_required(VERSION 3.14)
# --- 关键修改 1：指定 vcpkg 工具链（需替换为你的路径） ---
set(CMAKE_TOOLCHAIN_FILE "F:/Vcpkg/vcpkg-master/scripts/buildsystems/vcpkg.cmake")
# 设置项目名称和版本
project(QQ-Server VERSION 1.0 LANGUAGES CXX)

# 启用 Qt 的自动处理功能
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找并配置 Qt6 的 Core 和 Sql 模块
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql WebSockets HttpServer)


# --- 关键修改 2：查找 jwt-cpp ---
# 替换为手动加头文件路径
include_directories("F:/Vcpkg/vcpkg-master/installed/x64-windows/include")

find_package(OpenSSL REQUIRED)

# 查找源文件和资源文件
file(GLOB PROJECT_SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB PROJECT_SOURCES_QRC ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

#子模块
add_subdirectory(Server-Util)
add_subdirectory(Server-Core)
add_subdirectory(Server-NetWork)
add_subdirectory(Server-DataBase)


# 添加可执行文件
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES_CPP} ${PROJECT_SOURCES_QRC})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE 
Server-Util Server-Core Server-NetWork Server-DataBase
Qt6::Core Qt6::Gui Qt6::Sql Qt6::WebSockets Qt6::HttpServer
OpenSSL::Crypto OpenSSL::SSL
)


