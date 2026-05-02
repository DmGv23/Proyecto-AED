#include "../include/SparseMatrix.h"
#include <iostream>
#include <stdexcept>
#include <climits>
#include <cfloat>
#include <algorithm>

using namespace std;

// helpers privados
bool SparseMatrix::isNumeric(const string& s) const
{
    if (s.empty()) return false;
    bool hasDot = false;
    int start = 0;
    if (s[0] == '-' || s[0] == '+') start = 1;
    if (start == (int)s.size()) return false;
    for (int i = start; i < (int)s.size(); i++) {
        if (s[i] == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

double SparseMatrix::toDouble(const string& s) const
{
    try { return stod(s); }
    catch (...) { return 0.0; }
}

// constructor

SparseMatrix::SparseMatrix()
{
    rowHeaders = nullptr;
    colHeaders = nullptr;
}

// insertcell

void SparseMatrix::insertCell(int r, int c, string val)
{
    // buscar / crear RowHeader
    RowHeader* currentRow = rowHeaders;
    RowHeader* prevRow    = nullptr;
    while (currentRow && currentRow->row < r) {
        prevRow    = currentRow;
        currentRow = currentRow->next;
    }
    if (currentRow == nullptr || currentRow->row != r) {
        RowHeader* newRow = new RowHeader(r);
        newRow->next = currentRow;
        if (prevRow == nullptr) rowHeaders    = newRow;
        else                    prevRow->next = newRow;
        currentRow = newRow;
    }

    // buscar / crear ColHeader
    ColHeader* currentCol = colHeaders;
    ColHeader* prevCol    = nullptr;
    while (currentCol && currentCol->col < c) {
        prevCol    = currentCol;
        currentCol = currentCol->next;
    }
    if (currentCol == nullptr || currentCol->col != c) {
        ColHeader* newCol = new ColHeader(c);
        newCol->next = currentCol;
        if (prevCol == nullptr) colHeaders    = newCol;
        else                    prevCol->next = newCol;
        currentCol = newCol;
    }

    // si el nodo ya existe, solo actualizar valor
    Node* temp = currentRow->first;
    while (temp != nullptr) {
        if (temp->col == c) { temp->value = val; return; }
        temp = temp->nextInRow;
    }

   
    Node* newNode = new Node(r, c, val);

    // insertar ordenado en fila
    if (currentRow->first == nullptr || currentRow->first->col > c) {
        newNode->nextInRow = currentRow->first;
        currentRow->first  = newNode;
    } else {
        Node* t = currentRow->first;
        while (t->nextInRow && t->nextInRow->col < c) t = t->nextInRow;
        newNode->nextInRow = t->nextInRow;
        t->nextInRow       = newNode;
    }

    // insertar ordenado en columna
    if (currentCol->first == nullptr || currentCol->first->row > r) {
        newNode->nextInCol = currentCol->first;
        currentCol->first  = newNode;
    } else {
        Node* t = currentCol->first;
        while (t->nextInCol && t->nextInCol->row < r) t = t->nextInCol;
        newNode->nextInCol = t->nextInCol;
        t->nextInCol       = newNode;
    }
}

// getcell

string SparseMatrix::getCell(int r, int c)
{
    RowHeader* current = rowHeaders;
    while (current && current->row != r) current = current->next;
    if (current == nullptr) return "";
    Node* temp = current->first;
    while (temp != nullptr) {
        if (temp->col == c) return temp->value;
        temp = temp->nextInRow;
    }
    return "";
}

//modifycell
void SparseMatrix::modifyCell(int r, int c, string val)
{
    RowHeader* current = rowHeaders;
    while (current && current->row != r) current = current->next;
    if (current == nullptr) return;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (temp->col == c) { temp->value = val; return; }
        temp = temp->nextInRow;
    }
}

//deletecell

void SparseMatrix::deleteCell(int r, int c)
{
    RowHeader* currentRow = rowHeaders;
    while (currentRow && currentRow->row != r) currentRow = currentRow->next;
    if (currentRow == nullptr) return;

    Node* temp = currentRow->first;
    Node* prev = nullptr;
    while (temp && temp->col != c) { prev = temp; temp = temp->nextInRow; }
    if (temp == nullptr) return;

    if (prev == nullptr) currentRow->first = temp->nextInRow;
    else                 prev->nextInRow    = temp->nextInRow;

    ColHeader* currentCol = colHeaders;
    while (currentCol && currentCol->col != c) currentCol = currentCol->next;
    if (currentCol != nullptr) {
        Node* tempCol = currentCol->first;
        Node* prevCol = nullptr;
        while (tempCol && tempCol != temp) { prevCol = tempCol; tempCol = tempCol->nextInCol; }
        if (tempCol) {
            if (prevCol == nullptr) currentCol->first      = tempCol->nextInCol;
            else                    prevCol->nextInCol = tempCol->nextInCol;
        }
    }
    delete temp;
}

//  deleterow
void SparseMatrix::deleteRow(int r)
{
    RowHeader* current = rowHeaders;
    RowHeader* prev    = nullptr;
    while (current && current->row != r) { prev = current; current = current->next; }
    if (current == nullptr) return;
    while (current->first != nullptr) deleteCell(r, current->first->col);
    if (prev == nullptr) rowHeaders  = current->next;
    else                 prev->next  = current->next;
    delete current;
}

//deetecolumn

void SparseMatrix::deleteColumn(int c)
{
    ColHeader* current = colHeaders;
    ColHeader* prev    = nullptr;
    while (current && current->col != c) { prev = current; current = current->next; }
    if (current == nullptr) return;
    while (current->first != nullptr) deleteCell(current->first->row, c);
    if (prev == nullptr) colHeaders = current->next;
    else                 prev->next = current->next;
    delete current;
}

//  deleterange

void SparseMatrix::deleteRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);

    RowHeader* rh = rowHeaders;
    while (rh != nullptr) {
        if (rh->row >= r1 && rh->row <= r2) {
            vector<int> cols;
            Node* n = rh->first;
            while (n != nullptr) {
                if (n->col >= c1 && n->col <= c2) cols.push_back(n->col);
                n = n->nextInRow;
            }
            for (int col : cols) deleteCell(rh->row, col);
        }
        rh = rh->next;
    }
}

