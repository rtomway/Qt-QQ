# ChatMessage 模块的 CMake 配置

# 创建一个静态库或者共享库
add_library(Client-ChatMessage STATIC
  ChatMessage.h
  ChatMessage.cpp
  ChatManager.h
  ChatManager.cpp
  Message.h
  Message.cpp
)

target_include_directories(Client-ChatMessage PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Client-ChatMessage PRIVATE
	Qt6::Core Qt6::Widgets
)
