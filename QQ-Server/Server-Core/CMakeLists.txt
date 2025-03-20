# Core 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Server-Core STATIC
    MessageHandle.h
    MessageHandle.cpp
  "ConnectionManager.h" "ConnectionManager.cpp" 
  "MessageHandle/LoginHandle.h" "MessageHandle/LoginHandle.cpp" 
  "MessageHandle/RegisterHandle.h" "MessageHandle/RegisterHandle.cpp" 
  "MessageHandle/FriendHandle.h" "MessageHandle/FriendHandle.cpp" 
  "MessageHandle/GroupHandle.h" "MessageHandle/GroupHandle.cpp" 
  "MessageHandle/UserHandle.h" "MessageHandle/UserHandle.cpp"
)

# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Server-Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#链接库
target_link_libraries(Server-Core PUBLIC   #Core链接的库传递给链接Core库的使用者
    Qt6::Core Qt6::Gui Qt6::WebSockets Qt6::Sql
    Server-Util Server-NetWork  Server-DataBase
)