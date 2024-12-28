# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.16)

# 项目信息
project (online-judge)

set(CMAKE_CXX_STANDARD 17)

# 查找头文件的路径 cpp-httplib
include_directories(./include)
# 查找头文件的路径 mongocxx
include_directories(/usr/local/include/mongocxx/v_noabi /usr/local/include/libmongoc-1.0)
# 查找头文件的路径 bsoncxx
include_directories(/usr/local/include/bsoncxx/v_noabi /usr/local/include/libbson-1.0)
# 查找库的路径 青岛大学的安全沙箱库文件
link_directories(${CMAKE_SOURCE_DIR}/lib)
# 查找库的路径
link_directories(/usr/local/lib)
# 查找头文件的路径 redis-plus-plus
include_directories(hiredis sw)


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./src DIR_SRCS)

# 指定生成目标
add_executable(online-judge ${DIR_SRCS})

# 链接库
target_link_libraries(online-judge hiredis redis++ jsoncpp pthread mongocxx bsoncxx judger)

# 指定编译参数
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++17")
