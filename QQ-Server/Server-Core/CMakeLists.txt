# Core 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Server-Core STATIC

  Server-MessageHandle/LoginHandle.h        Server-MessageHandle/LoginHandle.cpp
  Server-MessageHandle/RegisterHandle.h     Server-MessageHandle/RegisterHandle.cpp
  Server-MessageHandle/FriendHandle.h       Server-MessageHandle/FriendHandle.cpp
  Server-MessageHandle/GroupHandle.h        Server-MessageHandle/GroupHandle.cpp 
  Server-MessageHandle/UserHandle.h         Server-MessageHandle/UserHandle.cpp
 
  Server-DBUtils/GroupDBUtils.h     Server-DBUtils/GroupDBUtils.cpp
  Server-DBUtils/LoginDBUtils.h     Server-DBUtils/LoginDBUtils.cpp
  Server-DBUtils/FriendDBUtils.h    Server-DBUtils/FriendDBUtils.cpp
  Server-DBUtils/UserDBUtils.h      Server-DBUtils/UserDBUtils.cpp
)
# 添加 include 路径（让头文件可见）
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Server-DBUtils
)

# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Server-Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#链接库
target_link_libraries(Server-Core PUBLIC   #Core链接的库传递给链接Core库的使用者
    Qt6::Core Qt6::Gui Qt6::WebSockets Qt6::Sql Qt6::HttpServer
    Server-Util Server-NetWork  Server-DataBase
)