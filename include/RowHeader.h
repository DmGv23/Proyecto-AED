#ifndef ROWHEADER_H
#define ROWHEADER_H

#include "Node.h"

class RowHeader{
public:

    int row;
    Node* first;
    RowHeader* next;

    RowHeader(int r)
    {
        row = r;
        first = nullptr;
        next = nullptr;
    }
};

#endif