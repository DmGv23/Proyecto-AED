#include "../include/SparseMatrix.h"
#include <iostream>
#include <algorithm>
#include <cfloat>
using namespace std;

// ── Helpers privados ──────────────────────────────────────────────────────────

bool SparseMatrix::isNumeric(const string &s) const
{
    if (s.empty())
        return false;
    bool hasDot = false;
    int start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    if (start == (int)s.size())
        return false;
    for (int i = start; i < (int)s.size(); i++)
    {
        if (s[i] == '.')
        {
            if (hasDot)
                return false;
            hasDot = true;
        }
        else if (s[i] < '0' || s[i] > '9')
        {
            return false;
        }
    }
    return true;
}

double SparseMatrix::toDouble(const string &s) const
{
    try
    {
        return stod(s);
    }
    catch (...)
    {
        return 0.0;
    }
}

// Busca o crea la cabecera de fila para r, insertando en orden ascendente.
Header *SparseMatrix::findOrCreateRow(int r)
{
    Header *cur = rowHeaders;
    Header *prev = nullptr;
    while (cur && cur->index < r)
    {
        prev = cur;
        cur = cur->next;
    }
    if (cur && cur->index == r)
        return cur;

    Header *h = new Header(r);
    h->next = cur;
    if (prev)
        prev->next = h;
    else
        rowHeaders = h;
    return h;
}

// Busca o crea la cabecera de columna para c, insertando en orden ascendente.
Header *SparseMatrix::findOrCreateCol(int c)
{
    Header *cur = colHeaders;
    Header *prev = nullptr;
    while (cur && cur->index < c)
    {
        prev = cur;
        cur = cur->next;
    }
    if (cur && cur->index == c)
        return cur;

    Header *h = new Header(c);
    h->next = cur;
    if (prev)
        prev->next = h;
    else
        colHeaders = h;
    return h;
}

// ── Constructor / Destructor ──────────────────────────────────────────────────

SparseMatrix::SparseMatrix() : rowHeaders(nullptr), colHeaders(nullptr) {}

SparseMatrix::~SparseMatrix()
{
    // Liberar todos los nodos recorriendo por filas
    Header *rh = rowHeaders;
    while (rh)
    {
        Node *n = rh->first;
        while (n)
        {
            Node *tmp = n;
            n = n->nextInRow;
            delete tmp;
        }
        Header *old = rh;
        rh = rh->next;
        delete old;
    }
    // Liberar cabeceras de columna (los nodos ya fueron eliminados)
    Header *ch = colHeaders;
    while (ch)
    {
        Header *old = ch;
        ch = ch->next;
        delete old;
    }
}

// ── Insertar / Actualizar ─────────────────────────────────────────────────────

void SparseMatrix::insertCell(int r, int c, string val)
{
    Header *rowH = findOrCreateRow(r);
    Header *colH = findOrCreateCol(c);

    // Si ya existe el nodo, solo actualizar valor
    Node *tmp = rowH->first;
    while (tmp)
    {
        if (tmp->col == c)
        {
            tmp->value = val;
            return;
        }
        tmp = tmp->nextInRow;
    }

    Node *node = new Node(r, c, val);

    // Insertar ordenado en fila
    if (!rowH->first || rowH->first->col > c)
    {
        node->nextInRow = rowH->first;
        rowH->first = node;
    }
    else
    {
        Node *t = rowH->first;
        while (t->nextInRow && t->nextInRow->col < c)
            t = t->nextInRow;
        node->nextInRow = t->nextInRow;
        t->nextInRow = node;
    }

    // Insertar ordenado en columna
    if (!colH->first || colH->first->row > r)
    {
        node->nextInCol = colH->first;
        colH->first = node;
    }
    else
    {
        Node *t = colH->first;
        while (t->nextInCol && t->nextInCol->row < r)
            t = t->nextInCol;
        node->nextInCol = t->nextInCol;
        t->nextInCol = node;
    }
}

// ── Consultar ─────────────────────────────────────────────────────────────────

string SparseMatrix::getCell(int r, int c)
{
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return "";
    Node *n = rh->first;
    while (n)
    {
        if (n->col == c)
            return n->value;
        n = n->nextInRow;
    }
    return "";
}

// ── Modificar (sin crear nodo) ────────────────────────────────────────────────

void SparseMatrix::modifyCell(int r, int c, string val)
{
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return;
    Node *n = rh->first;
    while (n)
    {
        if (n->col == c)
        {
            n->value = val;
            return;
        }
        n = n->nextInRow;
    }
}

// ── Eliminar celda ────────────────────────────────────────────────────────────

