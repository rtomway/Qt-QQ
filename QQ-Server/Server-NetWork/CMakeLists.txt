# NetWork 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Server-NetWork STATIC
        HttpRouteManager.h    HttpRouteManager.cpp
        ConnectionManager.h   ConnectionManager.cpp 
        Server.h  Server.cpp
)

# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Server-NetWork PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#链接库
target_link_libraries(Server-NetWork PUBLIC   #NetWork链接的库传递给链接NetWork库的使用者
    Qt6::Core Qt6::Gui Qt6::WebSockets 
   Server-Core
)