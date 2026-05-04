#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QList>
#include <QPair>
#include "../include/SparseMatrix.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onInsertar();
    void onModificar();
    void onEliminarCelda();
    void onConsultar();
    void onIr();

    void onOpSeleccion();   // maneja Suma/Prom/Máx/Mín según combo
    void onBorrarRango();

    void onEliminarFila();
    void onEliminarColumna();
    void onVerFila();
    void onVerColumna();

    void onLimpiarTodo();

    void onCellChanged(int row, int col);
    void onCellClicked(int row, int col);

private:
    void    traducirCoordenada(const QString &coord, int &r, int &c);
    QString coordToLabel(int row, int col);
    void    refreshGrid();
    void    expandIfNeeded(int row, int col);
    void    log(const QString &msg);

    QList<QPair<int,int>> resolverRango(const QString &texto);
    double operarSobreCeldas(const QList<QPair<int,int>> &celdas, int op, int &count);

    QTableWidget *table;
    QLineEdit    *edtCelda;
    QLineEdit    *edtValor;
    QLineEdit    *edtRango;
    QComboBox    *cmbOpSel;
    QLabel       *lblStatus;

    SparseMatrix sheet;
    bool         ignorarCambios = false;

    int tableRows = 50;
    int tableCols = 26;

    static constexpr int EXPAND_ROWS = 50;
    static constexpr int EXPAND_COLS = 10;
};

#endif