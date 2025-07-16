
# 为 UI 模块添加一个目标
add_library(Server-Util STATIC 
    SResultCode.h
    ImageUtil.h ImageUtil.cpp
    CreateId.h   CreateId.cpp
    GlobalTypes.h
    PacketCreate.h
    PacketCreate.cpp
)

#库
#find_package(Qt6 REQUIRED COMPONENTS Core Sql WebSockets)

# 设置头文件目录
target_include_directories(Server-Util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接 Qt 库（如果需要）
target_link_libraries(Server-Util PRIVATE
    Qt6::WebSockets Qt6::Core Qt6::Sql Qt6::Gui
    Server-DataBase
)
