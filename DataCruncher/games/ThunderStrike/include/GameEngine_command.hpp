// // 无界面，已弃用
// #pragma once

// #include "MyVector.hpp"
// #include "Config.hpp"
// #include "Entity.hpp"

// namespace game {

// class GameEngine {
// private:
//     Player player;

//     // 采用MyVector管理子弹以及敌人对象
//     dc::MyVector<Bullet> bullets;
//     dc::MyVector<Enemy> enemies;

//     bool is_running;

// public:
//     GameEngine() : is_running(true) {}
//     ~GameEngine() = default;

//     void run();

// private:
//     void process_input();    // 获取输入
//     void update_physics();   // 物理状态更新
//     void check_collisions(); // 碰撞检测
//     void render_frame();     // 渲染画面
//     void control_fps();      // 帧率控制

//     int init_keyboard();
//     void close_keyboard();
//     bool kbhit();            // 检查键盘按下
//     char getch();            // 无回车直接读取字符
//     void clear_screen();     // 清屏
// };
// }