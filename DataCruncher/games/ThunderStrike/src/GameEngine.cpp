#include "GameEngine.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>

namespace game {

namespace {
    using clock = std::chrono::high_resolution_clock;

    struct Profiler {
        std::uint64_t frames{0};
        std::chrono::nanoseconds t_input{0}, t_update{0}, t_collision{0}, t_render{0};
        void report() const {
            if (frames == 0) return;
            double f = static_cast<double>(frames);
            auto avg_ms = [](auto ns){ return std::chrono::duration<double, std::milli>(ns).count(); };
            std::cout << "[Profiler] over " << frames << " frames:\n";
            std::cout << std::fixed << std::setprecision(3);
            std::cout << "  input avg:     " << (avg_ms(t_input)/f) << " ms/frame\n";
            std::cout << "  update avg:    " << (avg_ms(t_update)/f) << " ms/frame\n";
            std::cout << "  collision avg: " << (avg_ms(t_collision)/f) << " ms/frame\n";
            std::cout << "  render avg:    " << (avg_ms(t_render)/f) << " ms/frame\n";
        }
    } profiler;

    constexpr bool g_enable_profiling = true;
}


void GameEngine::run() {
    std::srand(std::time(nullptr));

    // SFML 3.0 完美的事件和主窗口生存循环
    while (window.isOpen() && is_running) {
        if (g_enable_profiling) {
            auto t0 = clock::now();
            process_input();   // 1. 现代事件输入
            profiler.t_input += std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - t0);

            t0 = clock::now();
            update_physics();  // 2. 物理更新
            profiler.t_update += std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - t0);

            t0 = clock::now();
            check_collisions(); // 3. 碰撞检测
            profiler.t_collision += std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - t0);

            t0 = clock::now();
            render_frame();    // 4. 图形渲染
            profiler.t_render += std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - t0);

            ++profiler.frames;
            if (profiler.frames % 300 == 0) profiler.report();
        } else {
            process_input();   // 1. 现代事件输入
            update_physics();  // 2. 物理更新
            check_collisions(); // 3. 碰撞检测
            render_frame();    // 4. 图形渲染
        }
    }

    window.close();
    std::cout << "=================================\n";
    std::cout << "      GAME OVER, PILOT!          \n";
    std::cout << "      YOUR FINAL SCORE: " << player.score << "\n";
    std::cout << "=================================\n";
}

// 1. 现代化的非阻塞输入响应
void GameEngine::process_input() {
    // 1. 处理窗口系统事件（如点击红叉）
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            is_running = false;
        }
    }

    // 2. 完美的 SFML 3.0 实时按键扫描（带有时钟限速）
    if (window.hasFocus()) {
        // 使用类成员 move_clock，支持游戏重置
        if (move_clock.getElapsedTime().asMilliseconds() > 80) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
                player.move_left();
                move_clock.restart(); // 移动后重置时钟
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
                player.move_right();
                move_clock.restart(); // 移动后重置时钟
            }
        }
        
        // 使用类成员 shoot_clock
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space)) {
            if (shoot_clock.getElapsedTime().asMilliseconds() > 150) { 
                bullets.emplace_back(player.x, player.y - 1);
                shoot_clock.restart();
            }
        }
    }

    // 按 Escape 或 Q 键直接退出游戏
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Escape) || 
        sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Q)) {
        is_running = false;
    }
}

// 2. 物理更新（由于 SFML 锁了 60 帧，我们可以微调速度）
void GameEngine::update_physics() {
    spawn_counter++;
    if (spawn_counter >= ENEMY_SPAWN_RATE) {
        int spawn_x = 1 + (std::rand() % (SCREEN_WIDTH - 2));
        enemies.emplace_back(spawn_x, 1);
        spawn_counter = 0;
    }

    // 利用你的 MyVector 迭代器高频向前推进
    for (auto& b : bullets) { b.update(); }
    
    // 敌机物理下落
    enemy_move_counter++;
    if (enemy_move_counter >= 15) { // 每 15 帧下落一格
        for (auto& e : enemies) { e.update(); }
        enemy_move_counter = 0;
    }

    // 边界清理
    for (size_t i = 0; i < bullets.size(); ) {
        if (bullets[i].y < 1) {
            bullets.erase(i);
        } else {
            ++i;
        }
    }

    for (size_t i = 0; i < enemies.size(); ) {
        if (enemies[i].y >= SCREEN_HEIGHT - 1) {
            player.lives--;
            enemies.erase(i);
            if (player.lives <= 0) {
                is_running = false; // 敌机突破防线
                return;
            }
        } else {
            ++i;
        }
    }
}

// 3. 碰撞检测（标记清除法）
void GameEngine::check_collisions() {
    for (size_t b_idx = 0; b_idx < bullets.size(); ++b_idx) {
        if (!bullets[b_idx].is_alive) continue;   
        for (size_t e_idx = 0; e_idx < enemies.size(); ++e_idx) {
            if (!enemies[e_idx].is_alive) continue;           
            if (bullets[b_idx].x == enemies[e_idx].x && bullets[b_idx].y == enemies[e_idx].y) {
                bullets[b_idx].is_alive = false;
                enemies[e_idx].is_alive = false;
                player.score += 10;
                break; // 子弹已经击中目标，跳出内层敌机循环
            }
        }
    }

    for (size_t i = 0; i < bullets.size(); ) {
        if (!bullets[i].is_alive) bullets.erase(i);
        else ++i;
    }

    for (size_t i = 0; i < enemies.size(); ) {
        if (!enemies[i].is_alive) enemies.erase(i);
        else ++i;
    }
}

// 4. 🚀 降维打击：真正的 2D 画面渲染
void GameEngine::render_frame() {
    // A. 用深邃的星空黑洗刷整个屏幕
    window.clear(sf::Color::Black);

    // 计算缩放比例：将原先终端的坐标（24x20）映射到 800x600 像素的窗口上
    float cell_width = 800.0f / SCREEN_WIDTH;
    float cell_height = 600.0f / SCREEN_HEIGHT;

    // B. 绘制玩家：一个帅气的绿色三角形（或者矩形）
    sf::RectangleShape player_shape(sf::Vector2f(cell_width - 4, cell_height - 4));
    player_shape.setFillColor(sf::Color::Green);
    player_shape.setPosition({player.x * cell_width, player.y * cell_height});
    window.draw(player_shape);

    // C. 绘制子弹：一颗颗红色的激光能量弹（圆形）
    for (const auto& b : bullets) {
        sf::CircleShape bullet_shape((cell_width / 4.0f)); // 半径
        bullet_shape.setFillColor(sf::Color::Red);
        // 居中一下坐标
        bullet_shape.setPosition({b.x * cell_width + cell_width/4.0f, b.y * cell_height});
        window.draw(bullet_shape);
    }

    // D. 绘制敌人：一架架迎面扑来的黄色敌机
    for (const auto& e : enemies) {
        sf::RectangleShape enemy_shape(sf::Vector2f(cell_width - 6, cell_height - 6));
        enemy_shape.setFillColor(sf::Color::Yellow);
        enemy_shape.setPosition({e.x * cell_width, e.y * cell_height});
        window.draw(enemy_shape);
    }

    // E. 最终推送到显卡显示
    window.display();
}

} // namespace game