#pragma once

#include <string>
#include <iostream>
#include "Config.hpp"

namespace game {
    
// 所有对象基类
struct Entity {
    int x, y;
    char icon;
    bool is_alive;

    Entity(int start_x, int start_y, char c)
        : x(start_x), y(start_y), icon(c), is_alive(true) {}

    // Rule of Five: Ensure safe movement within MyVector during reallocations
    Entity(const Entity&) = default;
    Entity(Entity&&) noexcept = default;
    Entity& operator=(const Entity&) = default;
    Entity& operator=(Entity&&) noexcept = default;

    virtual ~Entity() = default;
};

// 1. 子弹类
struct Bullet : public Entity {
    // 子弹目前只会向上飞行
    Bullet(int start_x, int start_y) : Entity(start_x, start_y, '^') {
        // 游戏高频生成，我们可以看它是不是真的触发了 emplace_back 的原地构造
        // std::cout << "[Bullet] 孵化于 (" << x << "," << y << ")\n";
    }
    ~Bullet() {
        // std::cout << "[Bullet] 彻底销毁\n";
    }
    void update() {
        y--;
    }
};

// 2. 敌机类
struct Enemy : public Entity {
    Enemy(int start_x, int start_y) : Entity(start_x, start_y, 'V') {}

    void update() {
        y++; // 敌机向下沉
    }
};

// 3. 玩家战机类
struct Player : public Entity {
    int score;
    int lives;

    // 将 lives 调整为更合理的数值，节省逻辑判断压力
    Player() : Entity(12, 18, 'A'), score(0), lives(3) {} 

    void reset() {
        x = 12;
        y = 18;
        score = 0;
        lives = 3;
    }

    void move_left() {
        if (x > 1) x--;
    }

    void move_right() {
        if (x < SCREEN_WIDTH - 2) x++;
    }
};

} // namespace game
