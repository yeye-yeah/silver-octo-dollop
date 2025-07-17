#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> 

// ��������
#define GRID_SIZE 20        // ÿ�����ӵ����ش�С
#define MAZE_WIDTH 20       // �Թ���ȣ���������
#define MAZE_HEIGHT 20      // �Թ��߶ȣ���������
#define WINDOW_WIDTH (GRID_SIZE * MAZE_WIDTH)  // ���ڿ��
#define WINDOW_HEIGHT (GRID_SIZE * MAZE_HEIGHT + 70)  // ���ڸ߶ȣ���״̬����

// ����ö��
enum Direction { UP, DOWN, LEFT, RIGHT };

// ����ṹ��
struct Point {
    int x;  // ��������x
    int y;  // ��������y
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

// �Թ���
class Maze {
private:
    int map[MAZE_HEIGHT][MAZE_WIDTH];  // 0-·�� 1-ǽ��
    Point exitPos;                     // �������꣨���أ�

public:
    Maze() {
        // ��ʼ���Թ���ȫΪǽ�ڣ�
        for (int i = 0; i < MAZE_HEIGHT; i++)
            for (int j = 0; j < MAZE_WIDTH; j++)
                map[i][j] = 1;

        generateMaze();  // �����Թ�·��
        setExitPos();    // ���ó���λ��
    }

    // �����Թ����ݹ���ݷ���
    void generateMaze() {
        srand((unsigned)time(NULL));
        // �����(1,1)��ʼ���ɣ�ȷ��Ϊż�����꣬������·��
        dfs(1, 1);

        // ȷ�������ͨ·
        map[0][0] = 0;
        map[0][1] = 0;
        map[1][0] = 0;

        // ȷ������λ����ͨ· (18,18)
        map[18][18] = 0;
        map[18][17] = 0;
        map[17][18] = 0;
    }

    // �������������·
    void dfs(int x, int y) {
        // ��ǵ�ǰλ��Ϊ·��
        map[x][y] = 0;

        // ����������飨�ϡ��ҡ��¡���
        int dirs[4][2] = { {-1,0}, {0,1}, {1,0}, {0,-1} };

        // ʹ��Fisher-Yatesϴ���㷨
        for (int i = 3; i > 0; i--) {
            int r = rand() % (i + 1);
            int temp0 = dirs[i][0];
            int temp1 = dirs[i][1];
            dirs[i][0] = dirs[r][0];
            dirs[i][1] = dirs[r][1];
            dirs[r][0] = temp0;
            dirs[r][1] = temp1;
        }

        // �����ĸ�����
        for (int i = 0; i < 4; i++) {
            int nx = x + dirs[i][0] * 2;  // ����һ����·
            int ny = y + dirs[i][1] * 2;

            // �ж��Ƿ��ڱ߽�����δ�ڹ�
            if (nx > 0 && nx < MAZE_HEIGHT - 1 && ny > 0 && ny < MAZE_WIDTH - 1 && map[nx][ny] == 1) {
                // ��ͨ�м��ǽ
                map[x + dirs[i][0]][y + dirs[i][1]] = 0;
                // �ݹ�����һ��
                dfs(nx, ny);
            }
        }
    }

    // ���ó���λ�ã�18,18��
    void setExitPos() {
        // ���ڹ̶���(18,18)
        exitPos.x = 18 * GRID_SIZE + GRID_SIZE / 2;
        exitPos.y = 18 * GRID_SIZE + GRID_SIZE / 2;
    }

    // �����Թ�
    void draw() {
        // ����·������
        setfillcolor(LIGHTGRAY);
        solidrectangle(0, 0, WINDOW_WIDTH, GRID_SIZE * MAZE_HEIGHT);

        // ����ǽ��
        setfillcolor(BLUE);  // ʹ�ñ�׼��ɫ
        for (int i = 0; i < MAZE_HEIGHT; i++) {
            for (int j = 0; j < MAZE_WIDTH; j++) {
                if (map[i][j] == 1) {  // 1��ʾǽ��
                    int x = j * GRID_SIZE;
                    int y = i * GRID_SIZE;
                    solidrectangle(x, y, x + GRID_SIZE, y + GRID_SIZE);
                }
            }
        }

        // ���Ƴ��ڣ���ɫԲ�Σ�
        setfillcolor(GREEN);
        solidcircle(exitPos.x, exitPos.y, GRID_SIZE / 2);

        // ��������ǣ���ɫ���飩
        setfillcolor(RED);
        solidrectangle(0, 0, GRID_SIZE, GRID_SIZE);

        // �����յ��ǣ���ɫ���飩��(18,18)
        setfillcolor(YELLOW);
        solidrectangle(18 * GRID_SIZE,
            18 * GRID_SIZE,
            19 * GRID_SIZE,
            19 * GRID_SIZE);
    }

    // ��ײ��⣨�ж������Ƿ�Ϊǽ�ڣ�
    bool isWall(int x, int y) {
        // ת��Ϊ��������
        int gridX = x / GRID_SIZE;  // x�����Ӧ��
        int gridY = y / GRID_SIZE;  // y�����Ӧ��

        // �ж��Ƿ񳬳��߽������ǽ��
        if (gridY < 0 || gridY >= MAZE_HEIGHT || gridX < 0 || gridX >= MAZE_WIDTH)
            return true;
        return map[gridY][gridX] == 1;
    }

    // ��ȡ��������
    Point getExitPos() { return exitPos; }

