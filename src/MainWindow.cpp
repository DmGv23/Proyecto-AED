#include "MainWindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QFont>
#include <QString>
#include <QStringList>
#include <sstream>
#include <iomanip>

// ── utilidad: col 0→A, 1→B … ──────────────────
static QString colLetter(int c)
{
    QString s;
    c++;  // 1-based
    while (c > 0) {
        s.prepend(QChar('A' + (c - 1) % 26));
        c = (c - 1) / 26;
    }
    return s;
}

QString MainWindow::coordToLabel(int row, int col)
{
    return colLetter(col) + QString::number(row + 1);
}

// constructor 
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , sheet(new SparseMatrix())
{
    setWindowTitle("Hoja de Cálculo — Matriz Dispersa");
    resize(1100, 700);

    // ── Central widget ───────────────────────
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout* mainLay = new QHBoxLayout(central);

    // ── Panel izquierdo: tabla ───────────────
    table = new QTableWidget(ROWS, COLS, this);
    table->horizontalHeader()->setDefaultSectionSize(60);
    table->verticalHeader()->setDefaultSectionSize(24);

    QStringList hLabels, vLabels;
    for (int c = 0; c < COLS; c++) hLabels << colLetter(c);
    for (int r = 0; r < ROWS; r++) vLabels << QString::number(r + 1);
    table->setHorizontalHeaderLabels(hLabels);
    table->setVerticalHeaderLabels(vLabels);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(table, &QTableWidget::cellClicked, this, &MainWindow::onCellClicked);

    // ── Panel derecho: controles ─────────────
    QWidget* rightPanel = new QWidget(this);
    rightPanel->setFixedWidth(320);
    QVBoxLayout* rLay = new QVBoxLayout(rightPanel);

    // -- Grupo: celda seleccionada ------------
    QGroupBox* gbCell = new QGroupBox("Celda seleccionada");
    QGridLayout* gCell = new QGridLayout(gbCell);

    gCell->addWidget(new QLabel("Fila (1-based):"), 0, 0);
    spnRow = new QSpinBox(); spnRow->setRange(1, ROWS); spnRow->setValue(1);
    gCell->addWidget(spnRow, 0, 1);

    gCell->addWidget(new QLabel("Columna (1-based):"), 1, 0);
    spnCol = new QSpinBox(); spnCol->setRange(1, COLS); spnCol->setValue(1);
    gCell->addWidget(spnCol, 1, 1);

    gCell->addWidget(new QLabel("Valor:"), 2, 0);
    edtValue = new QLineEdit();
    gCell->addWidget(edtValue, 2, 1);

    QPushButton* btnInsert = new QPushButton("Insertar / Actualizar");
    QPushButton* btnDelete = new QPushButton("Eliminar celda");
    QPushButton* btnQuery  = new QPushButton("Consultar celda");
    gCell->addWidget(btnInsert, 3, 0, 1, 2);
    gCell->addWidget(btnDelete, 4, 0, 1, 2);
    gCell->addWidget(btnQuery,  5, 0, 1, 2);
    rLay->addWidget(gbCell);

    // -- Grupo: fila / columna ----------------
    QGroupBox* gbRC = new QGroupBox("Fila / Columna");
    QGridLayout* gRC = new QGridLayout(gbRC);

    QPushButton* btnDelRow = new QPushButton("Eliminar fila");
    QPushButton* btnDelCol = new QPushButton("Eliminar columna");
    gRC->addWidget(btnDelRow, 0, 0);
    gRC->addWidget(btnDelCol, 0, 1);
    rLay->addWidget(gbRC);

    // -- Grupo: rango -------------------------
    QGroupBox* gbRange = new QGroupBox("Rango (esquina opuesta)");
    QGridLayout* gRange = new QGridLayout(gbRange);

    gRange->addWidget(new QLabel("Fila 2:"), 0, 0);
    spnRow2 = new QSpinBox(); spnRow2->setRange(1, ROWS); spnRow2->setValue(1);
    gRange->addWidget(spnRow2, 0, 1);

    gRange->addWidget(new QLabel("Col 2:"), 1, 0);
    spnCol2 = new QSpinBox(); spnCol2->setRange(1, COLS); spnCol2->setValue(1);
    gRange->addWidget(spnCol2, 1, 1);

    QPushButton* btnDelRange = new QPushButton("Eliminar rango");
    gRange->addWidget(btnDelRange, 2, 0, 1, 2);
    rLay->addWidget(gbRange);

    // -- Grupo: agregación --------------------
    QGroupBox* gbAggr = new QGroupBox("Agregación");
    QGridLayout* gAggr = new QGridLayout(gbAggr);

    cmbAggr = new QComboBox();
    cmbAggr->addItems({"SUMA", "PROMEDIO", "MÁXIMO", "MÍNIMO"});
    gAggr->addWidget(new QLabel("Operación:"), 0, 0);
    gAggr->addWidget(cmbAggr, 0, 1);

    cmbAggrTarget = new QComboBox();
    cmbAggrTarget->addItems({"Fila (usar fila 1)", "Columna (usar col 1)", "Rango (usar rango)"});
    gAggr->addWidget(new QLabel("Sobre:"), 1, 0);
    gAggr->addWidget(cmbAggrTarget, 1, 1);

    QPushButton* btnAggr = new QPushButton("Calcular");
    gAggr->addWidget(btnAggr, 2, 0, 1, 2);

    lblResult = new QLabel("Resultado: —");
    lblResult->setStyleSheet("font-weight: bold; color: #1a6e1a;");
    gAggr->addWidget(lblResult, 3, 0, 1, 2);
    rLay->addWidget(gbAggr);

    // -- Grupo: visualización -----------------
    QGroupBox* gbVis = new QGroupBox("Visualización");
    QVBoxLayout* vVis = new QVBoxLayout(gbVis);
    QPushButton* btnShowAll = new QPushButton("Mostrar celdas ocupadas");
    vVis->addWidget(btnShowAll);
    rLay->addWidget(gbVis);

    // -- Log ----------------------------------
    QGroupBox* gbLog = new QGroupBox("Log");
    QVBoxLayout* vLog = new QVBoxLayout(gbLog);
    logArea = new QTextEdit();
    logArea->setReadOnly(true);
    logArea->setFixedHeight(120);
    logArea->setFont(QFont("Courier", 9));
    vLog->addWidget(logArea);
    rLay->addWidget(gbLog);
    rLay->addStretch();

    // ── Ensamblar layout ─────────────────────
    mainLay->addWidget(table, 1);
    mainLay->addWidget(rightPanel, 0);

    // ── Conectar señales ─────────────────────
    connect(btnInsert,   &QPushButton::clicked, this, &MainWindow::onInsertCell);
    connect(btnDelete,   &QPushButton::clicked, this, &MainWindow::onDeleteCell);
    connect(btnQuery,    &QPushButton::clicked, this, &MainWindow::onQueryCell);
    connect(btnDelRow,   &QPushButton::clicked, this, &MainWindow::onDeleteRow);
    connect(btnDelCol,   &QPushButton::clicked, this, &MainWindow::onDeleteColumn);
    connect(btnDelRange, &QPushButton::clicked, this, &MainWindow::onDeleteRange);
    connect(btnAggr,     &QPushButton::clicked, this, &MainWindow::onAggregate);
    connect(btnShowAll,  &QPushButton::clicked, this, &MainWindow::onShowAll);

    // ── Datos de ejemplo ─────────────────────
    sheet->insertCell(1, 1, "42");
    sheet->insertCell(1, 2, "58");
    sheet->insertCell(1, 3, "Hola");
    sheet->insertCell(2, 1, "15");
    sheet->insertCell(2, 2, "30");
    sheet->insertCell(3, 1, "100");
    refreshGrid();
    log("Hoja inicializada con datos de ejemplo.");
}

