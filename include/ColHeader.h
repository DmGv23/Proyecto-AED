#ifndef COLHEADER_H
#define COLHEADER_H

#include "Node.h"

class ColHeader{
public:

    int col;
    Node* first;
    ColHeader* next;

    ColHeader(int c)
    {
        col = c;
        first = nullptr;
        next = nullptr;
    }
};

#endif