// // 同样已弃用
// #include <iostream>
// #include <chrono>
// #include <thread>
// #include <cstdlib>
// #include <ctime>
// #include <unistd.h>
// #include <termios.h>
// #include <fcntl.h>
// #include "GameEngine_command.hpp"

// namespace game {

// // 非阻塞输入以及清屏实现
// // 总的来说，就是原本是只有当按下"enter"时，程序才会接受到输入（规范模式）
// // 对于该游戏来说，这是不对，因此需要改变
// // 初始化
// int GameEngine::init_keyboard() {
//     struct termios topts; // 创建一个termios结构体，存放终端配置信息
//     tcgetattr(STDIN_FILENO, &topts); // 获取当前信息
   
//     // ICANON：规范模式（必须输回车的罪魁祸首）
//     // ECHO：回显模式（在屏幕上画出你敲的字）
//     topts.c_lflag &= ~(ICANON | ECHO); // 关闭“规范模式”和回显

//     // TCSANOW：Terminal Control Set Actions Now，即刻生效配置。
//     tcsetattr(STDIN_FILENO, TCSANOW, &topts); // 让新配置生效
//     return 0;
// }

// void GameEngine::close_keyboard() {
//     struct termios topts;
//     tcgetattr(STDIN_FILENO, &topts);
//     topts.c_lflag |= (ICANON | ECHO);
//     tcsetattr(STDIN_FILENO, TCSANOW, &topts);
// }

// // 检查有没有按键
// bool GameEngine::kbhit() {
//     struct termios oldt, newt;
//     int ch;
//     int oldf;

//     // 1. 获取并修改终端属性（关闭缓冲区）
//     tcgetattr(STDIN_FILENO, &oldt);
//     newt = oldt;
//     newt.c_lflag &= ~(ICANON | ECHO);
//     tcsetattr(STDIN_FILENO, TCSANOW, &newt);

//     // 2. 【核心】精准获取状态 (F_GETFL)，绝对不能写成 F_SETFL！
//     oldf = fcntl(STDIN_FILENO, F_GETFL, 0); 
    
//     // 3. 注入非阻塞灵魂
//     fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

//     // 4. 尝试捞取字符（在非阻塞模式下，没按键会瞬间返回 EOF，绝对不卡死）
//     ch = getchar();

//     // 5. 瞬间恢复原状
//     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
//     fcntl(STDIN_FILENO, F_SETFL, oldf);

//     // 6. 判定雷达结果
//     if (ch != EOF) {
//         ungetc(ch, stdin); // 把字符放回去给 getch() 享用
//         return true;       // 报告长官：有人按键！
//     }
    
//     return false; // 报告长官：无人按键，悄悄溜走，绝不卡死！
// }

// char GameEngine::getch() {
//     return getchar();
// }

// void GameEngine::clear_screen() {
//     // \033：代表 ESC 键。这是告诉终端：“注意，后面的字符不是用来打印的，而是给你的控制命令！”
//     // [2J：清除整个屏幕（J 代表 Clear Screen，2 代表全屏）。
//     // [1;1H：将光标瞬间移动到左上角第 1 行、第 1 列（H 代表 Cursor Position）
//     std::cout << "\033[2J\033[1;1H";
// }

// // =================================================================
// // 游戏核心五大模块主循环
// // =================================================================
// void GameEngine::run() {
//     std::srand(std::time(nullptr)); // 初始化随机
//     init_keyboard();

//     bool total_exit = false;
//     while (!total_exit) {
//         player.reset();
//         bullets.clear();
//         enemies.clear();
//         is_running = true;

//         while (is_running) {
//             process_input();   // 输入
//             update_physics();  // 更新状态
//             check_collisions();// 检查碰撞
//             render_frame();    // 渲染界面
//             control_fps();
//         }

//         // 游戏不幸结束，进入“重启/退出”死循环等待面 ---
//         clear_screen();
//         while (true) {
//             clear_screen();
//             std::cout << "=================================\n";
//             std::cout << "      GAME OVER, PILOT!          \n";
//             std::cout << "      YOUR FINAL SCORE: " << player.score << " \n";
//             std::cout << "=================================\n\n";
//             std::cout << "  [R] Restart Game   [Q] Quit to Terminal\n";
//             std::cout << "=================================\n";
            
//             std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
//             if (kbhit()) {
//                 char choice = getch();
//                 if (choice == 'r' || choice == 'R') {
//                     break; // 跳出菜单死循环，外层的 while (!total_exit) 会自动重新开始游戏！
//                 }
//                 if (choice == 'q' || choice == 'Q') {
//                     total_exit = true; // 彻底想要离开
//                     break;
//                 }
//             }
//         }
//     }

//     close_keyboard();
//     clear_screen();
//     std::cout << "=================================\n";
//     std::cout << "      GAME OVER, PILOT!          \n";
//     std::cout << "      YOUR SCORE: " << player.score << " \n";
//     std::cout << "=================================\n";
// }

