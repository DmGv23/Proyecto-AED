#include "mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QMessageBox>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <QStatusBar>

// ── Utilidad: 0→A, 1→B, 25→Z, 26→AA … ───────────────────────────────────────
static QString colLetter(int c)
{
    QString s; c++;
    while (c > 0) { s.prepend(QChar('A' + (c-1) % 26)); c = (c-1) / 26; }
    return s;
}

QString MainWindow::coordToLabel(int row, int col)
{
    return colLetter(col) + QString::number(row + 1);
}

// ── Helper: botón de toolbar uniforme ────────────────────────────────────────
static QPushButton* makeBtn(const QString &text, const QString &tooltip = "")
{
    QPushButton *b = new QPushButton(text);
    b->setFixedHeight(32);
    b->setMinimumWidth(80);
    b->setFlat(false);
    if (!tooltip.isEmpty()) b->setToolTip(tooltip);
    return b;
}

// ── Etiqueta de sección en la barra ──────────────────────────────────────────
static QLabel* makeSecLabel(const QString &text)
{
    QLabel *l = new QLabel(text);
    l->setStyleSheet("color: palette(mid); font-size: 9px; font-weight: bold; text-transform: uppercase; letter-spacing: 1px;");
    l->setAlignment(Qt::AlignCenter);
    return l;
}

// ── Grupo visual: etiqueta arriba + widgets abajo ─────────────────────────────
// Devuelve un widget contenedor con label en top y un HBox de hijos
static QWidget* makeGroup(const QString &label, QList<QWidget*> widgets, int spacing = 4)
{
    QWidget *grp = new QWidget();
    QVBoxLayout *v = new QVBoxLayout(grp);
    v->setContentsMargins(6, 2, 6, 2);
    v->setSpacing(2);

    QLabel *lbl = makeSecLabel(label);
    v->addWidget(lbl);

    QWidget *row = new QWidget();
    QHBoxLayout *h = new QHBoxLayout(row);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(spacing);
    for (QWidget *w : widgets) h->addWidget(w);
    v->addWidget(row);
    return grp;
}

// ── Separador vertical ────────────────────────────────────────────────────────
static QFrame* makeSep()
{
    QFrame *f = new QFrame();
    f->setFrameShape(QFrame::VLine);
    f->setFrameShadow(QFrame::Sunken);
    f->setFixedWidth(8);
    return f;
}

