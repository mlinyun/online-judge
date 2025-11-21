# CMake 最低版本要求
cmake_minimum_required(VERSION 4.0)

# 设置项目名称、版本和编程语言
project(online-judge-backend VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++17 standard to be used")

# 设置源文件目录
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# 设置可执行文件输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 查找源文件目录下的所有源文件，并将其存储在 SOURCES 变量中
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")

# 查找 httplib 库
find_package(httplib REQUIRED)
# 查找 JsonCpp 库
find_package(jsoncpp REQUIRED)
# 查找 MongoDB C++ 驱动程序
find_package(mongocxx REQUIRED)
# 查找 Redis C++ 驱动程序
find_package(redis++ REQUIRED)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接依赖库（PRIVATE 表示这些依赖只在当前目标中使用）
target_link_libraries(${PROJECT_NAME} PRIVATE httplib::httplib JsonCpp::JsonCpp mongo::mongocxx_static redis++::redis++_static)