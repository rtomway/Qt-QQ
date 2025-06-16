# DataCenter 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Client-DataCenter STATIC
	FriendDataMoudle/Friend.h             FriendDataMoudle/Friend.cpp
    FriendDataMoudle/FriendManager.h      FriendDataMoudle/FriendManager.cpp
    FriendDataMoudle/FriendRespository.h  FriendDataMoudle//FriendRespository.cpp
    FriendDataMoudle//FriendService.h     FriendDataMoudle//FriendService.cpp

    GroupDataMoudle/Group.h               GroupDataMoudle/Group.cpp
    GroupDataMoudle/GroupManager.h        GroupDataMoudle/GroupManager.cpp
    GroupDataMoudle/GroupRespository.h    GroupDataMoudle/GroupRespository.cpp
    GroupDataMoudle/GroupService.h        GroupDataMoudle/GroupService.cpp

    AvatarDataMoudle/AvatarManager.h      AvatarDataMoudle/AvatarManager.cpp

    TempDataMoudle/TempManager.h          TempDataMoudle/TempManager.cpp

    LoginUserDataMoudle/LoginUserManager.h    LoginUserDataMoudle/LoginUserManager.cpp

    TokenManager.h          TokenManager.cpp   
)


   # 设置包含目录（确保所有模块能互相访问头文件）
target_include_directories(Client-DataCenter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/FriendDataMoudle
    ${CMAKE_CURRENT_SOURCE_DIR}/GroupDataMoudle
    ${CMAKE_CURRENT_SOURCE_DIR}/AvatarDataMoudle
    ${CMAKE_CURRENT_SOURCE_DIR}/TempDataMoudle
    ${CMAKE_CURRENT_SOURCE_DIR}/LoginUserDataMoudle
)

target_link_libraries(Client-DataCenter PRIVATE  
    Client-NetWork Client-Util Client-ChatMessage 
    Qt6::Core Qt6::Widgets Qt6::WebSockets Qt6::Network
)
