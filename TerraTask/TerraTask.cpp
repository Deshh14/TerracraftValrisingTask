#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <cmath>

using namespace std;

const int VALUE_IRON = 7;
const int VALUE_GOLD = 11;
const int VALUE_GEMS = 23;
const int VALUE_EXP = 1;

int N;
vector<vector<int>> graph;
vector<int> iron, gold, gems, xp;
vector<int> iron_left, gold_left, gems_left, xp_left;
vector<bool> visited;
string target;
int food;

int getValue(const string& res) {
    int v;
    if (res == "iron") v = VALUE_IRON;
    else if (res == "gold") v = VALUE_GOLD;
    else if (res == "gems") v = VALUE_GEMS;
    else v = VALUE_EXP;

    if (res == target) v *= 2;
    return v;
}

int& getLeft(int room, const string& res) {
    if (res == "iron") return iron_left[room];
    if (res == "gold") return gold_left[room];
    if (res == "gems") return gems_left[room];
    return xp_left[room];
}

bool hasAnyResources(int room) {
    return iron_left[room] > 0 || gold_left[room] > 0 ||
        gems_left[room] > 0 || xp_left[room] > 0;
}

string bestResource(int room) {
    vector<pair<int, string>> vals;
    if (iron_left[room] > 0) vals.push_back({ getValue("iron"), "iron" });
    if (gold_left[room] > 0) vals.push_back({ getValue("gold"), "gold" });
    if (gems_left[room] > 0) vals.push_back({ getValue("gems"), "gems" });
    if (xp_left[room] > 0) vals.push_back({ getValue("exp"), "exp" });

    if (vals.empty()) return "";

    sort(vals.begin(), vals.end());
    return vals.back().second;
}

void printState(ofstream& out, int room) {
    out << "state " << room << " ";
    out << (iron_left[room] > 0 ? to_string(iron_left[room]) : "_") << " ";
    out << (gold_left[room] > 0 ? to_string(gold_left[room]) : "_") << " ";
    out << (gems_left[room] > 0 ? to_string(gems_left[room]) : "_") << " ";
    out << (xp_left[room] > 0 ? to_string(xp_left[room]) : "_") << "\n";
}

vector<int> findNearestUnvisited(int current) {
    queue<int> q;
    vector<int> dist(N + 1, -1);
    set<int> visited_set;

    q.push(current);
    dist[current] = 0;
    visited_set.insert(current);

    int minDist = -1;
    vector<int> candidates;

    while (!q.empty()) {
        int v = q.front(); q.pop();

        if (!visited[v] && v != current) {
            if (minDist == -1) {
                minDist = dist[v];
                candidates.push_back(v);
            }
            else if (dist[v] == minDist) {
                candidates.push_back(v);
            }
            else if (dist[v] > minDist) {
                break;
            }
        }

        if (minDist != -1 && dist[v] >= minDist) continue;

        for (int to : graph[v]) {
            if (visited_set.find(to) == visited_set.end()) {
                visited_set.insert(to);
                dist[to] = dist[v] + 1;
                q.push(to);
            }
        }
    }

    sort(candidates.begin(), candidates.end());
    return candidates;
}

vector<int> buildPath(int from, int to) {
    if (from == to) return { from };

    queue<vector<int>> q;
    set<int> used;
    q.push({ from });
    used.insert(from);

    while (!q.empty()) {
        vector<int> path = q.front(); q.pop();
        int v = path.back();

        vector<int> neigh = graph[v];
        sort(neigh.begin(), neigh.end());

        for (int nxt : neigh) {
            if (nxt == to) {
                vector<int> newPath = path;
                newPath.push_back(nxt);
                return newPath;
            }
            if (used.find(nxt) == used.end()) {
                used.insert(nxt);
                vector<int> newPath = path;
                newPath.push_back(nxt);
                q.push(newPath);
            }
        }
    }
    return {};
}

