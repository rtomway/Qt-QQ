# NetWork 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(NetWork STATIC
    client.cpp
    client.h
)
# 设置包含目录（确保主程序能访问头文件）
target_include_directories(NetWork PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#链接库
target_link_libraries(NetWork Qt6::Core Qt6::Gui Qt6::WebSockets)
