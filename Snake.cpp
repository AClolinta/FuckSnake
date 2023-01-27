#pragma GCC optimize("O3", "Ofast", "inline")
#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "jsoncpp/json.h"

#define INT_MAX 2147483647  // 系统连这个宏定义都没有,无语
#define INT_MIN (-INT_MAX - 1)

using namespace std;

int n = -1;                            // height指的是X轴,用n代表
int m = -1;                            // width指Y轴，用m代表
vector<vector<int>> game_map;          // 地图信息
const int dx[4]      = {-1, 0, 1, 0};  // 左、上、右、下
const int dy[4]      = {0, 1, 0, -1};
bool invalid[25][25] = {0};

int possibleDire[10];
int posCount;

random_device rd;   // 将用于为随机数引擎获得种子
mt19937 gen(rd());  // 以播种标准 mersenne_twister_engine

deque<pair<int, int>> snake[2];  // snake[0]表示自己的蛇，snake[1]表示对方的蛇

int GetRandom(int MOD) {  // 生成随机数
    std::uniform_int_distribution<> dis(0, MOD);
    return dis(gen);
}

void OutputSnake(int index) {
    for (auto&& it = snake[index].begin(); it < snake[index].end(); ++it) {
        cout << it->first << " " << it->second << endl;
    }
    cout << "以上是蛇" << endl;
}

bool isGrow(int round) {
    if (round < 9) return true;
    if ((round - 9) % 3 == 0) return true;
    return false;
}

bool isObstacle(int index, pair<int, int> nextDir) {  // 障碍物判断,index为蛇的序号，nextDir为预计移动的点
    int _x = nextDir.first;
    int _y = nextDir.second;
    if (_x > n || _y > m || _x < 1 || _y < 1) return false;  // 越界了
    if (invalid[_x][_y]) return false;
    if (game_map[_x][_y] == INT_MIN) return false;  // 暂时用INT_MIN表示不可达

    return true;
}

bool isBody(pair<int, int> nextDir) {  // 判断是否触碰蛇身
    int nextX = nextDir.first;
    int nextY = nextDir.second;

    for (int i = 0; i < 2; ++i) {
        for (auto&& it = snake[i].begin(); it < snake[i].end(); ++it) {
            if (it->first == nextX && it->second == nextY) {
                return true;
            }
        }
    }
    return false;
}

void RemoveSnakeTail(int index) {
    // 先将记录的节点出列
    snake[index].pop_back();
}

void SnakeMove(int index, int dir, int round) {  // 编号为index的蛇，向dir方向前进（左、上、右、下）
    int x = snake[index].front().first;          // 获取蛇头所在位置,结构化绑定用不了
    int y = snake[index].front().second;
    snake[index].push_front({x + dx[dir], y + dy[dir]});
    // 下面是是否处理蛇尾的逻辑
    if (!isGrow(round)) {
        RemoveSnakeTail(index);
    }
}

// int FinalDecision() {
// }

int main() {
    vector<int> feasible_dir;  // 暂时用

    string str, temp;
    while (getline(cin, temp)) {
        str += temp;
    }

    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    // height指的是X轴，width指Y轴
    n = input["requests"][(Json::Value::UInt)0]["height"].asInt();  // 棋盘高度
    m = input["requests"][(Json::Value::UInt)0]["width"].asInt();   // 棋盘宽度
    //  cout << height << ":" << width << endl;
    /*
    下面开始初始化蛇
    蛇的初始位置在网格中的左上角(地图位置[1,1])与右下角(地图位置[n,m])
    */
    int snakeInit = input["request"][(Json::Value::UInt)0]["x"].asInt();
    if (snakeInit == 1) {
        snake[0].push_back({1, 1});
        snake[1].push_back({n, m});
    } else {
        snake[1].push_back({1, 1});
        snake[0].push_back({n, m});
    }
    // 下面初始化地图，大小为height+1 X width+1
    game_map.resize(n + 1);
    for (auto&& _map : game_map) {
        _map.resize(m + 1);  // 第0行和第0列不使用
    }

    int obsCnt = input["requests"][(Json::Value::UInt)0]["obstacle"].size();  // 处理障碍物

    for (int i = 0; i < obsCnt; ++i) {
        int obsX             = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["x"].asInt();
        int obsY             = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["y"].asInt();
        game_map[obsX][obsY] = INT_MIN;  // 将障碍物初始化为INT_MIN
        invalid[obsX][obsY]  = 1;
    }

    // 根据历史信息恢复现场
    int totalRound = input["responses"].size();
    int dire;
    for (int i = 0; i < totalRound; i++) {
        dire = input["responses"][i]["direction"].asInt();
        SnakeMove(0, dire, i);  // 0号蛇，向dire方向，第i回合

        dire = input["requests"][i + 1]["direction"].asInt();
        SnakeMove(1, dire, i);
    }

    if (!isGrow(totalRound)) {
        RemoveSnakeTail(0);
        RemoveSnakeTail(1);
    }

    int test_dir;

    for (int i = 0; i < 4; ++i) {
        if (isObstacle(0, {dx[i], dy[i]})) {
            // feasible_dir.push_back(i);
            possibleDire[posCount++]=i;
        }
    }

    if (feasible_dir.size() == 0) {
        test_dir = 2;
    }
    srand((unsigned)time(0)+totalRound);
    test_dir = possibleDire[rand()%posCount];



    // OutputSnake(0);

    // 随机决策

    Json::Value res;
    res["response"]["direction"] = test_dir;
    Json::FastWriter writer;
    cout << writer.write(res) << endl;

    return 0;
}
