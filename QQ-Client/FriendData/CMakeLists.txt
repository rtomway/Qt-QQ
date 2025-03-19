# NetWork 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(FriendData STATIC
    Friend.h
    Friend.cpp
    FriendManager.h
    FriendManager.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebSockets Network)

# 设置包含目录（确保主程序能访问头文件）
target_include_directories(FriendData PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../NetWork
   )
#
target_link_libraries(FriendData NetWork Util ChatMessage Qt6::Core Qt6::Widgets Qt6::WebSockets Qt6::Network)
