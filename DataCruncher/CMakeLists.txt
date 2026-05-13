cmake_minimum_required(VERSION 3.10)
project(DataCruncher VERSION 0.1)

set (CMAKE_CXX_STANDARD 17)

# 1. 定义头文件目录（告诉编译器哪里找.h)
include_directories(include)

# 2. 创建核心库
# 将算法编译成名为CruncherCore的静态库
add_library(CruncherCore src/calc.cpp)

# 3. 创建可执行文件
add_executable(DataApp src/main.cpp)

# 4. 链接
# 告诉系统，DataApp运行依赖CruncherCore库
target_link_libraries(DataApp CruncherCore)