// ── Constructor ───────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Hoja de Cálculo — Matriz Dispersa");
    resize(1280, 740);

    // ── Central widget ────────────────────────────────────────────────────────
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *vMain = new QVBoxLayout(central);
    vMain->setContentsMargins(0, 0, 0, 0);
    vMain->setSpacing(0);

    // ═══════════════════════════════════════════════════════════════════════
    // BARRA SUPERIOR — estilo Excel con grupos etiquetados
    // ═══════════════════════════════════════════════════════════════════════
    QWidget *bar = new QWidget(this);
    bar->setFixedHeight(70);   // más alta para respirar
    bar->setObjectName("excelBar");
    QHBoxLayout *hBar = new QHBoxLayout(bar);
    hBar->setContentsMargins(8, 4, 8, 4);
    hBar->setSpacing(3);

    // ── Grupo CELDA ───────────────────────────────────────────────────────
    edtCelda = new QLineEdit(); edtCelda->setFixedWidth(62); edtCelda->setFixedHeight(32);
    edtCelda->setPlaceholderText("A1");

    edtValor = new QLineEdit(); edtValor->setFixedWidth(200); edtValor->setFixedHeight(32);
    edtValor->setPlaceholderText("texto o número...");

    QPushButton *btnIr        = makeBtn("Ir",          "Navegar a la celda");
    QPushButton *btnInsertar  = makeBtn("Insertar",    "Insertar / actualizar celda");
    QPushButton *btnModif     = makeBtn("Modificar",   "Modificar celda sin crear nodo nuevo");
    QPushButton *btnElimC     = makeBtn("Elim. celda", "Eliminar celda");
    QPushButton *btnConsultar = makeBtn("Consultar",   "Consultar valor de celda");

    hBar->addWidget(makeGroup("Celda",
                              {edtCelda, edtValor, btnIr, btnInsertar, btnModif, btnElimC, btnConsultar}));

    hBar->addWidget(makeSep());

    // ── Grupo RANGO ───────────────────────────────────────────────────────
    edtRango = new QLineEdit(); edtRango->setFixedWidth(110); edtRango->setFixedHeight(32);
    edtRango->setPlaceholderText("A1:C3  o  A1,C3,E5");

    // Un solo combo + botón de operación (elimina los 4 botones separados)
    cmbOpSel = new QComboBox(); cmbOpSel->setFixedHeight(32); cmbOpSel->setFixedWidth(100);
    cmbOpSel->addItems({"Suma", "Promedio", "Máximo", "Mínimo"});
    cmbOpSel->setToolTip("Operación a aplicar sobre el rango o selección");

    QPushButton *btnOperar = makeBtn("Calcular →", "Aplica la operación al rango o a la selección con Ctrl+Click\nEl resultado se guarda en la celda destino");
    btnOperar->setMinimumWidth(100);

    QPushButton *btnBorrR = makeBtn("Borrar rango", "Eliminar celdas del rango");

    hBar->addWidget(makeGroup("Rango / Operación",
                              {edtRango, cmbOpSel, btnOperar, btnBorrR}));

    hBar->addWidget(makeSep());

    // ── Grupo FILA / COLUMNA ──────────────────────────────────────────────
    QPushButton *btnElimFila = makeBtn("Elim. fila",  "Eliminar toda la fila");
    QPushButton *btnElimCol  = makeBtn("Elim. col.",  "Eliminar toda la columna");
    QPushButton *btnVerFila  = makeBtn("Ver fila",    "Mostrar valores de la fila en el log");
    QPushButton *btnVerCol   = makeBtn("Ver col.",    "Mostrar valores de la columna en el log");

    hBar->addWidget(makeGroup("Fila / Columna",
                              {btnElimFila, btnElimCol, btnVerFila, btnVerCol}));

    hBar->addWidget(makeSep());

    // ── Grupo HOJA ────────────────────────────────────────────────────────
    QPushButton *btnLimpiar = makeBtn("Limpiar todo", "Vaciar toda la hoja");
    btnLimpiar->setStyleSheet("color: #c0392b; min-width: 100px;");

    hBar->addWidget(makeGroup("Hoja", {btnLimpiar}));

    hBar->addStretch();
    vMain->addWidget(bar);

    // ── Línea separadora ──────────────────────────────────────────────────
    QFrame *line = new QFrame(); line->setFrameShape(QFrame::HLine);
    vMain->addWidget(line);

    // ═══════════════════════════════════════════════════════════════════════
    // TABLA — hoja de cálculo dinámica
    // ═══════════════════════════════════════════════════════════════════════
    table = new QTableWidget(tableRows, tableCols, this);
    table->horizontalHeader()->setDefaultSectionSize(70);
    table->verticalHeader()->setDefaultSectionSize(24);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection); // Ctrl+Click
    table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);

    // Cabeceras
    QStringList hH, vH;
    for (int c = 0; c < tableCols; ++c) hH << colLetter(c);
    for (int r = 0; r < tableRows; ++r) vH << QString::number(r + 1);
    table->setHorizontalHeaderLabels(hH);
    table->setVerticalHeaderLabels(vH);

    vMain->addWidget(table, 1);

    // ═══════════════════════════════════════════════════════════════════════
    // STATUS BAR — log de última operación
    // ═══════════════════════════════════════════════════════════════════════
    lblStatus = new QLabel("Listo.  |  Doble clic o tecla para editar  |  Ctrl+Click = selección múltiple  |  La hoja crece al escribir en el borde");
    lblStatus->setContentsMargins(8, 2, 8, 2);
    statusBar()->addWidget(lblStatus, 1);

    // ── Conexiones ────────────────────────────────────────────────────────
    connect(btnIr,        &QPushButton::clicked, this, &MainWindow::onIr);
    connect(btnInsertar,  &QPushButton::clicked, this, &MainWindow::onInsertar);
    connect(btnModif,     &QPushButton::clicked, this, &MainWindow::onModificar);
    connect(btnElimC,     &QPushButton::clicked, this, &MainWindow::onEliminarCelda);
    connect(btnConsultar, &QPushButton::clicked, this, &MainWindow::onConsultar);
    connect(btnOperar,    &QPushButton::clicked, this, &MainWindow::onOpSeleccion);
    connect(btnBorrR,     &QPushButton::clicked, this, &MainWindow::onBorrarRango);
    connect(btnElimFila,  &QPushButton::clicked, this, &MainWindow::onEliminarFila);
    connect(btnElimCol,   &QPushButton::clicked, this, &MainWindow::onEliminarColumna);
    connect(btnVerFila,   &QPushButton::clicked, this, &MainWindow::onVerFila);
    connect(btnVerCol,    &QPushButton::clicked, this, &MainWindow::onVerColumna);
    connect(btnLimpiar,   &QPushButton::clicked, this, &MainWindow::onLimpiarTodo);

    connect(table, &QTableWidget::cellClicked,  this, &MainWindow::onCellClicked);
    connect(table, &QTableWidget::cellChanged,  this, &MainWindow::onCellChanged);

    // ── Datos de ejemplo ──────────────────────────────────────────────────
    sheet.insertCell(1,1,"42");  sheet.insertCell(1,2,"58");
    sheet.insertCell(1,3,"Hola");sheet.insertCell(2,1,"15");
    sheet.insertCell(2,2,"30"); sheet.insertCell(3,1,"100");
    refreshGrid();
    log("Hoja lista — la hoja crece automáticamente al escribir en el borde.");
}

