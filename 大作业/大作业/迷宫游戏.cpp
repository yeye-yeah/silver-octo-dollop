#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> 

// 常量定义
#define GRID_SIZE 20        // 每个格子的像素大小
#define MAZE_WIDTH 20       // 迷宫宽度（格子数）
#define MAZE_HEIGHT 20      // 迷宫高度（格子数）
#define WINDOW_WIDTH (GRID_SIZE * MAZE_WIDTH)  // 窗口宽度
#define WINDOW_HEIGHT (GRID_SIZE * MAZE_HEIGHT + 70)  // 窗口高度（含状态栏）

// 方向枚举
enum Direction { UP, DOWN, LEFT, RIGHT };

// 坐标结构体
struct Point {
    int x;  // 像素坐标x
    int y;  // 像素坐标y
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

// 迷宫类
class Maze {
private:
    int map[MAZE_HEIGHT][MAZE_WIDTH];  // 0-路径 1-墙壁
    Point exitPos;                     // 出口坐标（像素）

public:
    Maze() {
        // 初始化迷宫（全为墙壁）
        for (int i = 0; i < MAZE_HEIGHT; i++)
            for (int j = 0; j < MAZE_WIDTH; j++)
                map[i][j] = 1;

        generateMaze();  // 生成迷宫路径
        setExitPos();    // 设置出口位置
    }

    // 生成迷宫（递归回溯法）
    void generateMaze() {
        srand((unsigned)time(NULL));
        // 从起点(1,1)开始生成（确保为偶数坐标，便于挖路）
        dfs(1, 1);

        // 确保起点是通路
        map[0][0] = 0;
        map[0][1] = 0;
        map[1][0] = 0;

        // 确保出口位置是通路 (18,18)
        map[18][18] = 0;
        map[18][17] = 0;
        map[17][18] = 0;
    }

    // 深度优先搜索挖路
    void dfs(int x, int y) {
        // 标记当前位置为路径
        map[x][y] = 0;

        // 随机方向数组（上、右、下、左）
        int dirs[4][2] = { {-1,0}, {0,1}, {1,0}, {0,-1} };

        // 使用Fisher-Yates洗牌算法
        for (int i = 3; i > 0; i--) {
            int r = rand() % (i + 1);
            int temp0 = dirs[i][0];
            int temp1 = dirs[i][1];
            dirs[i][0] = dirs[r][0];
            dirs[i][1] = dirs[r][1];
            dirs[r][0] = temp0;
            dirs[r][1] = temp1;
        }

        // 尝试四个方向
        for (int i = 0; i < 4; i++) {
            int nx = x + dirs[i][0] * 2;  // 跳过一格挖路
            int ny = y + dirs[i][1] * 2;

            // 判断是否在边界内且未挖过
            if (nx > 0 && nx < MAZE_HEIGHT - 1 && ny > 0 && ny < MAZE_WIDTH - 1 && map[nx][ny] == 1) {
                // 打通中间的墙
                map[x + dirs[i][0]][y + dirs[i][1]] = 0;
                // 递归挖下一格
                dfs(nx, ny);
            }
        }
    }

    // 设置出口位置（18,18）
    void setExitPos() {
        // 出口固定在(18,18)
        exitPos.x = 18 * GRID_SIZE + GRID_SIZE / 2;
        exitPos.y = 18 * GRID_SIZE + GRID_SIZE / 2;
    }

    // 绘制迷宫
    void draw() {
        // 绘制路径背景
        setfillcolor(LIGHTGRAY);
        solidrectangle(0, 0, WINDOW_WIDTH, GRID_SIZE * MAZE_HEIGHT);

        // 绘制墙壁
        setfillcolor(BLUE);  // 使用标准蓝色
        for (int i = 0; i < MAZE_HEIGHT; i++) {
            for (int j = 0; j < MAZE_WIDTH; j++) {
                if (map[i][j] == 1) {  // 1表示墙壁
                    int x = j * GRID_SIZE;
                    int y = i * GRID_SIZE;
                    solidrectangle(x, y, x + GRID_SIZE, y + GRID_SIZE);
                }
            }
        }

        // 绘制出口（绿色圆形）
        setfillcolor(GREEN);
        solidcircle(exitPos.x, exitPos.y, GRID_SIZE / 2);

        // 绘制起点标记（红色方块）
        setfillcolor(RED);
        solidrectangle(0, 0, GRID_SIZE, GRID_SIZE);

        // 绘制终点标记（黄色方块）在(18,18)
        setfillcolor(YELLOW);
        solidrectangle(18 * GRID_SIZE,
            18 * GRID_SIZE,
            19 * GRID_SIZE,
            19 * GRID_SIZE);
    }

    // 碰撞检测（判断坐标是否为墙壁）
    bool isWall(int x, int y) {
        // 转换为网格坐标
        int gridX = x / GRID_SIZE;  // x坐标对应列
        int gridY = y / GRID_SIZE;  // y坐标对应行

        // 判断是否超出边界或碰到墙壁
        if (gridY < 0 || gridY >= MAZE_HEIGHT || gridX < 0 || gridX >= MAZE_WIDTH)
            return true;
        return map[gridY][gridX] == 1;
    }

    // 获取出口坐标
    Point getExitPos() { return exitPos; }

    // 获取出口网格坐标
    Point getExitGridPos() {
        return Point(18, 18);
    }
};

// 角色类
class Player {
private:
    Point pos;    // 当前位置
    int stepCount; // 步数统计
    DWORD lastMoveTime; // 上次移动时间

public:
    Player() {
        // 起点位置（左上角网格中心）
        pos.x = GRID_SIZE / 2;
        pos.y = GRID_SIZE / 2;
        stepCount = 0;
        lastMoveTime = GetTickCount();
    }