// // 1. 输入
// void GameEngine::process_input() {
//     // 获取键盘输入
//     if (kbhit()) {
//         char key = getch();
//         if (key == 'a' || key == 'A') {
//             player.move_left();
//         }
//         if (key == 'd' || key == 'D') {
//             player.move_right();
//         }
//         if (key == ' ') {
//             bullets.emplace_back(player.x, player.y - 1);
//         }
//         if (key == 'q' || key == 'Q') {
//             is_running = false;
//         }
//     }
// }

// // 2. 更新状态（敌机，子弹等）
// void GameEngine::update_physics() {
//     static int spawn_counter = 0;
//     static int enemy_move_counter = 0;
//     // 固定时间步长生成敌人
//     spawn_counter++;
//     if (spawn_counter >= ENEMY_SPAWN_RATE) {
//         int spawn_x = 1 + (std::rand() % (SCREEN_WIDTH - 2));
//         enemies.emplace_back(spawn_x, 1);
//         spawn_counter = 0;
//     }
//     // 测试点：利用MyVector的 begin()/end() 指针迭代器进行超高效物理遍历
//     for (auto& b : bullets) {
//         b.update();
//     }
//     // 敌人每三帧后向下移动一格
//     enemy_move_counter++;
//     if (enemy_move_counter >= 10) {
//         for (auto& e : enemies) {
//             e.update();
//         }
//         enemy_move_counter = 0;
//     }

//     // 清理越出边界的子弹
//     for (size_t i = 0; i < bullets.size();) {
//         if (bullets[i].y < 1) {
//             bullets.erase(i);
//         } else {
//             ++i;
//         }
//     }

//     // 检查敌机是否突破防线
//     for (size_t i = 0; i < enemies.size();) {
//         if (enemies[i].y >= SCREEN_HEIGHT - 1) {
//             player.lives--;
//             enemies.erase(i);
//             if (player.lives <= 0) {
//                 is_running = false;
//                 return;
//             }
//         } else {
//             ++i;
//         }
//     }
// }

// // 3. 碰撞检测
// void GameEngine::check_collisions() {
//     // 碰撞检测
//     for (size_t e_idx = 0; e_idx < enemies.size(); ++e_idx) {
//         for (size_t b_idx = 0; b_idx < bullets.size(); ++b_idx) {
//             if (enemies[e_idx].x == bullets[b_idx].x && enemies[e_idx].y == bullets[b_idx].y) {
//                 enemies[e_idx].is_alive = false;
//                 bullets[b_idx].is_alive = false;
//                 player.score += 10;
//             }
//         }
//     }
//     // 清理死去的对象
//     for (size_t i = 0; i < bullets.size();) {
//         if (!bullets[i].is_alive) {
//             bullets.erase(i);
//         } else {
//             ++i;
//         }
//     }
//     for (size_t i = 0; i < enemies.size();) {
//         if (!enemies[i].is_alive) {
//             enemies.erase(i);
//         } else {
//             ++i;
//         }
//     }

// }

// // 4. 渲染界面
// void GameEngine::render_frame() {
//     clear_screen();

//     // 画一个区域，周围用#
//     char board[SCREEN_HEIGHT][SCREEN_WIDTH];
//     for (int y = 0; y < SCREEN_HEIGHT; ++y) {
//         for (int x = 0; x < SCREEN_WIDTH; ++x) {
//             if (y == 0 || y == SCREEN_HEIGHT - 1 || x == 0 || x == SCREEN_WIDTH - 1) {
//                 board[y][x] = '#'; // 画围墙
//             } else {
//                 board[y][x] = ' '; // 画夜空
//             }
//         }
//     }

//     // 建立玩家
//     board[player.y][player.x] = player.icon;

//     // 建立子弹和敌人
//     for (const auto& b : bullets) {
//         if (b.y > 0 && b.y < SCREEN_HEIGHT - 1) {
//             board[b.y][b.x] = b.icon;
//         } 
//     }
//     for (const auto& e : enemies) {
//         if (e.y > 0 && e.y < SCREEN_HEIGHT - 1) {
//             board[e.y][e.x] = e.icon;
//         } 
//     }

//     // 将board输入到终端
//     std::string output = "";
//     for (int y = 0; y < SCREEN_HEIGHT; ++y) {
//         for (int x = 0; x < SCREEN_WIDTH; ++x) {
//             output += board[y][x];
//         }
//         output += "\n";
//     }
//     std::cout << output;
//     std::cout << "SCORE: " << player.score << "  LIVES: " << player.lives << "  [A/D: Move] [Space: Fire] [Q: Quit]\n";
// }

// // 帧率控制
// void GameEngine::control_fps() {
//     std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY_MS));
// }

// } // namespace game