void SparseMatrix::deleteCell(int r, int c)
{
    // Desconectar de fila
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return;

    Node *cur = rh->first, *prev = nullptr;
    while (cur && cur->col != c)
    {
        prev = cur;
        cur = cur->nextInRow;
    }
    if (!cur)
        return;

    if (prev)
        prev->nextInRow = cur->nextInRow;
    else
        rh->first = cur->nextInRow;

    // Desconectar de columna
    Header *ch = colHeaders;
    while (ch && ch->index != c)
        ch = ch->next;
    if (ch)
    {
        Node *cc = ch->first, *pc = nullptr;
        while (cc && cc != cur)
        {
            pc = cc;
            cc = cc->nextInCol;
        }
        if (cc)
        {
            if (pc)
                pc->nextInCol = cc->nextInCol;
            else
                ch->first = cc->nextInCol;
        }
    }
    delete cur;
}

// ── Eliminar fila ─────────────────────────────────────────────────────────────

void SparseMatrix::deleteRow(int r)
{
    Header *rh = rowHeaders, *prev = nullptr;
    while (rh && rh->index != r)
    {
        prev = rh;
        rh = rh->next;
    }
    if (!rh)
        return;

    while (rh->first)
        deleteCell(r, rh->first->col);

    if (prev)
        prev->next = rh->next;
    else
        rowHeaders = rh->next;
    delete rh;
}

// ── Eliminar columna ──────────────────────────────────────────────────────────

void SparseMatrix::deleteColumn(int c)
{
    Header *ch = colHeaders, *prev = nullptr;
    while (ch && ch->index != c)
    {
        prev = ch;
        ch = ch->next;
    }
    if (!ch)
        return;

    while (ch->first)
        deleteCell(ch->first->row, c);

    if (prev)
        prev->next = ch->next;
    else
        colHeaders = ch->next;
    delete ch;
}

// ── Eliminar rango ────────────────────────────────────────────────────────────

void SparseMatrix::deleteRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2)
        swap(r1, r2);
    if (c1 > c2)
        swap(c1, c2);

    Header *rh = rowHeaders;
    while (rh)
    {
        if (rh->index >= r1 && rh->index <= r2)
        {
            vector<int> cols;
            Node *n = rh->first;
            while (n)
            {
                if (n->col >= c1 && n->col <= c2)
                    cols.push_back(n->col);
                n = n->nextInRow;
            }
            for (int col : cols)
                deleteCell(rh->index, col);
        }
        rh = rh->next;
    }
}

// ── Agregación: fila ──────────────────────────────────────────────────────────

double SparseMatrix::sumRow(int r)
{
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return 0.0;
    double s = 0.0;
    for (Node *n = rh->first; n; n = n->nextInRow)
        if (isNumeric(n->value))
            s += toDouble(n->value);
    return s;
}

double SparseMatrix::averageRow(int r)
{
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return 0.0;
    double s = 0.0;
    int cnt = 0;
    for (Node *n = rh->first; n; n = n->nextInRow)
        if (isNumeric(n->value))
        {
            s += toDouble(n->value);
            cnt++;
        }
    return cnt ? s / cnt : 0.0;
}

double SparseMatrix::findMaxRow(int r)
{
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return 0.0;
    double mx = -DBL_MAX;
    bool found = false;
    for (Node *n = rh->first; n; n = n->nextInRow)
        if (isNumeric(n->value))
        {
            double v = toDouble(n->value);
            if (!found || v > mx)
            {
                mx = v;
                found = true;
            }
        }
    return found ? mx : 0.0;
}

double SparseMatrix::findMinRow(int r)
{
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return 0.0;
    double mn = DBL_MAX;
    bool found = false;
    for (Node *n = rh->first; n; n = n->nextInRow)
        if (isNumeric(n->value))
        {
            double v = toDouble(n->value);
            if (!found || v < mn)
            {
                mn = v;
                found = true;
            }
        }
    return found ? mn : 0.0;
}

// ── Agregación: columna ───────────────────────────────────────────────────────

double SparseMatrix::sumColumn(int c)
{
    Header *ch = colHeaders;
    while (ch && ch->index != c)
        ch = ch->next;
    if (!ch)
        return 0.0;
    double s = 0.0;
    for (Node *n = ch->first; n; n = n->nextInCol)
        if (isNumeric(n->value))
            s += toDouble(n->value);
    return s;
}

double SparseMatrix::averageColumn(int c)
{
    Header *ch = colHeaders;
    while (ch && ch->index != c)
        ch = ch->next;
    if (!ch)
        return 0.0;
    double s = 0.0;
    int cnt = 0;
    for (Node *n = ch->first; n; n = n->nextInCol)
        if (isNumeric(n->value))
        {
            s += toDouble(n->value);
            cnt++;
        }
    return cnt ? s / cnt : 0.0;
}

