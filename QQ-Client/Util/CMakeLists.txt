
# 查找源文件
file(GLOB UTIL_SOURCES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 添加静态库
add_library(Util STATIC ${UTIL_SOURCES_CPP})

target_include_directories(Util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 链接库
target_link_libraries(Util PRIVATE Qt6::Widgets Qt6::Core)