    // ��ȡ������������
    Point getExitGridPos() {
        return Point(18, 18);
    }
};

// ��ɫ��
class Player {
private:
    Point pos;    // ��ǰλ��
    int stepCount; // ����ͳ��
    DWORD lastMoveTime; // �ϴ��ƶ�ʱ��

public:
    Player() {
        // ���λ�ã����Ͻ��������ģ�
        pos.x = GRID_SIZE / 2;
        pos.y = GRID_SIZE / 2;
        stepCount = 0;
        lastMoveTime = GetTickCount();
    }

    // �ƶ���ɫ
    void move(Direction dir, Maze& maze) {
        // ����ƶ�������ƣ�200ms��
        DWORD currentTime = GetTickCount();
        if (currentTime - lastMoveTime < 200) {
            return;
        }
        lastMoveTime = currentTime;

        Point oldPos = pos;

        // ���ݷ����ƶ���ÿ���ƶ�һ������ľ��룩
        switch (dir) {
        case UP:    pos.y -= GRID_SIZE; break;
        case DOWN:  pos.y += GRID_SIZE; break;
        case LEFT:  pos.x -= GRID_SIZE; break;
        case RIGHT: pos.x += GRID_SIZE; break;
        }

        // ��ײ��⣺�������ǽ����ָ�ԭλ��
        if (maze.isWall(pos.x, pos.y)) {
            pos = oldPos;
        }
        else {
            // �ƶ���Ч������+1
            stepCount++;
        }
    }

    // ���ƽ�ɫ����ɫԲ�Σ�
    void draw() {
        setfillcolor(MAGENTA); // ʹ����ɫ�����������
        solidcircle(pos.x, pos.y, GRID_SIZE / 2 - 2); // Ԥ���߿�
    }

    // ����Ƿ񵽴����
    bool reachExit(Maze& maze) {
        Point exit = maze.getExitPos();
        // �����ɫ����ڵľ���
        int dx = pos.x - exit.x;
        int dy = pos.y - exit.y;
        return (dx * dx + dy * dy) < (GRID_SIZE * GRID_SIZE);
    }

    // ��ȡ����
    int getStepCount() { return stepCount; }

    // ��ȡ��ǰλ��
    Point getPosition() { return pos; }

    // ��ȡ����λ��
    Point getGridPosition() {
        return Point(pos.x / GRID_SIZE, pos.y / GRID_SIZE);
    }

    // ����λ�úͲ���
    void reset() {
        pos.x = GRID_SIZE / 2;
        pos.y = GRID_SIZE / 2;
        stepCount = 0;
        lastMoveTime = GetTickCount();
    }
};

// ��Ϸ��
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

    // �����������
    void handleInput() {
        // ��ⷽ���
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

        // ��⹦�ܼ�
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            isRunning = false;
        }
        if (GetAsyncKeyState('R') & 0x8000) {
            restart();
            // ������������
            Sleep(200);
        }
    }

    // ���¿�ʼ��Ϸ
    void restart() {
        maze = Maze();
        player.reset();
        gameWon = false;
    }

    // ��ʾ������λ����Ϣ
    void showSteps() {
        // ����״̬������
        setfillcolor(LIGHTGRAY);
        solidrectangle(0, GRID_SIZE * MAZE_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);

        // ��ʾ����
        settextcolor(BLACK);
        settextstyle(16, 0, _T("����"));

        // ʹ�ÿ��ַ���ʽ
        wchar_t text[200];
        Point gridPos = player.getGridPosition();
        Point exitGridPos = maze.getExitGridPos();

        swprintf_s(text, L"����: %d  λ��: (%d, %d)  ����: (%d, %d)",
            player.getStepCount(),
            gridPos.x, gridPos.y,
            exitGridPos.x, exitGridPos.y);

        // �����ı���ֱλ��
        int textY = GRID_SIZE * MAZE_HEIGHT + 10;
        outtextxy(10, textY, text);

        // ��Ӳ�����ʾ
        swprintf_s(text, L"�����/WASD�ƶ� | R�����¿�ʼ | ESC�˳�");
        outtextxy(10, textY + 20, text);

        // ���Ŀ����ʾ
        swprintf_s(text, L"Ŀ��: ��(0,0)�ƶ���(18,18)");
        outtextxy(10, textY + 40, text);
    }

    // ������Ϸ��ѭ��
    void run() {
        // ��ʼ��ͼ�δ���
        initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
        setbkcolor(WHITE);

        // ��Ϸ��ѭ��
        while (isRunning) {
            // ˫�����ͼ��ʼ
            BeginBatchDraw();
            cleardevice();

            // ������ϷԪ��
            maze.draw();
            player.draw();
            showSteps();

            // ��������
            handleInput();

            // ����Ƿ�ͨ��
            if (!gameWon && player.reachExit(maze)) {
                gameWon = true;

                // ��ʾͨ����Ϣ
                settextcolor(RED);
                settextstyle(24, 0, _T("����"));
                wchar_t winText[100];
                swprintf_s(winText, L"��ϲͨ�أ�����: %d", player.getStepCount());
                outtextxy(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 50, winText);
                swprintf_s(winText, L"��R�����¿�ʼ��Ϸ");
                outtextxy(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2, winText);

                // ˢ����ʾ
                EndBatchDraw();
                FlushBatchDraw();

                // �ȴ��û�����
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
                continue; // ��������ѭ��ʣ�ಿ��
            }

            // ˫�����ͼ����
            EndBatchDraw();
            Sleep(50); // ����֡��
        }

        // �ر�ͼ�δ���
        closegraph();
    }
};

// ������
int main() {
    // ���������������
    SetConsoleOutputCP(65001);

    Game game;
    game.run();
    return 0;
}