vector<int> pathToStart(int current) {
    if (current == 0) return { 0 };

    queue<vector<int>> q;
    set<int> used;
    q.push({ current });
    used.insert(current);

    while (!q.empty()) {
        vector<int> path = q.front(); q.pop();
        int v = path.back();

        if (v == 0) return path;

        vector<int> neigh = graph[v];
        sort(neigh.begin(), neigh.end());

        for (int nxt : neigh) {
            if (visited[nxt] && used.find(nxt) == used.end()) {
                used.insert(nxt);
                vector<int> newPath = path;
                newPath.push_back(nxt);
                q.push(newPath);
            }
        }
    }
    return {};
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " input.txt" << endl;
        return 1;
    }

    ifstream in(argv[1]);
    if (!in) {
        ofstream err("result.txt");
        err << "invalid input format\n";
        return 0;
    }

    string line;
    if (!getline(in, line)) {
        ofstream err("result.txt");
        err << "invalid input format\n";
        return 0;
    }
    try {
        N = stoi(line);
    }
    catch (...) {
        ofstream err("result.txt");
        err << "invalid input format\n";
        return 0;
    }
    if (N < 1 || N > 255) {
        ofstream err("result.txt");
        err << "invalid input format\n";
        return 0;
    }

    graph.resize(N + 1);
    iron.resize(N + 1, 0);
    gold.resize(N + 1, 0);
    gems.resize(N + 1, 0);
    xp.resize(N + 1, 0);
    iron_left.resize(N + 1, 0);
    gold_left.resize(N + 1, 0);
    gems_left.resize(N + 1, 0);
    xp_left.resize(N + 1, 0);
    visited.assign(N + 1, false);

    for (int i = 0; i <= N; i++) {
        if (!getline(in, line)) {
            ofstream err("result.txt");
            err << "invalid input format\n";
            return 0;
        }
        stringstream ss(line);
        int id;
        string neighStr;
        ss >> id >> neighStr;

        replace(neighStr.begin(), neighStr.end(), ',', ' ');
        stringstream ns(neighStr);
        int nb;
        while (ns >> nb) {
            if (nb < 0 || nb > N) {
                ofstream err("result.txt");
                err << "invalid input format\n";
                return 0;
            }
            graph[id].push_back(nb);
        }

        int iro, gol, gem, ex;
        if (!(ss >> iro >> gol >> gem >> ex)) {
            ofstream err("result.txt");
            err << "invalid input format\n";
            return 0;
        }
        if (iro < 0 || iro > 255 || gol < 0 || gol > 255 ||
            gem < 0 || gem > 255 || ex < 0 || ex > 255) {
            ofstream err("result.txt");
            err << "invalid input format\n";
            return 0;
        }

        iron[id] = iro;
        gold[id] = gol;
        gems[id] = gem;
        xp[id] = ex;
        iron_left[id] = iro;
        gold_left[id] = gol;
        gems_left[id] = gem;
        xp_left[id] = ex;
    }

    if (!getline(in, line)) {
        ofstream err("result.txt");
        err << "invalid input format\n";
        return 0;
    }
    stringstream ss(line);
    ss >> food >> target;
    if (food < 2 || food > 255) {
        ofstream err("result.txt");
        err << "invalid input format\n";
        return 0;
    }
    if (target != "iron" && target != "gold" && target != "gems" && target != "exp") {
        ofstream err("result.txt");
        err << "invalid input format\n";
        return 0;
    }

    ofstream out("result.txt");
    if (!out) return 1;

    int current = 0;
    visited[0] = true;
    int foodSpent = 0;
    int halfFood = food / 2;
    bool firstCollect[256] = { false };

    while (foodSpent < halfFood && food > 0) {
        int nextRoom = -1;

        vector<int> unvisitedNeighbors;
        for (int nb : graph[current]) {
            if (!visited[nb]) {
                unvisitedNeighbors.push_back(nb);
            }
        }

        if (!unvisitedNeighbors.empty()) {
            sort(unvisitedNeighbors.begin(), unvisitedNeighbors.end());
            nextRoom = unvisitedNeighbors[0];
        }
        else {
            vector<int> nearest = findNearestUnvisited(current);
            if (!nearest.empty()) {
                vector<int> path = buildPath(current, nearest[0]);
                if (path.size() >= 2) {
                    nextRoom = path[1];
                }
            }
        }

        if (nextRoom == -1) break;

        if (food <= 0) break;
        food--;
        foodSpent++;
        out << "go " << nextRoom << "\n";
        current = nextRoom;
        visited[current] = true;

        printState(out, current);

        if (!firstCollect[current] && hasAnyResources(current)) {
            string res = bestResource(current);
            if (!res.empty()) {
                out << "collect " << res << "\n";
                getLeft(current, res)--;
                firstCollect[current] = true;
                printState(out, current);
            }
        }
    }

    vector<int> wayHome = pathToStart(current);
    if (wayHome.empty()) {
        out << "result 0 0 0 0 0\n";
        return 0;
    }

    int stepsToHome = wayHome.size() - 1;
    int extraFood = food - stepsToHome;

    if (extraFood < 0) {
        out << "result 0 0 0 0 0\n";
        return 0;
    }

    for (size_t i = 1; i < wayHome.size(); i++) {
        int next = wayHome[i];

        food--;
        out << "go " << next << "\n";
        current = next;

        if (current != 0) {
            printState(out, current);
        }

        if (extraFood > 0 && current != 0) {
            while (extraFood > 0 && hasAnyResources(current)) {
                string res = bestResource(current);
                if (res.empty()) break;

                out << "collect " << res << "\n";
                getLeft(current, res)--;
                extraFood--;
                food--;
                printState(out, current);
            }
        }
    }

    int totalIron = 0, totalGold = 0, totalGems = 0, totalExp = 0;
    int totalValue = 0;

    for (int i = 0; i <= N; i++) {
        int colIron = iron[i] - iron_left[i];
        int colGold = gold[i] - gold_left[i];
        int colGems = gems[i] - gems_left[i];
        int colExp = xp[i] - xp_left[i];

        totalIron += colIron;
        totalGold += colGold;
        totalGems += colGems;
        totalExp += colExp;

        totalValue += colIron * getValue("iron");
        totalValue += colGold * getValue("gold");
        totalValue += colGems * getValue("gems");
        totalValue += colExp * getValue("exp");
    }

    out << "result " << totalIron << " " << totalGold << " "
        << totalGems << " " << totalExp << " " << totalValue << "\n";

    return 0;
}