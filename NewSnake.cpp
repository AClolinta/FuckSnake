#include <bits/stdc++.h>

#include <iostream>

#include "jsoncpp/json.h"

#define INT_MAX 2147483647  // 系统连这个宏定义都没有,无语
#define INT_MIN (-INT_MAX - 1)

using namespace std;

int n = -1;  // height指的是X轴,用n代表
int m = -1;  // width指Y轴，用m代表

const int dx[4] = {-1, 0, 1, 0};  // 左、上、右、下
const int dy[4] = {0, 1, 0, -1};

vector<vector<int>> game_map;    // 地图信息
deque<pair<int, int>> snake[2];  // snake[0]表示自己的蛇，snake[1]表示对方的蛇

int possibleDire[10];
int posCount;

random_device rd;
mt19937 mt(rd());

bool isGrow(int round) {  // 本回合是否生长

    if (round <= 9) return true;
    if ((round - 9) % 3 == 0) return true;
    return false;
}

void RemoveSnakeTail(int snake_id) {  // 删除蛇尾

    snake[snake_id].pop_back();
}

void SnakeMove(int snake_id, int dire, int num) {  // 编号为id的蛇朝向dire方向移动一步

    int _x = snake[snake_id].front().first;
    int _y = snake[snake_id].front().second;
    snake[snake_id].push_front({_x + dx[dire], _y + dy[dire]});
    if (!isGrow(num)) {
        RemoveSnakeTail(snake_id);
    }
}
void outputSnakeBody(int snake_id) {  // 调试语句

    cout << "Snake No." << snake_id << endl;
    for (auto&& it = snake[snake_id].begin(); it != snake[snake_id].end(); ++it)
        cout << it->first << " " << it->second << endl;
    cout << endl;
}

bool isBody(pair<int, int> next_dire) {  // 判断(x,y)位置是否有蛇

    int _x = next_dire.first;
    int _y = next_dire.second;
    for (int id = 0; id <= 1; id++) {
        for (auto&& it = snake[id].begin(); it != snake[id].end(); ++it) {
            if (_x == it->first && _y == it->second)
                return true;
        }
    }

    return false;
}

bool isObstacle(int snake_id, int k) {  // 判断当前移动方向的下一格是否合法

    int x = snake[snake_id].front().first + dx[k];
    int y = snake[snake_id].front().second + dy[k];
    if (x > n || y > m || x < 1 || y < 1) return false;
    if (game_map[x][y] == INT_MIN) return false;
    if (isBody({x, y})) return false;  // 判断是否接触蛇身
    return true;
}

int Random(int MOD) {  // 随机生成一个随机数

    uniform_int_distribution<int> dist(0, MOD);
    return dist(mt);
}

int FinalDecision() {
    vector<int> feasible_dir;
    // 先检查前进方向是否合法
    for (int i = 0; i < 4; ++i) {
        if (isObstacle(0, i)) {
            feasible_dir.push_back(i);
        }
    }
    return feasible_dir[Random(feasible_dir.size() - 1)];
}

int main() {
    string str;
    string temp;
    while (getline(cin, temp)) {
        str += temp;
    }

    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    n = input["requests"][(Json::Value::UInt)0]["height"].asInt();  // 棋盘高度
    m = input["requests"][(Json::Value::UInt)0]["width"].asInt();   // 棋盘宽度

    // 下面初始化地图，大小为height+1 X width+1
    game_map.resize(n + 1);
    for (auto&& _map : game_map) {
        _map.resize(m + 1);  // 第0行和第0列不使用
    }

    int snake_init = input["requests"][(Json::Value::UInt)0]["x"].asInt();  // 读蛇初始化的信息
    if (snake_init == 1) {
        snake[0].push_front({1, 1});
        snake[1].push_front({n, m});
    } else {
        snake[1].push_front({1, 1});
        snake[0].push_front({n, m});
    }
    // 处理地图中的障碍物
    int obsCnt = input["requests"][(Json::Value::UInt)0]["obstacle"].size();

    for (int i = 0; i < obsCnt; i++) {
        int _x           = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["x"].asInt();
        int _y           = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["y"].asInt();
        game_map[_x][_y] = INT_MIN;
    }

    // 根据历史信息恢复现场
    int total = input["responses"].size();

    int recover_dire;
    for (int i = 0; i < total; i++) {
        recover_dire = input["responses"][i]["direction"].asInt();
        SnakeMove(0, recover_dire, i);

        recover_dire = input["requests"][i + 1]["direction"].asInt();
        SnakeMove(1, recover_dire, i);
    }

    if (!isGrow(total))  // 本回合两条蛇生长
    {
        RemoveSnakeTail(0);
        RemoveSnakeTail(1);
    }

    srand((unsigned)time(0) + total);

    int decide_dir = FinalDecision();

    // 随机做出一个决策
    Json::Value ret;
    ret["response"]["direction"] = decide_dir;

    Json::FastWriter writer;
    cout << writer.write(ret) << endl;

    return 0;
}