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

int n           = -1;  // height指的是X轴,用n代表
int m           = -1;  // width指Y轴，用m代表

const int dx[4] = {-1, 0, 1, 0};  // 左、上、右、下
const int dy[4] = {0, 1, 0, -1};

vector<vector<int>> game_map;    // 地图信息
deque<pair<int, int>> snake[2];  // snake[0]表示自己的蛇，snake[1]表示对方的蛇

// struct point {
//     int x, y;
//     point(int _x, int _y) {
//         x = _x;
//         y = _y;
//     }
// };

// list<point> snake[2];  // 0表示自己的蛇，1表示对方的蛇
int possibleDire[10];
int posCount;

bool isGrow(int round) {  // 本回合是否生长

    if (round <= 9) return true;
    if ((round - 9) % 3 == 0) return true;
    return false;
}

void deleteEnd(int snake_id) {  // 删除蛇尾

    snake[snake_id].pop_back();
}

void SnakeMove(int snake_id, int dire, int num) {  // 编号为id的蛇朝向dire方向移动一步

    int _x = snake[snake_id].front().first;
    int _y = snake[snake_id].front().second;
    snake[snake_id].push_front({_x + dx[dire], _y + dy[dire]});
    if (!isGrow(num)) {
        deleteEnd(snake_id);
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
    if (isBody({x, y})) return false;
    return true;
}

int Rand(int p) {  // 随机生成一个0到p的数字
    return rand() * rand() * rand() % p;
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
        int _x          = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["x"].asInt();
        int _y          = input["requests"][(Json::Value::UInt)0]["obstacle"][(Json::Value::UInt)i]["y"].asInt();

		game_map[_x][_y] = INT_MIN;
    }

    // 根据历史信息恢复现场
    int total = input["responses"].size();

    int dire;
    for (int i = 0; i < total; i++) {
        dire = input["responses"][i]["direction"].asInt();
        SnakeMove(0, dire, i);

        dire = input["requests"][i + 1]["direction"].asInt();
        SnakeMove(1, dire, i);
    }

    if (!isGrow(total))  // 本回合两条蛇生长
    {
        deleteEnd(0);
        deleteEnd(1);
    }

    for (int k = 0; k < 4; k++)
        if (isObstacle(0, k))
            possibleDire[posCount++] = k;

    srand((unsigned)time(0) + total);

    // 随机做出一个决策
    Json::Value ret;
    ret["response"]["direction"] = possibleDire[rand() % posCount];

    Json::FastWriter writer;
    cout << writer.write(ret) << endl;

    return 0;
}