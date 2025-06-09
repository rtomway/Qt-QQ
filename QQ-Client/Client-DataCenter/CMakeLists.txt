# DataCenter 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Client-DataCenter STATIC
    Group.h
    Group.cpp
    GroupManager.h
    GroupManager.cpp
	Friend.h
    Friend.cpp
    FriendManager.h
    FriendManager.cpp
    AvatarManager.h
    AvatarManager.cpp
 "TempManager.h" "TempManager.cpp" 
 "LoginUserManager.h" "LoginUserManager.cpp"
 "TokenManager.h" "TokenManager.cpp"
)


# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Client-DataCenter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
   )
#
target_link_libraries(Client-DataCenter PRIVATE  
    Client-NetWork Client-Util Client-ChatMessage 
    Qt6::Core Qt6::Widgets Qt6::WebSockets Qt6::Network
)
