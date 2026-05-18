#pragma once

// 战场地图
namespace game {
    // 游戏地图大小
    constexpr int SCREEN_WIDTH = 25;
    constexpr int SCREEN_HEIGHT = 20;

    // 游戏帧率控制
    constexpr int FRAME_DELAY_MS = 40;

    // 敌机生成概率(数字越小，敌人越多)
    constexpr int ENEMY_SPAWN_RATE = 15;
}