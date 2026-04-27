#include "SparseMatrix.h"
#include <iostream>

int main()
{
    SparseMatrix sheet(10,10);

    sheet.insertCell(2,3,"42");
    sheet.insertCell(2,5,"99");
    sheet.insertCell(4,3,"77");

    sheet.deleteCell(2,3);

    sheet.printOccupiedCells();

    return 0;
}