#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include "Node.h"
#include "Header.h"
#include <vector>
#include <string>

class SparseMatrix
{

private:
    Header *rowHeaders; // lista de cabeceras de fila
    Header *colHeaders; // lista de cabeceras de columna

    bool isNumeric(const std::string &s) const;
    double toDouble(const std::string &s) const;

    // helpers internos para buscar/crear cabeceras
    Header *findOrCreateRow(int r);
    Header *findOrCreateCol(int c);

public:
    SparseMatrix();
    ~SparseMatrix();

    // ── Operaciones básicas ──────────────────────────────
    void insertCell(int row, int col, std::string value);
    std::string getCell(int row, int col);
    void modifyCell(int row, int col, std::string value);
    void deleteCell(int row, int col);

    // ── Filas y columnas ─────────────────────────────────
    void deleteRow(int row);
    void deleteColumn(int col);
    void deleteRange(int r1, int c1, int r2, int c2);

    // ── Agregación: fila ─────────────────────────────────
    double sumRow(int row);
    double averageRow(int row);
    double findMaxRow(int row);
    double findMinRow(int row);

    // ── Agregación: columna ──────────────────────────────
    double sumColumn(int col);
    double averageColumn(int col);
    double findMaxColumn(int col);
    double findMinColumn(int col);

    // ── Agregación: rango ────────────────────────────────
    double sumRange(int r1, int c1, int r2, int c2);
    double averageRange(int r1, int c1, int r2, int c2);
    double maxRange(int r1, int c1, int r2, int c2);
    double minRange(int r1, int c1, int r2, int c2);

    // ── Visualización ────────────────────────────────────
    void printOccupiedCells();
    void printRow(int row);
    void printColumn(int col);

    std::vector<Node *> getAllNodes();
};

#endif // SPARSEMATRIX_H