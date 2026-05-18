#pragma once

#include <SFML/Graphics.hpp> //  引入 SFML 全家桶
#include "MyVector.hpp" 
#include "Config.hpp"
#include "Entity.hpp"

namespace game {

class GameEngine {
private:
    // 📺 SFML 渲染窗口对象（代替了原来的终端屏幕）
    sf::RenderWindow window;
    
    Player player;
    dc::MyVector<Bullet> bullets;
    dc::MyVector<Enemy>  enemies;
    
    bool is_running;

    // 🎮 游戏逻辑计时器与计数器
    sf::Clock move_clock;      // 玩家移动限制
    sf::Clock shoot_clock;     // 射击 CD
    int spawn_counter;         // 敌人生成计数
    int enemy_move_counter;    // 敌人移动节奏控制

public:
    // 初始化一个 800x600 像素、标题为 "ThunderStrike" 的窗口
    GameEngine() : 
        window(sf::VideoMode(sf::Vector2u{800, 600}), "ThunderStrike"), 
        is_running(true),
        spawn_counter(0),
        enemy_move_counter(0) 
    {
        window.setFramerateLimit(60);
    }
    
    ~GameEngine() = default;

    void run();

private:
    // 依然是那经典的五大模块，但内部实现将迎来大换血！
    void process_input();   
    void update_physics();  
    void check_collisions(); 
    void render_frame();    
};

} // namespace game
