#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include "Node.h"
#include "RowHeader.h"
#include "ColHeader.h"

class SparseMatrix {

private:
    RowHeader* rowHeaders;
    ColHeader* colHeaders;

public:
    SparseMatrix();   // <- ya sin tamaños

    void insertCell(int row,int col,std::string value);

    std::string getCell(int row,int col);

    void deleteCell(int row,int col);

    void deleteRow(int row);

    void deleteColumn(int col);

    int sumRow(int row);

    int sumColumn(int col);

    double averageRow(int row);

    int findMaxRow(int row);

    int findMinRow(int row);

    void printOccupiedCells();

    void printColumn(int col);

    ~SparseMatrix();
};

#endif