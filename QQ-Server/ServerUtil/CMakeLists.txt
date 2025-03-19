
# 为 UI 模块添加一个目标
add_library(ServerUtil STATIC 
    SSqlConnectionPool.h
    SSqlConnectionPool.cpp
    SResultCode.h
)

#库
#find_package(Qt6 REQUIRED COMPONENTS Core Sql WebSockets)

# 设置头文件目录
target_include_directories(ServerUtil PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接 Qt 库（如果需要）
target_link_libraries(ServerUtil PRIVATE Qt6::WebSockets Qt6::Core Qt6::Sql)
