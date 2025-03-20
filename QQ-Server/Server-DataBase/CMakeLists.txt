
# 为 UI 模块添加一个目标
add_library(Server-DataBase STATIC 
   SSqlConnectionPool.h
    SSqlConnectionPool.cpp
  "DataBaseQuery.h" "DataBaseQuery.cpp")

#库
#find_package(Qt6 REQUIRED COMPONENTS Core Sql WebSockets)

# 设置头文件目录
target_include_directories(Server-DataBase PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接 Qt 库（如果需要）
target_link_libraries(Server-DataBase PRIVATE  Qt6::Sql)
