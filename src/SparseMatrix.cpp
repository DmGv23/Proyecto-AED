#include "../include/SparseMatrix.h"
#include <iostream>

SparseMatrix::SparseMatrix()
{
    rowHeaders = nullptr;
    colHeaders = nullptr;
}

void SparseMatrix::insertCell(int r,int c,string val)
{
    RowHeader* currentRow = rowHeaders;
    RowHeader* prevRow = nullptr;

    while(currentRow && currentRow->row < r)
    {
        prevRow = currentRow;
        currentRow = currentRow->next;
    }

    if(currentRow == nullptr || currentRow->row != r)
    {
        RowHeader* newRow =
            new RowHeader(r);

        newRow->next = currentRow;

        if(prevRow==nullptr)
            rowHeaders = newRow;
        else
            prevRow->next = newRow;

        currentRow = newRow;
    }


    ColHeader* currentCol = colHeaders;
    ColHeader* prevCol = nullptr;

    while(currentCol && currentCol->col < c)
    {
        prevCol = currentCol;
        currentCol = currentCol->next;
    }

    if(currentCol==nullptr || currentCol->col!=c)
    {
        ColHeader* newCol =
            new ColHeader(c);

        newCol->next = currentCol;

        if(prevCol==nullptr)
            colHeaders = newCol;
        else
            prevCol->next = newCol;

        currentCol = newCol;
    }


    Node* newNode = new Node(r,c,val);

    // insertar ordenado en fila
    if(currentRow->first==nullptr ||
       currentRow->first->col > c)
    {
        newNode->nextInRow =
            currentRow->first;

        currentRow->first =
            newNode;
    }
    else
    {
        Node* temp = currentRow->first;

        while(temp->nextInRow &&
              temp->nextInRow->col < c)
        {
            temp = temp->nextInRow;
        }

        newNode->nextInRow =
            temp->nextInRow;

        temp->nextInRow =
            newNode;
    }



    // insertar ordenado en columna
    if(currentCol->first==nullptr ||
       currentCol->first->row > r)
    {
        newNode->nextInCol =
            currentCol->first;

        currentCol->first =
            newNode;
    }
    else
    {
        Node* temp = currentCol->first;

        while(temp->nextInCol &&
              temp->nextInCol->row < r)
        {
            temp = temp->nextInCol;
        }

        newNode->nextInCol =
            temp->nextInCol;

        temp->nextInCol =
            newNode;
    }
}


string SparseMatrix::getCell(int r,int c)
{
    RowHeader* current = rowHeaders;

    while(current && current->row != r)
        current = current->next;

    if(current == nullptr)
        return "";


    Node* temp = current->first;

    while(temp != nullptr)
    {
        if(temp->col == c)
            return temp->value;

        temp = temp->nextInRow;
    }

    return "";
}


void SparseMatrix::deleteCell(int r,int c)
{
    RowHeader* currentRow = rowHeaders;

    while(currentRow && currentRow->row != r)
        currentRow = currentRow->next;

    if(currentRow == nullptr)
        return;



    Node* temp = currentRow->first;
    Node* prev = nullptr;

    while(temp && temp->col != c)
    {
        prev = temp;
        temp = temp->nextInRow;
    }

    if(temp==nullptr)
        return;


    if(prev==nullptr)
        currentRow->first=temp->nextInRow;
    else
        prev->nextInRow=temp->nextInRow;



    ColHeader* currentCol = colHeaders;

    while(currentCol && currentCol->col!=c)
        currentCol=currentCol->next;

    if(currentCol!=nullptr)
    {
        Node* tempCol=currentCol->first;
        Node* prevCol=nullptr;

        while(tempCol && tempCol!=temp)
        {
            prevCol=tempCol;
            tempCol=tempCol->nextInCol;
        }

        if(tempCol)
        {
            if(prevCol==nullptr)
                currentCol->first=tempCol->nextInCol;
            else
                prevCol->nextInCol=tempCol->nextInCol;
        }
    }

    delete temp;
}


void SparseMatrix::deleteRow(int r)
{
    RowHeader* current = rowHeaders;
    RowHeader* prev = nullptr;

    while(current && current->row != r)
    {
        prev = current;
        current = current->next;
    }

    if(current==nullptr)
        return;


    while(current->first != nullptr)
    {
        int col = current->first->col;

        deleteCell(r,col);
    }


    if(prev==nullptr)
        rowHeaders=current->next;
    else
        prev->next=current->next;

    delete current;
}

