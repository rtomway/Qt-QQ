cmake_minimum_required(VERSION 3.14)

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
find_package(Qt6 REQUIRED COMPONENTS Core Gui  Sql WebSockets)

# 查找源文件和资源文件
file(GLOB PROJECT_SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB PROJECT_SOURCES_QRC ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

add_subdirectory(ServerUtil)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES_CPP} ${PROJECT_SOURCES_QRC})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE ServerUtil Qt6::Core Qt6::Gui Qt6::Sql Qt6::WebSockets)

