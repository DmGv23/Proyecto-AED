#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include "Node.h"

class SparseMatrix {

private:
    Node** rowHeaders;
    Node** colHeaders;

    int rows;
    int cols;

public:
    SparseMatrix(int r, int c);

    void insertCell(int row,int col,std::string value);

    std::string getCell(int row,int col);

    void deleteCell(int row,int col);

    void printOccupiedCells();
    ~SparseMatrix();
};

#endif