double SparseMatrix::findMaxColumn(int c)
{
    Header *ch = colHeaders;
    while (ch && ch->index != c)
        ch = ch->next;
    if (!ch)
        return 0.0;
    double mx = -DBL_MAX;
    bool found = false;
    for (Node *n = ch->first; n; n = n->nextInCol)
        if (isNumeric(n->value))
        {
            double v = toDouble(n->value);
            if (!found || v > mx)
            {
                mx = v;
                found = true;
            }
        }
    return found ? mx : 0.0;
}

double SparseMatrix::findMinColumn(int c)
{
    Header *ch = colHeaders;
    while (ch && ch->index != c)
        ch = ch->next;
    if (!ch)
        return 0.0;
    double mn = DBL_MAX;
    bool found = false;
    for (Node *n = ch->first; n; n = n->nextInCol)
        if (isNumeric(n->value))
        {
            double v = toDouble(n->value);
            if (!found || v < mn)
            {
                mn = v;
                found = true;
            }
        }
    return found ? mn : 0.0;
}

// ── Agregación: rango ─────────────────────────────────────────────────────────

double SparseMatrix::sumRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2)
        swap(r1, r2);
    if (c1 > c2)
        swap(c1, c2);
    double s = 0.0;
    for (Header *rh = rowHeaders; rh; rh = rh->next)
        if (rh->index >= r1 && rh->index <= r2)
            for (Node *n = rh->first; n; n = n->nextInRow)
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value))
                    s += toDouble(n->value);
    return s;
}

double SparseMatrix::averageRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2)
        swap(r1, r2);
    if (c1 > c2)
        swap(c1, c2);
    double s = 0.0;
    int cnt = 0;
    for (Header *rh = rowHeaders; rh; rh = rh->next)
        if (rh->index >= r1 && rh->index <= r2)
            for (Node *n = rh->first; n; n = n->nextInRow)
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value))
                {
                    s += toDouble(n->value);
                    cnt++;
                }
    return cnt ? s / cnt : 0.0;
}

double SparseMatrix::maxRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2)
        swap(r1, r2);
    if (c1 > c2)
        swap(c1, c2);
    double mx = -DBL_MAX;
    bool found = false;
    for (Header *rh = rowHeaders; rh; rh = rh->next)
        if (rh->index >= r1 && rh->index <= r2)
            for (Node *n = rh->first; n; n = n->nextInRow)
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value))
                {
                    double v = toDouble(n->value);
                    if (!found || v > mx)
                    {
                        mx = v;
                        found = true;
                    }
                }
    return found ? mx : 0.0;
}

double SparseMatrix::minRange(int r1, int c1, int r2, int c2)
{
    if (r1 > r2)
        swap(r1, r2);
    if (c1 > c2)
        swap(c1, c2);
    double mn = DBL_MAX;
    bool found = false;
    for (Header *rh = rowHeaders; rh; rh = rh->next)
        if (rh->index >= r1 && rh->index <= r2)
            for (Node *n = rh->first; n; n = n->nextInRow)
                if (n->col >= c1 && n->col <= c2 && isNumeric(n->value))
                {
                    double v = toDouble(n->value);
                    if (!found || v < mn)
                    {
                        mn = v;
                        found = true;
                    }
                }
    return found ? mn : 0.0;
}

// ── Visualización ─────────────────────────────────────────────────────────────

void SparseMatrix::printOccupiedCells()
{
    for (Header *rh = rowHeaders; rh; rh = rh->next)
        for (Node *n = rh->first; n; n = n->nextInRow)
            cout << "(" << n->row << "," << n->col << ") " << n->value << "\n";
}

void SparseMatrix::printRow(int r)
{
    Header *rh = rowHeaders;
    while (rh && rh->index != r)
        rh = rh->next;
    if (!rh)
        return;
    for (Node *n = rh->first; n; n = n->nextInRow)
        cout << "(" << n->row << "," << n->col << ") " << n->value << "\n";
}

void SparseMatrix::printColumn(int c)
{
    Header *ch = colHeaders;
    while (ch && ch->index != c)
        ch = ch->next;
    if (!ch)
        return;
    for (Node *n = ch->first; n; n = n->nextInCol)
        cout << "(" << n->row << "," << n->col << ") " << n->value << "\n";
}

vector<Node *> SparseMatrix::getAllNodes()
{
    vector<Node *> nodes;
    for (Header *rh = rowHeaders; rh; rh = rh->next)
        for (Node *n = rh->first; n; n = n->nextInRow)
            nodes.push_back(n);
    return nodes;
}