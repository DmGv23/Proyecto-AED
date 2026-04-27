#include "SparseMatrix.h"
#include <iostream>
using namespace std;

int main()
{
    SparseMatrix sheet;


    // Datos base
    sheet.insertCell(2,3,"42");
    sheet.insertCell(2,5,"99");
    sheet.insertCell(4,3,"77");
    sheet.insertCell(4,6,"15");


    cout<<"--- printOccupiedCells ---"<<endl;
    sheet.printOccupiedCells();



    cout<<"\n--- getCell ---"<<endl;
    cout<<sheet.getCell(2,3)<<endl; //42



    cout<<"\n--- sumRow ---"<<endl;
    cout<<sheet.sumRow(2)<<endl; //141



    cout<<"\n--- sumColumn ---"<<endl;
    cout<<sheet.sumColumn(3)<<endl; //119



    cout<<"\n--- averageRow ---"<<endl;
    cout<<sheet.averageRow(2)<<endl; //70.5



    cout<<"\n--- findMaxRow ---"<<endl;
    cout<<sheet.findMaxRow(2)<<endl; //99



    cout<<"\n--- findMinRow ---"<<endl;
    cout<<sheet.findMinRow(4)<<endl; //15



    cout<<"\n--- printColumn(3) ---"<<endl;
    sheet.printColumn(3);



    cout<<"\n--- deleteCell(2,3) ---"<<endl;
    sheet.deleteCell(2,3);
    sheet.printOccupiedCells();



    // reinserto para seguir pruebas
    sheet.insertCell(2,3,"42");



    cout<<"\n--- deleteRow(2) ---"<<endl;
    sheet.deleteRow(2);
    sheet.printOccupiedCells();



    // reinserto
    sheet.insertCell(2,3,"42");
    sheet.insertCell(2,5,"99");



    cout<<"\n--- deleteColumn(3) ---"<<endl;
    sheet.deleteColumn(3);
    sheet.printOccupiedCells();

    cout<<"\n--- casos borde ---"<<endl;

    cout<<"Celda inexistente: "
        <<sheet.getCell(99,99)
        <<endl;

    cout<<"Suma fila inexistente: "
        <<sheet.sumRow(99)
        <<endl;

    cout<<"Promedio fila inexistente: "
        <<sheet.averageRow(99)
        <<endl;

    cout<<"Max fila inexistente: "
        <<sheet.findMaxRow(99)
        <<endl;


    sheet.deleteRow(99);
    sheet.deleteColumn(99);
    sheet.deleteCell(99,99);

    cout<<"No crasheo ✔"<<endl;


    return 0;
}