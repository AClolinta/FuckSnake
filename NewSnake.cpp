#include <bits/stdc++.h>

#include <iostream>

#include "jsoncpp/json.h"

using namespace std;

int n = -1;  // height指的是X轴,用n代表
int m = -1;  // width指Y轴，用m代表

const int dx[4] = {-1, 0, 1, 0};  // 左、上、右、下
const int dy[4] = {0, 1, 0, -1};

vector<vector<int>> game_map;      // 地图信息
deque<pair<int, int>> snake[2];    // snake[0]表示自己的蛇，snake[1]表示对方的蛇
vector<pair<int, int>> obstacles;  // 障碍物

//test
vector<int> test;
int ttcnt=1;

//test
int possibleDire[10];
int posCount;

random_device rd;
mt19937 mt(rd());

bool isGrow(int round) {  // 本回合是否生长

    if (round <= 9) return true;
    if ((round - 9) % 3 == 0) return true;
    return false;
}

bool isBody(pair<int, int> next_dire) {  // 判断(x,y)位置是否有蛇

    int _x = next_dire.first;
    int _y = next_dire.second;
    for (int id = 0; id <= 1; id++) {
        for (auto&& it = snake[id].begin(); it < snake[id].end(); ++it) {
            if (_x == it->first && _y == it->second) {
                return true;
            }
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

void BFS(pair<int, int> node, vector<vector<bool>>& visited) {
    // 每一次会遍历一个连通分量
    queue<pair<int, int>> q;
    q.push(node);
    visited[node.first][node.second] = true;  // 标记访问过的地块
    while (!q.empty()) {
        auto _node = q.front();
        q.pop();  // 头部节点出列
        for (int i = 0; i < 4; ++i) {
            int _x = dx[i] + _node.first;
            int _y = dy[i] + _node.second;
            if (_x <= n && _y <= m && _x >= 1 && _y >= 1 && visited[_x][_y] == false) {
                // 判断越界以及是否访问过
                q.push({_x, _y});
                visited[_x][_y] = true;  // 标记访问地块
            }
        }
    }
}

int GetConnectComponent(pair<int, int> node, bool flag = false) {  // 传flag参数就是计算当前联通分量，不传参就是计算预计移动的方向的联通分量

    vector<vector<bool>> visited(n + 1, vector<bool>(m + 1, false));  // 标记访问地块
    int connect_component = 0;

    // 检查的预测还是普通计算
    if (flag) {
        // 先标记预计移动方向为已经访问过的地块（相当于是蛇身）
        visited[node.first][node.second] = true;
    }
    // 标记障碍物地块
    for (auto&& _obstacle : obstacles) {
        visited[_obstacle.first][_obstacle.second] = true;
    }
    // 标记蛇身为障碍物
    for (int id = 0; id <= 1; id++) {
        for (auto&& it = snake[id].begin(); it != snake[id].end(); ++it) {
            visited[it->first][it->second] = true;
        }
    }

    // 遍历每一个地块
    for (size_t i = 1; i != visited.size(); ++i) {
        for (size_t j = 1; j != visited.front().size(); ++j) {
            if (visited[i][j] == true || game_map[i][j] == INT_MIN) continue;  // 跳过障碍物以及已经访问过的地块
            /*每一次的BFS都会找到一个连通分量*/
            BFS({i, j}, visited);
            ++connect_component;
        }
    }

    return connect_component;
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

    cout << "Snake" << snake_id << endl;
    for (auto&& it = snake[snake_id].begin(); it < snake[snake_id].end(); ++it)
        cout << it->first << " " << it->second << endl;
    cout << endl;
}

int FinalDecision() {
    vector<int> feasible_dir;
    int this_round_connect_component = GetConnectComponent({-1, -1});
    // 先检查前进方向是否合法
    for (int i = 0; i < 4; ++i) {
        if (isObstacle(0, i)) {  // 先检查边界
            if (GetConnectComponent({snake[0].front().first + dx[i], snake[0].front().first + dy[i]}, true) == this_round_connect_component) {
                feasible_dir.push_back(i);
            }
        }
    }
    // || GetConnectComponent({snake[0].front().first + dx[i], snake[0].front().first + dy[i]}) != this_round_connect_component
    return feasible_dir[Random(feasible_dir.size() - 1)];
    // return this_round_connect_component ;
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
        obstacles.push_back({_x, _y});  // 记录障碍物
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

    if (!isGrow(total)) {  // 本回合两条蛇生长
        RemoveSnakeTail(0);
        RemoveSnakeTail(1);
    }

    int decide_dir = FinalDecision();

    // 随机做出一个决策
    Json::Value ret;
    ret["response"]["direction"] = decide_dir;

    Json::FastWriter writer;
    cout << writer.write(ret) << endl;

    return 0;
}