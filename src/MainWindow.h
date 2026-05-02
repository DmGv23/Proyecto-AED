#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include "../include/SparseMatrix.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // celda
    void onInsertCell();
    void onDeleteCell();
    void onQueryCell();
    // fila/col
    void onDeleteRow();
    void onDeleteColumn();
    // rango
    void onDeleteRange();
    // agregacion
    void onAggregate();
    // visualizacion
    void onShowAll();
    void onCellClicked(int row, int col);

private:
    SparseMatrix* sheet;
    static const int ROWS = 20;
    static const int COLS = 20;

    QTableWidget* table;
    QLineEdit*    edtValue;
    QSpinBox*     spnRow;
    QSpinBox*     spnCol;
    QSpinBox*     spnRow2;
    QSpinBox*     spnCol2;
    QComboBox*    cmbAggr;
    QComboBox*    cmbAggrTarget;
    QLabel*       lblResult;
    QTextEdit*    logArea;

    void refreshGrid();
    void log(const QString& msg);
    QString coordToLabel(int row, int col);
};

#endif