MainWindow::~MainWindow() {}

// ── Helpers ───────────────────────────────────────────────────────────────────

void MainWindow::traducirCoordenada(const QString &coord, int &r, int &c)
{
    r = -1; c = -1;
    QString s = coord.trimmed().toUpper();
    if (s.isEmpty()) return;
    if (!s.at(0).isLetter()) { r = s.toInt() - 1; return; }
    int i = 0; c = 0;
    while (i < s.size() && s.at(i).isLetter()) {
        c = c * 26 + (s.at(i).unicode() - 'A' + 1); ++i;
    }
    c--; r = s.mid(i).toInt() - 1;
}

// Expande la tabla si el usuario escribe cerca del borde
void MainWindow::expandIfNeeded(int row, int col)
{
    bool expanded = false;

    if (row >= tableRows - 3) {
        tableRows += EXPAND_ROWS;
        table->setRowCount(tableRows);
        QStringList vH;
        for (int r = 0; r < tableRows; ++r) vH << QString::number(r + 1);
        table->setVerticalHeaderLabels(vH);
        expanded = true;
    }
    if (col >= tableCols - 2) {
        tableCols += EXPAND_COLS;
        table->setColumnCount(tableCols);
        QStringList hH;
        for (int c = 0; c < tableCols; ++c) hH << colLetter(c);
        table->setHorizontalHeaderLabels(hH);
        expanded = true;
    }
    if (expanded)
        log(QString("Hoja expandida a %1 filas × %2 columnas").arg(tableRows).arg(tableCols));
}

void MainWindow::refreshGrid()
{
    ignorarCambios = true;
    table->clearContents();
    for (Node *n : sheet.getAllNodes()) {
        expandIfNeeded(n->row, n->col);
        if (n->row < tableRows && n->col < tableCols) {
            QTableWidgetItem *item = new QTableWidgetItem(
                QString::fromStdString(n->value));
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(n->row, n->col, item);
        }
    }
    ignorarCambios = false;
}

void MainWindow::log(const QString &msg) { lblStatus->setText(msg); }

// Parsea "A1:C3" o "A1,C3,E5" → lista de (row,col) 0-based
QList<QPair<int,int>> MainWindow::resolverRango(const QString &texto)
{
    QList<QPair<int,int>> lista;
    QString t = texto.trimmed().toUpper();
    if (t.isEmpty()) return lista;

    if (t.contains(':') && !t.contains(',')) {
        // Rectangular
        QStringList p = t.split(':');
        if (p.size() != 2) return lista;
        int r1,c1,r2,c2;
        traducirCoordenada(p[0],r1,c1); traducirCoordenada(p[1],r2,c2);
        if (r1<0||c1<0||r2<0||c2<0) return lista;
        if (r1>r2) std::swap(r1,r2); if (c1>c2) std::swap(c1,c2);
        for (int r=r1;r<=r2;++r) for (int c=c1;c<=c2;++c) lista.append({r,c});
    } else {
        // No contiguo: "A1,C3,E5"
        for (const QString &tok : t.split(',', Qt::SkipEmptyParts)) {
            int r,c; traducirCoordenada(tok.trimmed(),r,c);
            if (r>=0 && c>=0) lista.append({r,c});
        }
    }
    return lista;
}