    // 移动角色
    void move(Direction dir, Maze& maze) {
        // 添加移动间隔控制（200ms）
        DWORD currentTime = GetTickCount();
        if (currentTime - lastMoveTime < 200) {
            return;
        }
        lastMoveTime = currentTime;

        Point oldPos = pos;

        // 根据方向移动（每次移动一个网格的距离）
        switch (dir) {
        case UP:    pos.y -= GRID_SIZE; break;
        case DOWN:  pos.y += GRID_SIZE; break;
        case LEFT:  pos.x -= GRID_SIZE; break;
        case RIGHT: pos.x += GRID_SIZE; break;
        }

        // 碰撞检测：如果碰到墙壁则恢复原位置
        if (maze.isWall(pos.x, pos.y)) {
            pos = oldPos;
        }
        else {
            // 移动有效，步数+1
            stepCount++;
        }
    }

    // 绘制角色（紫色圆形）
    void draw() {
        setfillcolor(MAGENTA); // 使用紫色，与出口区分
        solidcircle(pos.x, pos.y, GRID_SIZE / 2 - 2); // 预留边框
    }

    // 检测是否到达出口
    bool reachExit(Maze& maze) {
        Point exit = maze.getExitPos();
        // 计算角色与出口的距离
        int dx = pos.x - exit.x;
        int dy = pos.y - exit.y;
        return (dx * dx + dy * dy) < (GRID_SIZE * GRID_SIZE);
    }

    // 获取步数
    int getStepCount() { return stepCount; }

    // 获取当前位置
    Point getPosition() { return pos; }

    // 获取网格位置
    Point getGridPosition() {
        return Point(pos.x / GRID_SIZE, pos.y / GRID_SIZE);
    }

    // 重置位置和步数
    void reset() {
        pos.x = GRID_SIZE / 2;
        pos.y = GRID_SIZE / 2;
        stepCount = 0;
        lastMoveTime = GetTickCount();
    }
};

// 游戏类
class Game {
private:
    Maze maze;
    Player player;
    bool isRunning;
    bool gameWon;

public:
    Game() {
        isRunning = true;
        gameWon = false;
    }

    // 处理键盘输入
    void handleInput() {
        // 检测方向键
        if ((GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState('W') & 0x8000)) {
            player.move(UP, maze);
        }
        else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState('S') & 0x8000)) {
            player.move(DOWN, maze);
        }
        else if ((GetAsyncKeyState(VK_LEFT) & 0x8000) || (GetAsyncKeyState('A') & 0x8000)) {
            player.move(LEFT, maze);
        }
        else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState('D') & 0x8000)) {
            player.move(RIGHT, maze);
        }

        // 检测功能键
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            isRunning = false;
        }
        if (GetAsyncKeyState('R') & 0x8000) {
            restart();
            // 避免连续触发
            Sleep(200);
        }
    }

    // 重新开始游戏
    void restart() {
        maze = Maze();
        player.reset();
        gameWon = false;
    }

    // 显示步数和位置信息
    void showSteps() {
        // 绘制状态栏背景
        setfillcolor(LIGHTGRAY);
        solidrectangle(0, GRID_SIZE * MAZE_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);

        // 显示步数
        settextcolor(BLACK);
        settextstyle(16, 0, _T("宋体"));

        // 使用宽字符格式
        wchar_t text[200];
        Point gridPos = player.getGridPosition();
        Point exitGridPos = maze.getExitGridPos();

        swprintf_s(text, L"步数: %d  位置: (%d, %d)  出口: (%d, %d)",
            player.getStepCount(),
            gridPos.x, gridPos.y,
            exitGridPos.x, exitGridPos.y);

        // 计算文本垂直位置
        int textY = GRID_SIZE * MAZE_HEIGHT + 10;
        outtextxy(10, textY, text);

        // 添加操作提示
        swprintf_s(text, L"方向键/WASD移动 | R键重新开始 | ESC退出");
        outtextxy(10, textY + 20, text);

        // 添加目标提示
        swprintf_s(text, L"目标: 从(0,0)移动到(18,18)");
        outtextxy(10, textY + 40, text);
    }

    // 运行游戏主循环
    void run() {
        // 初始化图形窗口
        initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
        setbkcolor(WHITE);

        // 游戏主循环
        while (isRunning) {
            // 双缓冲绘图开始
            BeginBatchDraw();
            cleardevice();

            // 绘制游戏元素
            maze.draw();
            player.draw();
            showSteps();

            // 处理输入
            handleInput();

            // 检测是否通关
            if (!gameWon && player.reachExit(maze)) {
                gameWon = true;

                // 显示通关信息
                settextcolor(RED);
                settextstyle(24, 0, _T("宋体"));
                wchar_t winText[100];
                swprintf_s(winText, L"恭喜通关！步数: %d", player.getStepCount());
                outtextxy(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 50, winText);
                swprintf_s(winText, L"按R键重新开始游戏");
                outtextxy(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2, winText);

                // 刷新显示
                EndBatchDraw();
                FlushBatchDraw();

                // 等待用户操作
                while (gameWon && isRunning) {
                    if (GetAsyncKeyState('R') & 0x8000) {
                        restart();
                        break;
                    }
                    else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                        isRunning = false;
                        break;
                    }
                    Sleep(50);
                }
                continue; // 跳过本次循环剩余部分
            }

            // 双缓冲绘图结束
            EndBatchDraw();
            Sleep(50); // 控制帧率
        }

        // 关闭图形窗口
        closegraph();
    }
};

// 主函数
int main() {
    // 解决中文乱码问题
    SetConsoleOutputCP(65001);

    Game game;
    game.run();
    return 0;
}