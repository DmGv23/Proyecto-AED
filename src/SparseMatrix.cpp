#include "../include/SparseMatrix.h"
#include <iostream>

SparseMatrix::SparseMatrix(int r, int c){
    rows = r;
    cols = c;

    rowHeaders = new Node*[rows];
    colHeaders = new Node*[cols];

    for(int i=0; i<rows; i++)
        rowHeaders[i] = nullptr;

    for(int j=0; j<cols; j++)
        colHeaders[j] = nullptr;
}

void SparseMatrix::insertCell(int r,int c,std::string val)
{
    Node* temp = rowHeaders[r];

    // si existe, actualizar
    while(temp != nullptr)
    {
        if(temp->col == c)
        {
            temp->value = val;
            return;
        }

        temp = temp->nextInRow;
    }

    // si no existe, crear nodo nuevo
    Node* newNode = new Node(r,c,val);

    if(rowHeaders[r]==nullptr)
    {
        rowHeaders[r]=newNode;
    }
    else
    {
        temp=rowHeaders[r];

        while(temp->nextInRow!=nullptr)
            temp=temp->nextInRow;

        temp->nextInRow=newNode;
    }

    if(colHeaders[c]==nullptr)
        colHeaders[c]=newNode;
}

std::string SparseMatrix::getCell(int row,int col)
{
    Node* temp=rowHeaders[row];

    while(temp!=nullptr)
    {
        if(temp->col==col)
            return temp->value;

        temp=temp->nextInRow;
    }
    return "";
}

void SparseMatrix::deleteCell(int row,int col)
{
    Node* temp = rowHeaders[row];
    Node* prev = nullptr;

    while(temp != nullptr)
    {
        if(temp->col == col)
        {
            if(prev == nullptr)
            {
                rowHeaders[row] = temp->nextInRow;
            }
            else
            {
                prev->nextInRow = temp->nextInRow;
            }

            delete temp;
            return;
        }

        prev = temp;
        temp = temp->nextInRow;
    }
}

void SparseMatrix::printOccupiedCells()
{
    for(int i=0;i<rows;i++)
    {
        Node* temp = rowHeaders[i];

        while(temp!=nullptr)
        {
            std::cout<<"("
                     <<temp->row<<","
                     <<temp->col<<") "
                     <<temp->value
                      <<std::endl;

            temp=temp->nextInRow;
        }
    }
}

SparseMatrix::~SparseMatrix(){
}