void SparseMatrix::deleteColumn(int c)
{
    ColHeader* current = colHeaders;
    ColHeader* prev = nullptr;

    while(current && current->col != c)
    {
        prev = current;
        current = current->next;
    }

    if(current == nullptr)
        return;


    while(current->first != nullptr)
    {
        int row = current->first->row;

        deleteCell(row,c);
    }


    if(prev==nullptr)
        colHeaders=current->next;
    else
        prev->next=current->next;

    delete current;
}

int SparseMatrix::sumRow(int r)
{
    RowHeader* current=rowHeaders;

    while(current && current->row!=r)
        current=current->next;

    if(current==nullptr)
        return 0;


    int sum=0;

    Node* temp=current->first;

    while(temp!=nullptr)
    {
        sum += stoi(temp->value);

        temp=temp->nextInRow;
    }

    return sum;
}

int SparseMatrix::sumColumn(int c)
{
    ColHeader* current = colHeaders;

    while(current && current->col != c)
        current = current->next;

    if(current == nullptr)
        return 0;


    int sum = 0;

    Node* temp = current->first;

    while(temp != nullptr)
    {
        sum += stoi(temp->value);

        temp = temp->nextInCol;
    }

    return sum;
}

double SparseMatrix::averageRow(int r)
{
    RowHeader* current = rowHeaders;

    while(current && current->row != r)
        current = current->next;

    if(current == nullptr)
        return 0;


    int suma = 0;
    int count = 0;

    Node* temp = current->first;

    while(temp != nullptr)
    {
        suma += stoi(temp->value);

        count++;

        temp = temp->nextInRow;
    }

    if(count==0)
        return 0;

    return (double)suma/count;
}

int SparseMatrix::findMaxRow(int r)
{
    RowHeader* current = rowHeaders;

    while(current && current->row != r)
        current = current->next;

    if(current == nullptr)
        return 0;


    Node* temp = current->first;

    if(temp == nullptr)
        return 0;


    int maximo = stoi(temp->value);

    while(temp != nullptr)
    {
        int valor = stoi(temp->value);

        if(valor > maximo)
            maximo = valor;

        temp = temp->nextInRow;
    }

    return maximo;
}

int SparseMatrix::findMinRow(int r)
{
    RowHeader* current = rowHeaders;

    while(current && current->row != r)
        current = current->next;

    if(current == nullptr)
        return 0;


    Node* temp = current->first;

    if(temp == nullptr)
        return 0;


    int minimo = stoi(temp->value);

    while(temp != nullptr)
    {
        int valor = stoi(temp->value);

        if(valor < minimo)
            minimo = valor;

        temp = temp->nextInRow;
    }

    return minimo;
}



void SparseMatrix::printOccupiedCells()
{
    RowHeader* currentRow = rowHeaders;

    while(currentRow != nullptr)
    {
        Node* temp = currentRow->first;

        while(temp != nullptr)
        {
            cout<<"("
                <<temp->row
                <<","
                <<temp->col
                <<") "
                <<temp->value
                <<endl;

            temp = temp->nextInRow;
        }

        currentRow = currentRow->next;
    }
}


void SparseMatrix::printColumn(int c)
{
    ColHeader* current = colHeaders;

    while(current && current->col != c)
        current = current->next;

    if(current == nullptr)
        return;


    Node* temp = current->first;

    while(temp != nullptr)
    {
        cout<<"("
            <<temp->row
            <<","
            <<temp->col
            <<") "
            <<temp->value
            <<endl;

        temp = temp->nextInCol;
    }
}

SparseMatrix::~SparseMatrix()
{
    RowHeader* currentRow = rowHeaders;

    while(currentRow != nullptr)
    {
        Node* temp = currentRow->first;

        while(temp != nullptr)
        {
            Node* toDelete = temp;

            temp = temp->nextInRow;

            delete toDelete;
        }

        RowHeader* oldRow = currentRow;

        currentRow = currentRow->next;

        delete oldRow;
    }



    ColHeader* currentCol = colHeaders;

    while(currentCol != nullptr)
    {
        ColHeader* oldCol = currentCol;

        currentCol = currentCol->next;

        delete oldCol;
    }
}