# Logger 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Client-Logger STATIC
    network_logger.h
    network_logger.cpp
)


# 设置包含目录（确保主程序能访问头文件）
target_include_directories(Client-Logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
   )
#
target_link_libraries(Client-Logger PRIVATE )
