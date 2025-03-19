# 查找所有的源文件和头文件
file(GLOB UI_SOURCES
    LoginWidget/*.cpp
    MainWidget/*.cpp
    MainWidget/AddFriendWidget/*.cpp
    MainWidget/ContactPage/*.cpp
    MainWidget/MessagePage/*.cpp
)

file(GLOB UI_HEADERS
    LoginWidget/*.h
    MainWidget/*.h
    MainWidget/AddFriendWidget/*.h
    MainWidget/ContactPage/*.h
    MainWidget/MessagePage/*.h
)

# 为 UI 模块添加一个目标
add_library(UI STATIC ${UI_SOURCES} ${UI_HEADERS})

# 设置头文件目录
target_include_directories(UI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    #子文件互相访问
    ${CMAKE_CURRENT_SOURCE_DIR}/LoginWidget
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/AddFriendWidget
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/ContactPage
    ${CMAKE_CURRENT_SOURCE_DIR}/MainWidget/MessagePage
    ${CMAKE_CURRENT_SOURCE_DIR}/../FriendData  # FriendData 模块的路径
    ${CMAKE_CURRENT_SOURCE_DIR}/../ChatMessage  # ChatMessage 模块的路径
    ${CMAKE_CURRENT_SOURCE_DIR}/../Util  # Util 模块的路径
      ${CMAKE_CURRENT_SOURCE_DIR}/../NetWork # Util 模块的路径
)

# 链接 Qt 库（如果需要）
target_link_libraries(UI PRIVATE Qt6::Widgets Qt6::Core Util NetWork FriendData ChatMessage)
