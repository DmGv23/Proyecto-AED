#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include "Node.h"
#include "RowHeader.h"
#include "ColHeader.h"
#include <vector>
#include <string>

class SparseMatrix {

private:
    RowHeader* rowHeaders;
    ColHeader* colHeaders;

    // Helper: true si string es numérico 
    bool isNumeric(const std::string& s) const;

    // Helper: convierte string a double de forma segura
    double toDouble(const std::string& s) const;

public:
    SparseMatrix();

    // ---------- Operaciones básicas de celda ----------
    // Inserta o actualiza 
    void insertCell(int row, int col, std::string value);

    std::string getCell(int row, int col);

    // Modificar celda existente 
    void modifyCell(int row, int col, std::string value);

    void deleteCell(int row, int col);

    // ---------- Operaciones sobre filas/columnas ----------
    void deleteRow(int row);

    void deleteColumn(int col);

    // Eliminar rango rectangular
    void deleteRange(int r1, int c1, int r2, int c2);

    // ---------- Agregación: fila ----------
    double sumRow(int row);
    double averageRow(int row);
    double findMaxRow(int row);
    double findMinRow(int row);

    // ---------- Agregación: columna ----------
    double sumColumn(int col);
    double averageColumn(int col);
    double findMaxColumn(int col);
    double findMinColumn(int col);

    // ---------- Agregación: rango ----------
    double sumRange(int r1, int c1, int r2, int c2);
    double averageRange(int r1, int c1, int r2, int c2);
    double maxRange(int r1, int c1, int r2, int c2);
    double minRange(int r1, int c1, int r2, int c2);

    // Visualización
    void printOccupiedCells();
    void printRow(int row);
    void printColumn(int col);

    // Retorna lista de todos los nodos 
    std::vector<Node*> getAllNodes();

    ~SparseMatrix();
};

#endif