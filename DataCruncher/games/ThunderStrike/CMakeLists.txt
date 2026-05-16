# 1. 检索源文件
file(GLOB GAME_SOURCES "src/*.cpp")

# 2. 生成可行行文件
add_executable(ThunderStrikeGames ${GAME_SOURCES})

# 3. 指定头文件
target_include_directories(ThunderStrikeGames PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 4. 链接MyVectorLib
target_link_libraries(ThunderStrikeGames PRIVATE MyVectorLib)