#ifndef NODE_H
#define NODE_H

#include <string>
using namespace std;

class Node {
public:
    int row;
    int col;
    string value;

    Node* nextInRow;
    Node* nextInCol;

    Node(int r, int c, string val) {
        row = r;
        col = c;
        value = val;

        nextInRow = nullptr;
        nextInCol = nullptr;
    }
};

#endif