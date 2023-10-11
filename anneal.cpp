#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>

#define INITIAL_TEMP 1000000
#define COOLING_RATE 0.9
#define STOP_THRESHOLD 0.001

using namespace std;

// Define a structure to represent an edge between nodes
struct EDGE {
    int start;  // Start node of the edge
    int end;    // End node of the edge
};

// Define a structure to represent a node with x and y coordinates
struct NODE {
    int x;  // x-coordinate of the node
    int y;  // y-coordinate of the node
};

// Function prototypes
void anneal(vector<NODE>& current, vector<EDGE>& edges);
void alter(vector<NODE>& next);
int evaluate(vector<NODE>& current, vector<EDGE>& edges);
void accept(int& curr_val, int next_val, vector<NODE>& current, vector<NODE>& next, float temperature);
float adjustTemp();

// Main function
int main(int argc, char* argv[]) {
    auto start = chrono::high_resolution_clock::now();
    string id;
    string line;
    int gridx, gridy;
    int vert = 0;
    int length = 0;
    vector<EDGE> edges;
    vector<NODE> nodes;

    // Check if the correct number of command-line arguments is provided
    if (argc != 3) {
        cout << "Incorrect number of input arguments. Please specify 1 input file and 1 output file";
        return -1;
    }

    const char* ifile = argv[1];
    ifstream inputFile(ifile);
    const char* ofile = argv[2];
    ofstream outputFile(ofile);

    // Check if the input and output files are successfully opened
    if (!inputFile) {
        cout << "Error opening file: " << ifile << endl;
        return -1;
    }
    if (!outputFile) {
        cout << "Error opening file: " << ofile << endl;
        return -1;
    }

    // Read input file to populate grid information, node count, and edges
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

    // Initialize random seed
    srand(time(NULL));

    // Generate random node coordinates within the grid
    for (int i = 0; i < vert; i++) {
        NODE node;
        node.x = (rand()) % gridx;
        node.y = (rand()) % gridy;
        nodes.push_back(node);
    }

    // Write node information to the output file
    for (int i = 0; i < vert; i++) {
        outputFile << "Node " << i << " placed at (" << nodes[i].x << ", " << nodes[i].y << ")" << endl;
    }

    // Call the annealing algorithm to optimize the layout
    anneal(nodes, edges);

    // Calculate and write edge lengths to the output file
    for (EDGE lengths : edges) {
        length = abs(nodes[lengths.end].x - nodes[lengths.start].x) + abs(nodes[lengths.end].y - nodes[lengths.start].y);
        outputFile << "Edge from " << lengths.start << " to " << lengths.end << " has length " << length << endl;
    }

    // Close input and output files
    inputFile.close();
    outputFile.close();

    auto end = chrono::high_resolution_clock::now();
    auto runtime = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Runtime = " << runtime.count() << " microseconds" << endl;
}

// Function to adjust the temperature during the annealing process
float adjustTemp() {
    static float temp = INITIAL_TEMP;
    temp *= COOLING_RATE;
    return temp;
}

// Annealing algorithm function
void anneal(vector<NODE>& current, vector<EDGE>& edges) {
    float temp;
    int curr_val, next_val;
    vector<NODE> next;
    int i = 0;

    temp = INITIAL_TEMP;
    curr_val = evaluate(current, edges);
    cout << "Initial Score= " << curr_val << endl << endl;

    // Annealing loop
    while (temp > STOP_THRESHOLD) {
        next = current;
        alter(next);
        next_val = evaluate(next, edges);
        accept(curr_val, next_val, current, next, temp);
        temp = adjustTemp();
        i++;
    }

    // Output the results
    cout << "Explored " << i << " solutions" << endl;
    cout << "Final score = " << curr_val << endl << endl;
}

// Function to accept or reject a new solution during annealing
void accept(int& curr_val, int next_val, vector<NODE>& current, vector<NODE>& next, float temp) {
    int delta_e;
    float p, r;

    delta_e = next_val - curr_val;
    if (delta_e <= 0) {
        current = next;
        curr_val = next_val;
    } else {
        p = exp(-((float)delta_e) / temp);
        r = (float)rand() / RAND_MAX;
        if (r < p) {
            current = next;
            curr_val = next_val;
        }
    }
}

// Function to alter the current solution (swap two nodes)
void alter(vector<NODE>& next) {
    int a, b;
    NODE temp;
    do {
        a = rand() % next.size();
        b = rand() % next.size();
    } while (a == b);
    temp = next[a];
    next[a] = next[b];
    next[b] = temp;
}

// Function to evaluate the quality of a solution by calculating total edge length
int evaluate(vector<NODE>& next, vector<EDGE>& edges) {
    int distance = 0;

    for (EDGE edge : edges) {
        distance += abs(next[edge.end].x - next[edge.start].x) + abs(next[edge.end].y - next[edge.start].y);
    }

    return distance;
}