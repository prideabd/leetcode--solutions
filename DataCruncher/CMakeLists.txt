cmake_minimum_required(VERSION 3.10)
project(DataCruncher VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =================================================================
# 1. 定义全局公共基础库 (MyVector)
# =================================================================
# 因为 MyVector 是 header-only 模板库，我们把它定义为 INTERFACE 库
add_library(MyVectorLib INTERFACE)
# 任何子项目只要 link 了 MyVectorLib，就能直接 #include "MyVector.hpp"
target_include_directories(MyVectorLib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core_libs/MyVector/include>
)

# =================================================================
# 2. 调度子目录（告诉 CMake 去哪里找真正的业务项目）
# =================================================================
add_subdirectory(cruncher_engine)
add_subdirectory(games/ThunderStrike)





