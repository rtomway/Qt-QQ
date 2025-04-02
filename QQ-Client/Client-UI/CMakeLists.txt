# UI 模块的 CMake 配置

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

# 查找所有的源文件和头文件
file(GLOB UI_SOURCES
    MainWindow.cpp
    LoginWidget/*.cpp
    MainWidget/*.cpp
    MainWidget/AddFriendWidget/*.cpp
    MainWidget/ContactPage/*.cpp
    MainWidget/ChatPage/*.cpp
    MainWidget/NoticePage/*.cpp
    MainWidget/GroupInviteWidget/*.cpp
    MainWidget/ItemWidget/*.cpp
)
file(GLOB UI_HEADERS
    MainWindow.h
    LoginWidget/*.h
    MainWidget/*.h
    MainWidget/AddFriendWidget/*.h
    MainWidget/ContactPage/*.h
    MainWidget/ChatPage/*.h
    MainWidget/NoticePage/*.h
    MainWidget/GroupInviteWidget/*.h
    MainWidget/ItemWidget/*.h
)
# 为 UI 模块添加一个目标
add_library(Client-UI STATIC ${UI_SOURCES} ${UI_HEADERS})

#公开目录供外界访问
target_include_directories(Client-UI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    #子文件互相访问
    ${CMAKE_CURRENT_SOURCE_DIR}/LoginWidget
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/AddFriendWidget
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/ContactPage
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/ChatPage
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/NoticePage
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/GroupInviteWidget
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/MessageListItem
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/ItemWidget
)

# 链接 库（如果需要）
target_link_libraries(Client-UI PRIVATE
    Qt6::Widgets Qt6::Core 
    Client-Util Client-NetWork Client-DataCenter Client-ChatMessage

)