//  agrega filas

double SparseMatrix::sumRow(int r)
{
    RowHeader* current = rowHeaders;
    while (current && current->row != r) current = current->next;
    if (current == nullptr) return 0.0;
    double sum = 0.0;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) sum += toDouble(temp->value);
        temp = temp->nextInRow;
    }
    return sum;
}

double SparseMatrix::averageRow(int r)
{
    RowHeader* current = rowHeaders;
    while (current && current->row != r) current = current->next;
    if (current == nullptr) return 0.0;
    double sum = 0.0; int count = 0;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) { sum += toDouble(temp->value); count++; }
        temp = temp->nextInRow;
    }
    return count == 0 ? 0.0 : sum / count;
}

double SparseMatrix::findMaxRow(int r)
{
    RowHeader* current = rowHeaders;
    while (current && current->row != r) current = current->next;
    if (current == nullptr) return 0.0;
    double maxVal = -DBL_MAX; bool found = false;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) {
            double v = toDouble(temp->value);
            if (!found || v > maxVal) { maxVal = v; found = true; }
        }
        temp = temp->nextInRow;
    }
    return found ? maxVal : 0.0;
}

double SparseMatrix::findMinRow(int r)
{
    RowHeader* current = rowHeaders;
    while (current && current->row != r) current = current->next;
    if (current == nullptr) return 0.0;
    double minVal = DBL_MAX; bool found = false;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) {
            double v = toDouble(temp->value);
            if (!found || v < minVal) { minVal = v; found = true; }
        }
        temp = temp->nextInRow;
    }
    return found ? minVal : 0.0;
}

// agrega columnas nuevas
double SparseMatrix::sumColumn(int c)
{
    ColHeader* current = colHeaders;
    while (current && current->col != c) current = current->next;
    if (current == nullptr) return 0.0;
    double sum = 0.0;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) sum += toDouble(temp->value);
        temp = temp->nextInCol;
    }
    return sum;
}

double SparseMatrix::averageColumn(int c)
{
    ColHeader* current = colHeaders;
    while (current && current->col != c) current = current->next;
    if (current == nullptr) return 0.0;
    double sum = 0.0; int count = 0;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) { sum += toDouble(temp->value); count++; }
        temp = temp->nextInCol;
    }
    return count == 0 ? 0.0 : sum / count;
}

double SparseMatrix::findMaxColumn(int c)
{
    ColHeader* current = colHeaders;
    while (current && current->col != c) current = current->next;
    if (current == nullptr) return 0.0;
    double maxVal = -DBL_MAX; bool found = false;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) {
            double v = toDouble(temp->value);
            if (!found || v > maxVal) { maxVal = v; found = true; }
        }
        temp = temp->nextInCol;
    }
    return found ? maxVal : 0.0;
}