double MainWindow::operarSobreCeldas(const QList<QPair<int,int>> &celdas, int op, int &count)
{
    double acum=0, maxV=-DBL_MAX, minV=DBL_MAX; count=0;
    for (auto &[r,c] : celdas) {
        std::string v = sheet.getCell(r,c);
        if (v.empty()) continue;
        try {
            double d = std::stod(v);
            acum += d; if(d>maxV) maxV=d; if(d<minV) minV=d; count++;
        } catch(...) {}
    }
    if (!count) return 0.0;
    switch(op){case 0:return acum; case 1:return acum/count; case 2:return maxV; case 3:return minV;}
    return 0.0;
}

// ── Slot: edición directa en la tabla ────────────────────────────────────────
void MainWindow::onCellChanged(int row, int col)
{
    if (ignorarCambios) return;
    QTableWidgetItem *item = table->item(row, col);
    QString txt = item ? item->text().trimmed() : "";

    if (txt.isEmpty()) {
        sheet.deleteCell(row, col);
        log(QString("Celda %1 vaciada.").arg(coordToLabel(row, col)));
    } else {
        sheet.insertCell(row, col, txt.toStdString());
        if (item) item->setTextAlignment(Qt::AlignCenter);
        expandIfNeeded(row, col);
        log(QString("%1 = \"%2\"  (edición directa)").arg(coordToLabel(row, col)).arg(txt));
    }
}

// ── Slot: click en celda → rellena campos ────────────────────────────────────
void MainWindow::onCellClicked(int row, int col)
{
    edtCelda->setText(coordToLabel(row, col));
    edtValor->setText(QString::fromStdString(sheet.getCell(row, col)));

    auto sel = table->selectedItems();
    if (sel.size() > 1)
        log(QString("%1 celdas seleccionadas. Elige operación y pulsa 'Operar sel.'").arg(sel.size()));
}

// ── Operaciones de celda ──────────────────────────────────────────────────────

void MainWindow::onIr()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    expandIfNeeded(r, c);
    if (r>=0 && c>=0 && r<tableRows && c<tableCols) {
        table->setCurrentCell(r,c);
        if (!table->item(r,c)) table->setItem(r,c,new QTableWidgetItem());
        table->scrollToItem(table->item(r,c));
        log(QString("Navegando a %1").arg(coordToLabel(r,c)));
    } else { log("Coordenada inválida."); }
}

void MainWindow::onInsertar()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0||c<0) { log("Celda inválida (ej: A1)."); return; }
    QString val = edtValor->text().trimmed();
    if (val.isEmpty()) { log("Ingresa un valor."); return; }
    sheet.insertCell(r,c,val.toStdString());
    expandIfNeeded(r,c);
    refreshGrid();
    log(QString("Insertado %1 = \"%2\"").arg(coordToLabel(r,c)).arg(val));
}

void MainWindow::onModificar()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0||c<0) { log("Celda inválida."); return; }
    QString val = edtValor->text().trimmed();
    if (val.isEmpty()) { log("Ingresa un valor."); return; }
    if (sheet.getCell(r,c).empty()) { log(QString("%1 está vacía — usa Insertar.").arg(coordToLabel(r,c))); return; }
    std::string prev = sheet.getCell(r,c);
    sheet.modifyCell(r,c,val.toStdString());
    refreshGrid();
    log(QString("Modificada %1: \"%2\" → \"%3\"").arg(coordToLabel(r,c)).arg(QString::fromStdString(prev)).arg(val));
}

void MainWindow::onEliminarCelda()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0||c<0) { log("Celda inválida."); return; }
    sheet.deleteCell(r,c); refreshGrid();
    log(QString("Eliminada celda %1").arg(coordToLabel(r,c)));
}

void MainWindow::onConsultar()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0||c<0) { log("Celda inválida."); return; }
    std::string val = sheet.getCell(r,c);
    log(val.empty()
            ? QString("Consulta %1: (vacía)").arg(coordToLabel(r,c))
            : QString("Consulta %1 = \"%2\"").arg(coordToLabel(r,c)).arg(QString::fromStdString(val)));
}