MainWindow::~MainWindow() { delete sheet; }

// helpers
void MainWindow::refreshGrid()
{
    table->clearContents();
    // resetear colores
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
            QTableWidgetItem* item = new QTableWidgetItem("");
            item->setBackground(Qt::white);
            table->setItem(r, c, item);
        }

    auto nodes = sheet->getAllNodes();
    for (Node* n : nodes) {
        if (n->row < ROWS && n->col < COLS) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(n->value));
            item->setBackground(QColor(200, 230, 255));
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(n->row, n->col, item);
        }
    }
}

void MainWindow::log(const QString& msg) {
    logArea->append(msg);
}

// slots

void MainWindow::onCellClicked(int row, int col) {
    spnRow->setValue(row + 1);
    spnCol->setValue(col + 1);
    QString val = QString::fromStdString(sheet->getCell(row, col));
    edtValue->setText(val);
}

void MainWindow::onInsertCell() {
    int r = spnRow->value() - 1;
    int c = spnCol->value() - 1;
    QString val = edtValue->text().trimmed();
    if (val.isEmpty()) { QMessageBox::warning(this, "Error", "Ingrese un valor."); return; }
    sheet->insertCell(r, c, val.toStdString());
    refreshGrid();
    log(QString("Insertado %1 = \"%2\"").arg(coordToLabel(r, c)).arg(val));
}

