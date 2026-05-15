cmake_minimum_required(VERSION 3.10)
project(DataCruncher VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 寻找所有的源文件（包含 src 目录下的 main.cpp 等）
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 2. 定义你的接口库 (Header-only Library)
# 因为模板类都在 .hpp 中，这种库在 CMake 中被称为 INTERFACE 库
add_library(DataCruncherLib INTERFACE)

# 3. 核心：配置头文件搜索路径
# 这里告诉 CMake，任何链接了 DataCruncherLib 的目标都会自动获得 include 文件夹作为搜索根目录。
# BUILD_INTERFACE (开发模式)：CMake 会去 ${CMAKE_CURRENT_SOURCE_DIR}/include 找头文件。
# INSTALL_INTERFACE：去安装目录下的 include 文件夹找
target_include_directories(DataCruncherLib INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 4. 生成可执行文件
add_executable(DataApp ${SOURCES})

# 5. 关键的一步：将可执行文件与你的库“绑定”
# 这样 DataApp 就能找到 <DataCruncher/DataCruncher.hpp> 了
target_link_libraries(DataApp PRIVATE DataCruncherLib)



