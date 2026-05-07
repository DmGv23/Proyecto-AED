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
    // ── Barra superior: escritura ──────────────────────
    void onInsertar();
    void onModificar();
    void onOpSeleccion();
    void onBorrarRango();
    void onLimpiarTodo();

    // ── Barra lateral: lectura / eliminación ───────────
    void onConsultar();
    void onVerFila();
    void onVerColumna();
    void onEliminarCelda();
    void onEliminarFila();
    void onEliminarColumna();

    // ── Tabla ──────────────────────────────────────────
    void onCellChanged(int row, int col);
    void onCellClicked(int row, int col);
    void onIr();

private:
    // helpers
    void    traducirCoordenada(const QString &coord, int &r, int &c);
    QString coordToLabel(int row, int col);
    void    refreshGrid();
    void    expandIfNeeded(int row, int col);
    void    log(const QString &msg);
    void    showError(const QString &msg);   // ← nuevo: popup de error

    QList<QPair<int,int>> resolverRango(const QString &texto);

    // Devuelve false y llama showError() si el rango mezcla tipos
    bool operarSobreCeldas(const QList<QPair<int,int>> &celdas,
                           int op, double &result, int &count);

    // ── Widgets barra superior ─────────────────────────
    QTableWidget *table;
    QLineEdit    *edtCelda;
    QLineEdit    *edtValor;
    QLineEdit    *edtRango;
    QComboBox    *cmbOpSel;
    QLabel       *lblStatus;

    // ── Widgets barra lateral ──────────────────────────
    QLabel       *sideInfoLabel;   // muestra resultado de consulta en el panel

    SparseMatrix sheet;
    bool         ignorarCambios = false;

    int tableRows = 50;
    int tableCols = 26;

    static constexpr int EXPAND_ROWS = 50;
    static constexpr int EXPAND_COLS = 10;
};

#endif