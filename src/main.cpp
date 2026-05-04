#include "SparseMatrix.h"
#include <iostream>
using namespace std;

int main()
{
    SparseMatrix sheet;

    // Datos base
    sheet.insertCell(2, 3, "42");
    sheet.insertCell(2, 5, "99");
    sheet.insertCell(4, 3, "77");
    sheet.insertCell(4, 6, "15");
    sheet.insertCell(1, 1, "texto"); // caso borde: texto en celda

    cout << "--- printOccupiedCells ---" << endl;
    sheet.printOccupiedCells();

    cout << "\n--- getCell (2,3) ---" << endl;
    cout << sheet.getCell(2, 3) << endl; // 42

    cout << "\n--- insertCell duplicado (actualiza, no duplica) ---" << endl;
    sheet.insertCell(2, 3, "999");
    cout << sheet.getCell(2, 3) << endl; // 999
    sheet.insertCell(2, 3, "42");        // restaurar

    cout << "\n--- sumRow(2) ---" << endl;
    cout << sheet.sumRow(2) << endl; // 141

    cout << "\n--- sumColumn(3) ---" << endl;
    cout << sheet.sumColumn(3) << endl; // 119

    cout << "\n--- averageRow(2) ---" << endl;
    cout << sheet.averageRow(2) << endl; // 70.5

    cout << "\n--- findMaxRow(2) ---" << endl;
    cout << sheet.findMaxRow(2) << endl; // 99

    cout << "\n--- findMinRow(4) ---" << endl;
    cout << sheet.findMinRow(4) << endl; // 15

    cout << "\n--- averageColumn(3) ---" << endl;
    cout << sheet.averageColumn(3) << endl;

    cout << "\n--- findMaxColumn(3) ---" << endl;
    cout << sheet.findMaxColumn(3) << endl;

    cout << "\n--- findMinColumn(3) ---" << endl;
    cout << sheet.findMinColumn(3) << endl;

    cout << "\n--- sumRange(1,1,4,6) ---" << endl;
    cout << sheet.sumRange(1, 1, 4, 6) << endl;

    cout << "\n--- averageRange(1,1,4,6) ---" << endl;
    cout << sheet.averageRange(1, 1, 4, 6) << endl;

    cout << "\n--- maxRange(1,1,4,6) ---" << endl;
    cout << sheet.maxRange(1, 1, 4, 6) << endl;

    cout << "\n--- minRange(1,1,4,6) ---" << endl;
    cout << sheet.minRange(1, 1, 4, 6) << endl;

    cout << "\n--- printRow(2) ---" << endl;
    sheet.printRow(2);

    cout << "\n--- printColumn(3) ---" << endl;
    sheet.printColumn(3);

    cout << "\n--- deleteRange(1,1,3,6) ---" << endl;
    sheet.deleteRange(1, 1, 3, 6);
    sheet.printOccupiedCells();

    // restaurar
    sheet.insertCell(2, 3, "42");
    sheet.insertCell(2, 5, "99");

    cout << "\n--- deleteRow(2) ---" << endl;
    sheet.deleteRow(2);
    sheet.printOccupiedCells();

    sheet.insertCell(2, 3, "42");
    sheet.insertCell(2, 5, "99");

    cout << "\n--- deleteColumn(3) ---" << endl;
    sheet.deleteColumn(3);
    sheet.printOccupiedCells();

    cout << "\n--- CASOS BORDE ---" << endl;
    cout << "Celda inexistente: '" << sheet.getCell(99, 99) << "'" << endl;
    cout << "Suma fila inexistente: " << sheet.sumRow(99) << endl;
    cout << "Promedio fila inexistente: " << sheet.averageRow(99) << endl;
    cout << "Max fila inexistente: " << sheet.findMaxRow(99) << endl;
    cout << "Suma rango sin numericos: " << sheet.sumRange(1, 1, 1, 1) << endl;
    sheet.deleteRow(99);
    sheet.deleteColumn(99);
    sheet.deleteCell(99, 99);
    sheet.deleteRange(99, 99, 100, 100);
    cout << "No crasheo con casos borde ✔" << endl;

    return 0;
}