# Util 模块的 CMake 配置

# 查找源文件
file(GLOB UTIL_SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# 添加静态库
add_library(Client-Util STATIC ${UTIL_SOURCES_CPP})
#公开目录供外界访问
target_include_directories(Client-Util PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)
# 链接库
target_link_libraries(Client-Util PRIVATE
	Qt6::Widgets Qt6::Core
)
