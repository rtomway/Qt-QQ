cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(Util VERSION 1.0 LANGUAGES CXX)

# 启用 Qt 的自动处理功能
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 查找并配置 Qt6 的 Widgets 和 Sql 模块
#find_package(Qt6 REQUIRED COMPONENTS Widgets Sql WebSockets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql WebSockets)

# 查找源文件
file(GLOB UTIL_SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 添加所有源文件到 UTIL_FILE 列表
list(APPEND UTIL_FILE ${UTIL_SOURCES_CPP})

# 添加静态库
add_library(${PROJECT_NAME} STATIC ${UTIL_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 链接库
#target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql Qt6::WebSockets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Sql Qt6::WebSockets)
