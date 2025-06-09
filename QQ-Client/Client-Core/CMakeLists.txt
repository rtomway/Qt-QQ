# Core 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Client-Core STATIC
    "EventBus.h" "EventBus.cpp"
     "Client_MessageHandle/Client_LoginHandle.h" "Client_MessageHandle/Client_LoginHandle.cpp"
     "Client_MessageHandle/Client_FriendHandle.h" "Client_MessageHandle/Client_FriendHandle.cpp"
     "Client_MessageHandle/Client_GroupHandle.cpp" "Client_MessageHandle/Client_GroupHandle.h"
     "Client_MessageHandle/Client_UserHandle.h"  "Client_MessageHandle/Client_UserHandle.cpp"
)

# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Client-Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#链接库
target_link_libraries(Client-Core PUBLIC   #Core链接的库传递给链接Core库的使用者
    Qt6::Core Qt6::Gui Qt6::WebSockets 
    Client-Util Client-DataCenter Client-NetWork
)