double SparseMatrix::findMinColumn(int c)
{
    ColHeader* current = colHeaders;
    while (current && current->col != c) current = current->next;
    if (current == nullptr) return 0.0;
    double minVal = DBL_MAX; bool found = false;
    Node* temp = current->first;
    while (temp != nullptr) {
        if (isNumeric(temp->value)) {
            double v = toDouble(temp->value);
            if (!found || v < minVal) { minVal = v; found = true; }
        }
        temp = temp->nextInCol;
    }
    return found ? minVal : 0.0;
}

//  agrega rango

double SparseMatrix::sumRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);
    double sum = 0.0;
    RowHeader* rh = rowHeaders;
    while (rh != nullptr) {
        if (rh->row >= r1 && rh->row <= r2) {
            Node* n = rh->first;
            while (n != nullptr) {
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value))
                    sum += toDouble(n->value);
                n = n->nextInRow;
            }
        }
        rh = rh->next;
    }
    return sum;
}

double SparseMatrix::averageRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);
    double sum = 0.0; int count = 0;
    RowHeader* rh = rowHeaders;
    while (rh != nullptr) {
        if (rh->row >= r1 && rh->row <= r2) {
            Node* n = rh->first;
            while (n != nullptr) {
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value))
                    { sum += toDouble(n->value); count++; }
                n = n->nextInRow;
            }
        }
        rh = rh->next;
    }
    return count == 0 ? 0.0 : sum / count;
}

double SparseMatrix::maxRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);
    double maxVal = -DBL_MAX; bool found = false;
    RowHeader* rh = rowHeaders;
    while (rh != nullptr) {
        if (rh->row >= r1 && rh->row <= r2) {
            Node* n = rh->first;
            while (n != nullptr) {
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value)) {
                    double v = toDouble(n->value);
                    if (!found || v > maxVal) { maxVal = v; found = true; }
                }
                n = n->nextInRow;
            }
        }
        rh = rh->next;
    }
    return found ? maxVal : 0.0;
}

double SparseMatrix::minRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2) swap(r1, r2);
    if (c1 > c2) swap(c1, c2);
    double minVal = DBL_MAX; bool found = false;
    RowHeader* rh = rowHeaders;
    while (rh != nullptr) {
        if (rh->row >= r1 && rh->row <= r2) {
            Node* n = rh->first;
            while (n != nullptr) {
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value)) {
                    double v = toDouble(n->value);
                    if (!found || v < minVal) { minVal = v; found = true; }
                }
                n = n->nextInRow;
            }
        }
        rh = rh->next;
    }
    return found ? minVal : 0.0;
}

// visualizacion
void SparseMatrix::printOccupiedCells()
{
    RowHeader* currentRow = rowHeaders;
    while (currentRow != nullptr) {
        Node* temp = currentRow->first;
        while (temp != nullptr) {
            cout << "(" << temp->row << "," << temp->col << ") " << temp->value << endl;
            temp = temp->nextInRow;
        }
        currentRow = currentRow->next;
    }
}

void SparseMatrix::printRow(int r)
{
    RowHeader* current = rowHeaders;
    while (current && current->row != r) current = current->next;
    if (current == nullptr) return;
    Node* temp = current->first;
    while (temp != nullptr) {
        cout << "(" << temp->row << "," << temp->col << ") " << temp->value << endl;
        temp = temp->nextInRow;
    }
}

void SparseMatrix::printColumn(int c)
{
    ColHeader* current = colHeaders;
    while (current && current->col != c) current = current->next;
    if (current == nullptr) return;
    Node* temp = current->first;
    while (temp != nullptr) {
        cout << "(" << temp->row << "," << temp->col << ") " << temp->value << endl;
        temp = temp->nextInCol;
    }
}

vector<Node*> SparseMatrix::getAllNodes()
{
    vector<Node*> nodes;
    RowHeader* rh = rowHeaders;
    while (rh != nullptr) {
        Node* n = rh->first;
        while (n != nullptr) { nodes.push_back(n); n = n->nextInRow; }
        rh = rh->next;
    }
    return nodes;
}

// Destructor

SparseMatrix::~SparseMatrix()
{
    RowHeader* currentRow = rowHeaders;
    while (currentRow != nullptr) {
        Node* temp = currentRow->first;
        while (temp != nullptr) {
            Node* toDelete = temp;
            temp = temp->nextInRow;
            delete toDelete;
        }
        RowHeader* oldRow = currentRow;
        currentRow = currentRow->next;
        delete oldRow;
    }
    ColHeader* currentCol = colHeaders;
    while (currentCol != nullptr) {
        ColHeader* oldCol = currentCol;
        currentCol = currentCol->next;
        delete oldCol;
    }
}