// ── Operaciones de rango ──────────────────────────────────────────────────────

static std::string fmt(double v) {
    std::ostringstream o; o << std::fixed << std::setprecision(2) << v; return o.str();
}

void MainWindow::onBorrarRango()
{
    auto cel = resolverRango(edtRango->text());
    if (cel.isEmpty()) { log("Rango inválido."); return; }
    for (auto &[r,cv] : cel) sheet.deleteCell(r,cv);
    refreshGrid();
    log(QString("Borradas %1 celdas.").arg(cel.size()));
}

// ── Fila / Columna ────────────────────────────────────────────────────────────

void MainWindow::onEliminarFila()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0) { log("Fila inválida."); return; }
    sheet.deleteRow(r); refreshGrid();
    log(QString("Eliminada fila %1").arg(r+1));
}

void MainWindow::onEliminarColumna()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (c<0) { log("Columna inválida."); return; }
    sheet.deleteColumn(c); refreshGrid();
    log(QString("Eliminada columna %1").arg(colLetter(c)));
}

void MainWindow::onVerFila()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0) { log("Fila inválida."); return; }
    QString res = QString("Fila %1:").arg(r+1); bool found=false;
    for (Node *n : sheet.getAllNodes())
        if (n->row==r){ res += QString("  %1=%2").arg(coordToLabel(r,n->col)).arg(QString::fromStdString(n->value)); found=true; }
    log(found ? res : res+"  (vacía)");
}

void MainWindow::onVerColumna()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (c<0) { log("Columna inválida."); return; }
    QString res = QString("Col %1:").arg(colLetter(c)); bool found=false;
    for (Node *n : sheet.getAllNodes())
        if (n->col==c){ res += QString("  %1=%2").arg(coordToLabel(n->row,c)).arg(QString::fromStdString(n->value)); found=true; }
    log(found ? res : res+"  (vacía)");
}

// ── Selección no contigua ─────────────────────────────────────────────────────

void MainWindow::onOpSeleccion()
{
    int op = cmbOpSel->currentIndex();  // 0=Suma 1=Promedio 2=Máximo 3=Mínimo
    QList<QPair<int,int>> celdas;
    QStringList ops = {"SUMA","PROMEDIO","MÁXIMO","MÍNIMO"};

    // Prioridad 1: Ctrl+Click en la tabla (más de 1 celda seleccionada)
    auto selItems = table->selectedItems();
    if (selItems.size() > 1)
        for (QTableWidgetItem *it : selItems) celdas.append({it->row(), it->column()});

    // Prioridad 2: campo Rango — acepta A1:C3 (rectangular) o A1,C3,E5 (no contiguo)
    if (celdas.isEmpty() && !edtRango->text().trimmed().isEmpty())
        celdas = resolverRango(edtRango->text());

    if (celdas.isEmpty()) {
        log("Escribe un rango en el campo Rango (ej: A1:C3) o selecciona celdas con Ctrl+Click.");
        return;
    }

    int cnt; double res = operarSobreCeldas(celdas, op, cnt);
    if (!cnt) { log("Ninguna celda del rango tiene valor numérico."); return; }

    std::string s = fmt(res);

    int r,c; traducirCoordenada(edtCelda->text(), r, c);
    if (r>=0 && c>=0) {
        sheet.insertCell(r, c, s); expandIfNeeded(r, c); refreshGrid();
        log(QString("%1 [%2 celdas] = %3 → guardado en %4")
                .arg(ops[op]).arg(cnt).arg(QString::fromStdString(s)).arg(coordToLabel(r,c)));
    } else {
        log(QString("%1 [%2 celdas] = %3  (escribe una celda destino en 'Celda' para guardar el resultado)")
                .arg(ops[op]).arg(cnt).arg(QString::fromStdString(s)));
    }
}

// ── Limpiar todo ──────────────────────────────────────────────────────────────

void MainWindow::onLimpiarTodo()
{
    auto nodes = sheet.getAllNodes();
    std::vector<std::pair<int,int>> coords;
    for (Node *n : nodes) coords.push_back({n->row, n->col});
    for (auto &[r,c] : coords) sheet.deleteCell(r,c);
    ignorarCambios = true;
    table->clearContents();
    ignorarCambios = false;
    log("Hoja limpiada.");
}