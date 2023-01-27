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

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(std::pair<T1, T2> const& pair) const {
        std::size_t h1 = std::hash<T1>()(pair.first);
        std::size_t h2 = std::hash<T2>()(pair.second);
        return h1 ^ h2;
    }
};

int width, height;
vector<vector<int>> game_map;     // 地图信息
const int dx[4] = {-1, 0, 1, 0};  // 左、上、右、下
const int dy[4] = {0, 1, 0, -1};

random_device rd;   // 将用于为随机数引擎获得种子
mt19937 gen(rd());  // 以播种标准 mersenne_twister_engine

deque<pair<int, int>> snake[2];                          // snake[0]表示自己的蛇，snake[1]表示对方的蛇
unordered_set<pair<int, int>, pair_hash> snakeTrack[2];  // 存储蛇身的节点，提高检索效率

int GetRandom(int MOD) {  // 生成随机数
    std::uniform_int_distribution<> dis(0, MOD);
    return dis(gen);
}

// Json::Value GameInit() {
//     string str, temp;
//     while (getline(cin, temp)) {
//         str += temp;
//     }

//     Json::Reader reader;
//     Json::Value input;
//     reader.parse(str, input);
//     height = input["requests"][(Json::Value::UInt)0]["height"].asInt();  // 棋盘高度
//     width  = input["requests"][(Json::Value::UInt)0]["width"].asInt();   // 棋盘宽度
//     /*
//     下面开始初始化蛇
//     蛇的初始位置在网格中的左上角(地图位置[1,1])与右下角(地图位置[n,m])
//     */
//     int snakeInit = input["request"][(Json::Value::UInt)0]["x"].asInt();
//     if (snakeInit == 1) {
//         snake[0].push_back({1, 1});
//         snake[1].push_back({height, width});
//     } else {
//         snake[1].push_back({1, 1});
//         snake[0].push_back({height, width});
//     }
//     // 下面初始化地图，大小为height+1 X width+1
//     game_map.resize(height + 1);
//     for (auto&& m : game_map) {
//         m.resize(width + 1);  // 第0行和第0列不使用
//     }

//     int obsCnt = input["requests"][(Json::Value::UInt)0]["obstacle"].size();  // 处理障碍物

//     for (int i = 0; i < obsCnt; ++i) {
//         int obsX             = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["x"].asInt();
//         int obsY             = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["y"].asInt();
//         game_map[obsX][obsY] = INT_MIN;  // 将障碍物初始化为INT_MIN
//     }

//     return input;
// }

bool isGrow(int round) {
    if (round < 9) return true;
    if ((round - 9) % 3 == 0) return true;
    return false;
}

bool isObstacle(int index, pair<int, int> nextDir) {  // 障碍物判断,index为蛇的序号，nextDir为预计移动的点
    int nextX = nextDir.first;
    int nextY = nextDir.second;
    if (nextX < 1 || nextY < 1 || nextX > width || nextY > height) return false;  // 越界了
    if (game_map[nextX][nextY] == INT_MIN) return false;                          // 暂时用INT_MIN表示不可达

    return true;
}

void RemoveSnakeTail(int index) {
    // 先将记录的节点出列
    snakeTrack[index].erase(snakeTrack[index].find(snake[index].back()));
    snake[index].pop_back();
}

void SnakeMove(int index, int dir, int round) {  // 编号为index的蛇，向dir方向前进（左、上、右、下）
    auto x = snake[index].front().first;         // 获取蛇头所在位置,结构化绑定用不了
    int y  = snake[index].front().second;
    snake[index].push_front({x + dx[dir], y + dy[dir]});
    // 下面是是否处理蛇尾的逻辑
    if (!isGrow(round)) {
        RemoveSnakeTail(index);
    }
    // 记录一下蛇身的节点
    snakeTrack[index].emplace(snake[index].front());
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
    height = input["requests"][(Json::Value::UInt)0]["height"].asInt();  // 棋盘高度
    width  = input["requests"][(Json::Value::UInt)0]["width"].asInt();   // 棋盘宽度
    /*
    下面开始初始化蛇
    蛇的初始位置在网格中的左上角(地图位置[1,1])与右下角(地图位置[n,m])
    */
    int snakeInit = input["request"][(Json::Value::UInt)0]["x"].asInt();
    if (snakeInit == 1) {
        snake[0].push_back({1, 1});
        snake[1].push_back({height, width});
    } else {
        snake[1].push_back({1, 1});
        snake[0].push_back({height, width});
    }
    // 下面初始化地图，大小为height+1 X width+1
    game_map.resize(height + 1);
    for (auto&& m : game_map) {
        m.resize(width + 1);  // 第0行和第0列不使用
    }

    int obsCnt = input["requests"][(Json::Value::UInt)0]["obstacle"].size();  // 处理障碍物

    for (int i = 0; i < obsCnt; ++i) {
        int obsX             = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["x"].asInt();
        int obsY             = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["y"].asInt();
        game_map[obsX][obsY] = INT_MIN;  // 将障碍物初始化为INT_MIN
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

    for (int i = 0; i < 4; ++i) {
        if (isObstacle(0, {dx[i], dy[i]})) {
            feasible_dir.push_back(i);
        }
    }

    // 随机决策

    Json::Value res;
    res["response"]["direction"] = feasible_dir[rand() % (feasible_dir.size() - 1)];
    Json::FastWriter writer;
    cout << writer.write(res) << endl;

    return 0;
}
