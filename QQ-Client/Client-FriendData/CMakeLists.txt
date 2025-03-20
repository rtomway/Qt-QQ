# FriendData 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Client-FriendData STATIC
    Friend.h
    Friend.cpp
    FriendManager.h
    FriendManager.cpp
)

#外部库
#find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebSockets Network)

# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Client-FriendData PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Client-NetWork
   )
#
target_link_libraries(Client-FriendData PRIVATE  #FriendData链接的库不传递给链接FriendData库的使用者
    Client-NetWork Client-Util Client-ChatMessage 
    Qt6::Core Qt6::Widgets Qt6::WebSockets Qt6::Network
)
