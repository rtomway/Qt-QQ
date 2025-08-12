# NetWork 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Client-NetWork STATIC
     WebClient.h    WebClient.cpp
     HttpClient.h   HttpClient.cpp
     MessageHandle.h    MessageHandle.cpp
     NetWorkService.h   NetWorkService.cpp
     HttpClientPort.h   
     WebClientPort.h
     MessageQueue.h MessageQueue.cpp
)

# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Client-NetWork PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#链接库
target_link_libraries(Client-NetWork PUBLIC   #NetWork链接的库传递给链接NetWork库的使用者
    Qt6::Core Qt6::Gui Qt6::WebSockets 
    Client-Core Client-Logger
)