void MainWindow::onDeleteCell() {
    int r = spnRow->value() - 1;
    int c = spnCol->value() - 1;
    sheet->deleteCell(r, c);
    refreshGrid();
    log(QString("Eliminada celda %1").arg(coordToLabel(r, c)));
}

void MainWindow::onQueryCell() {
    int r = spnRow->value() - 1;
    int c = spnCol->value() - 1;
    string val = sheet->getCell(r, c);
    if (val.empty())
        log(QString("Consulta %1: (vacía)").arg(coordToLabel(r, c)));
    else
        log(QString("Consulta %1 = \"%2\"").arg(coordToLabel(r, c)).arg(QString::fromStdString(val)));
}

void MainWindow::onDeleteRow() {
    int r = spnRow->value() - 1;
    sheet->deleteRow(r);
    refreshGrid();
    log(QString("Eliminada fila %1").arg(r + 1));
}

void MainWindow::onDeleteColumn() {
    int c = spnCol->value() - 1;
    sheet->deleteColumn(c);
    refreshGrid();
    log(QString("Eliminada columna %1").arg(colLetter(c)));
}

void MainWindow::onDeleteRange() {
    int r1 = spnRow->value() - 1,  c1 = spnCol->value() - 1;
    int r2 = spnRow2->value() - 1, c2 = spnCol2->value() - 1;
    sheet->deleteRange(r1, c1, r2, c2);
    refreshGrid();
    log(QString("Eliminado rango %1:%2")
        .arg(coordToLabel(r1, c1))
        .arg(coordToLabel(r2, c2)));
}

void MainWindow::onAggregate() {
    int op     = cmbAggr->currentIndex();       // 0=SUMA 1=PROM 2=MAX 3=MIN
    int target = cmbAggrTarget->currentIndex(); // 0=fila 1=col 2=rango
    int r1 = spnRow->value() - 1,  c1 = spnCol->value() - 1;
    int r2 = spnRow2->value() - 1, c2 = spnCol2->value() - 1;

    double result = 0.0;
    QString desc;

    if (target == 0) {
        // fila
        switch (op) {
            case 0: result = sheet->sumRow(r1);     desc = QString("SUMA fila %1").arg(r1+1);     break;
            case 1: result = sheet->averageRow(r1); desc = QString("PROM fila %1").arg(r1+1);     break;
            case 2: result = sheet->findMaxRow(r1); desc = QString("MAX fila %1").arg(r1+1);      break;
            case 3: result = sheet->findMinRow(r1); desc = QString("MIN fila %1").arg(r1+1);      break;
        }
    } else if (target == 1) {
        // columna
        switch (op) {
            case 0: result = sheet->sumColumn(c1);     desc = QString("SUMA col %1").arg(colLetter(c1));  break;
            case 1: result = sheet->averageColumn(c1); desc = QString("PROM col %1").arg(colLetter(c1));  break;
            case 2: result = sheet->findMaxColumn(c1); desc = QString("MAX col %1").arg(colLetter(c1));   break;
            case 3: result = sheet->findMinColumn(c1); desc = QString("MIN col %1").arg(colLetter(c1));   break;
        }
    } else {
        // rango
        switch (op) {
            case 0: result = sheet->sumRange(r1,c1,r2,c2);     desc = QString("SUMA rango");     break;
            case 1: result = sheet->averageRange(r1,c1,r2,c2); desc = QString("PROM rango");     break;
            case 2: result = sheet->maxRange(r1,c1,r2,c2);     desc = QString("MAX rango");      break;
            case 3: result = sheet->minRange(r1,c1,r2,c2);     desc = QString("MIN rango");      break;
        }
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << result;
    QString res = QString::fromStdString(oss.str());
    lblResult->setText(QString("Resultado: %1").arg(res));
    log(QString("%1 = %2").arg(desc).arg(res));
}

void MainWindow::onShowAll() {
    auto nodes = sheet->getAllNodes();
    if (nodes.empty()) { log("No hay celdas ocupadas."); return; }
    log("── Celdas ocupadas ──");
    for (Node* n : nodes) {
        log(QString("  %1 = \"%2\"")
            .arg(coordToLabel(n->row, n->col))
            .arg(QString::fromStdString(n->value)));
    }
    log(QString("Total: %1 celda(s)").arg(nodes.size()));
}
