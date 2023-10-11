#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define INITIAL_TEMP 1000000
#define COOLING_RATE 0.99999
#define STOP_THRESHOLD 0.001

using namespace std;

struct EDGE {
    int start;
    int end;
};

struct NODE {
    int x;
    int y;
};

void anneal(vector<NODE>& current, vector<EDGE>& edges);
void alter(vector<NODE>& next);
int evaluate(vector<NODE>& current, vector<EDGE>& edges);
void accept(int& curr_val, int next_val, vector<NODE>& current, vector<NODE>& next, float temperature); 
float adustTemp();

int main(int argc, char* argv[]) {
    string id;
    string line;
    int gridx, gridy;
    int vert = 0;
    vector<EDGE> edges;
    vector<NODE> nodes;


    if (argc != 3) {
        cout << "Incorrect number of input arguments. Please specify 1 input file and 1 output file";
    return 1;
    }

    const char* ifile = argv[1];
    ifstream inputFile(ifile);
    const char* ofile = argv[2];
    ifstream outputFile(ofile);

    if(!inputFile) {
        cout << "Error opening file: " << ifile << endl;
        return 1;
    }
    if(!outputFile) {
        cout << "Error opening file: " << ofile << endl;
        return 1;
    }

    while (getline(inputFile, line)) {
        istringstream instr(line);
        int xPos, yPos, xSize, ySize;

        instr >> id;

        if (id == "g") {
            instr >> gridx >> gridy;
        } else if (id == "v") {
            instr >> vert;
        } else if (id == "e") {
            EDGE edge;
            instr >> edge.start >> edge.end; 
            edges.push_back(edge);
        }
    }

    srand(time(NULL));

    for (int i=0; i < vert; i++) {
        NODE node;
        node.x = (rand())%gridx;
        node.y = (rand())%gridy;
        nodes.push_back(node);
    }

    anneal(nodes, edges);

    for(int i=0; i<nodes.size(); i++) {
        cout << nodes[i].x << " " << nodes[i].y << endl;
    }
}

float adjustTemp() {
    static float temp = INITIAL_TEMP;
    temp *= COOLING_RATE;
    return temp;
}

void anneal(vector<NODE>& current, vector<EDGE>& edges) {
    float temp;
    int curr_val, next_val;
    vector<NODE> next;
    int i = 0;

    temp = INITIAL_TEMP;
    curr_val = evaluate(current, edges);
    while (temp > STOP_THRESHOLD) {
        next = current;
        alter(next);
        accept(curr_val, next_val, current, next, temp);
        temp = adjustTemp();
        i++;
    }
}

void accept(int& curr_val, int next_val, vector<NODE>& current, vector<NODE>& next, float temp) {
    int delta_e, i;
    float p, r;

    delta_e = next_val - curr_val;
    if (delta_e <= 0) {
        current = next;
        curr_val = next_val;
    } else {
        p = exp(-((float)delta_e)/temp);
        r = (float)rand() / RAND_MAX;
        if (r<p) {
            current = next;
            curr_val = next_val;
        }
    }
}

void alter(vector<NODE>& next) {
    int a, b, temp;
    do {
        a = rand()%next.size();
        b = rand()%next.size();
    } while (a == b);
    temp = next[a].x;
    next[a].x = next[b].x;
    next[b].x = temp;
    temp = next[a].y;
    next[a].y = next[b].y;
    next[b].y = temp;
}

int evaluate(vector<NODE>& next, vector<EDGE>& edges) {
    int distance, i;

    distance = 0;
    for (EDGE edge : edges) {
        distance += abs(next[edge.end].x - next[edge.start].x) + abs(next[edge.end].y - next[edge.start].y);
    }
    return distance;
}