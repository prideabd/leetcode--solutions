cmake_minimum_required(VERSION 3.16)
project(ThunderStrike)

# 1. 检索源文件
set(GAME_SOURCES
    "src/main.cpp"
    "src/GameEngine.cpp")

find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)
# 2. 生成可行行文件
add_executable(ThunderStrikeGames ${GAME_SOURCES})

# 3. 指定头文件
target_include_directories(ThunderStrikeGames PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 4. 链接MyVectorLib
target_link_libraries(ThunderStrikeGames PRIVATE MyVectorLib)
target_link_libraries(ThunderStrikeGames PRIVATE SFML::Graphics SFML::Window SFML::System)