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
#include <QGroupBox>
#include <QGridLayout>
#include <QStatusBar>
#include <sstream>
#include <iomanip>
#include <cfloat>

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
    l->setStyleSheet("color: palette(mid); font-size: 9px; font-weight: bold;"
                     " text-transform: uppercase; letter-spacing: 1px;");
    l->setAlignment(Qt::AlignCenter);
    return l;
}

// ── Grupo visual para la barra superior ──────────────────────────────────────
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
    resize(1380, 760);

    // ── Central widget ────────────────────────────────────────────────────────
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *vMain = new QVBoxLayout(central);
    vMain->setContentsMargins(0, 0, 0, 0);
    vMain->setSpacing(0);

    // ═══════════════════════════════════════════════════════════════════════
    // BARRA SUPERIOR — operaciones de escritura / cálculo
    // ═══════════════════════════════════════════════════════════════════════
    QWidget *bar = new QWidget(this);
    bar->setFixedHeight(70);
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

    hBar->addWidget(makeGroup("Celda",
                              {edtCelda, edtValor, btnIr, btnInsertar, btnModif}));

    hBar->addWidget(makeSep());

    // ── Grupo RANGO ───────────────────────────────────────────────────────
    edtRango = new QLineEdit(); edtRango->setFixedWidth(110); edtRango->setFixedHeight(32);
    edtRango->setPlaceholderText("A1:C3  o  A1,C3,E5");

    cmbOpSel = new QComboBox(); cmbOpSel->setFixedHeight(32); cmbOpSel->setFixedWidth(100);
    cmbOpSel->addItems({"Suma", "Promedio", "Máximo", "Mínimo"});
    cmbOpSel->setToolTip("Operación a aplicar sobre el rango");

    QPushButton *btnOperar = makeBtn("Calcular →", "Aplica la operación al rango\nEl resultado se guarda en la celda destino");
    btnOperar->setMinimumWidth(100);

    QPushButton *btnBorrR = makeBtn("Borrar rango", "Eliminar celdas del rango");

    hBar->addWidget(makeGroup("Rango / Operación",
                              {edtRango, cmbOpSel, btnOperar, btnBorrR}));

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
    // ÁREA CENTRAL — tabla + barra lateral
    // ═══════════════════════════════════════════════════════════════════════
    QWidget *contentArea = new QWidget();
    QHBoxLayout *hContent = new QHBoxLayout(contentArea);
    hContent->setContentsMargins(0, 0, 0, 0);
    hContent->setSpacing(0);

    // ── TABLA ─────────────────────────────────────────────────────────────
    table = new QTableWidget(tableRows, tableCols, this);
    table->horizontalHeader()->setDefaultSectionSize(70);
    table->verticalHeader()->setDefaultSectionSize(24);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed);

    QStringList hH, vH;
    for (int c = 0; c < tableCols; ++c) hH << colLetter(c);
    for (int r = 0; r < tableRows; ++r) vH << QString::number(r + 1);
    table->setHorizontalHeaderLabels(hH);
    table->setVerticalHeaderLabels(vH);

    hContent->addWidget(table, 1);

    // ── BARRA LATERAL — consulta / ver / eliminar ─────────────────────────
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(230);
    sidebar->setObjectName("sidebar");
    sidebar->setStyleSheet(
        "QWidget#sidebar { "
        "  background-color: palette(window); "
        "  border-left: 1px solid palette(mid); "
        "}"
        );

    QVBoxLayout *vSide = new QVBoxLayout(sidebar);
    vSide->setContentsMargins(10, 12, 10, 12);
    vSide->setSpacing(10);

    // Título del panel
    QLabel *sideTitle = new QLabel("Panel de Operaciones");
    sideTitle->setAlignment(Qt::AlignCenter);
    sideTitle->setStyleSheet(
        "font-weight: bold; font-size: 11px;"
        " color: palette(windowText);"
        " border-bottom: 1px solid palette(mid);"
        " padding-bottom: 6px;"
        );
    vSide->addWidget(sideTitle);

    // ── Grupo: Consulta ───────────────────────────────────────────────────
    QGroupBox *gbConsulta = new QGroupBox("Consulta");
    gbConsulta->setStyleSheet("QGroupBox { font-weight: bold; font-size: 10px; }");
    QVBoxLayout *vConsulta = new QVBoxLayout(gbConsulta);
    vConsulta->setSpacing(6);

    QPushButton *sbtnConsultar = new QPushButton("Consultar celda");
    sbtnConsultar->setToolTip("Muestra el valor de la celda activa");
    sbtnConsultar->setFixedHeight(30);

    sideInfoLabel = new QLabel("—");
    sideInfoLabel->setAlignment(Qt::AlignCenter);
    sideInfoLabel->setWordWrap(true);
    sideInfoLabel->setStyleSheet(
        "background: palette(base);"
        " border: 1px solid palette(mid);"
        " border-radius: 3px;"
        " padding: 4px 6px;"
        " font-family: monospace;"
        " color: #1a6e1a;"
        " font-weight: bold;"
        );
    sideInfoLabel->setMinimumHeight(32);

    vConsulta->addWidget(sbtnConsultar);
    vConsulta->addWidget(sideInfoLabel);
    vSide->addWidget(gbConsulta);

    // ── Grupo: Visualizar ─────────────────────────────────────────────────
    QGroupBox *gbVer = new QGroupBox("Visualizar");
    gbVer->setStyleSheet("QGroupBox { font-weight: bold; font-size: 10px; }");
    QVBoxLayout *vVer = new QVBoxLayout(gbVer);
    vVer->setSpacing(6);

    QPushButton *sbtnVerFila  = new QPushButton("Ver fila");
    QPushButton *sbtnVerCol   = new QPushButton("Ver columna");
    sbtnVerFila->setToolTip("Muestra en el log los valores de la fila activa");
    sbtnVerCol->setToolTip("Muestra en el log los valores de la columna activa");
    sbtnVerFila->setFixedHeight(30);
    sbtnVerCol->setFixedHeight(30);

    vVer->addWidget(sbtnVerFila);
    vVer->addWidget(sbtnVerCol);
    vSide->addWidget(gbVer);

    // ── Grupo: Eliminar ───────────────────────────────────────────────────
    QGroupBox *gbElim = new QGroupBox("Eliminar");
    gbElim->setStyleSheet("QGroupBox { font-weight: bold; font-size: 10px; }");
    QVBoxLayout *vElim = new QVBoxLayout(gbElim);
    vElim->setSpacing(6);

    QPushButton *sbtnElimCelda = new QPushButton("Eliminar celda");
    QPushButton *sbtnElimFila  = new QPushButton("Eliminar fila");
    QPushButton *sbtnElimCol   = new QPushButton("Eliminar columna");

    sbtnElimCelda->setToolTip("Elimina la celda activa");
    sbtnElimFila->setToolTip("Elimina toda la fila de la celda activa");
    sbtnElimCol->setToolTip("Elimina toda la columna de la celda activa");

    sbtnElimCelda->setFixedHeight(30);
    sbtnElimFila->setFixedHeight(30);
    sbtnElimCol->setFixedHeight(30);

    // Estilo rojo suave para los botones de eliminar
    QString elimStyle = "QPushButton { color: #c0392b; } "
                        "QPushButton:hover { background-color: #fdecea; }";
    sbtnElimCelda->setStyleSheet(elimStyle);
    sbtnElimFila->setStyleSheet(elimStyle);
    sbtnElimCol->setStyleSheet(elimStyle);

    vElim->addWidget(sbtnElimCelda);
    vElim->addWidget(sbtnElimFila);
    vElim->addWidget(sbtnElimCol);
    vSide->addWidget(gbElim);

    // ── Grupo: celda activa (info contextual) ─────────────────────────────
    QGroupBox *gbInfo = new QGroupBox("Celda activa");
    gbInfo->setStyleSheet("QGroupBox { font-weight: bold; font-size: 10px; }");
    QVBoxLayout *vInfo = new QVBoxLayout(gbInfo);

    QLabel *hintLabel = new QLabel(
        "Haz clic en una celda\n"
        "de la tabla para\n"
        "seleccionarla y luego\n"
        "usa los botones\n"
        "de este panel."
        );
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    hintLabel->setStyleSheet("color: palette(mid); font-size: 9px; font-style: italic;");
    vInfo->addWidget(hintLabel);
    vSide->addWidget(gbInfo);

    vSide->addStretch();

    hContent->addWidget(sidebar);
    vMain->addWidget(contentArea, 1);

    // ═══════════════════════════════════════════════════════════════════════
    // STATUS BAR
    // ═══════════════════════════════════════════════════════════════════════
    lblStatus = new QLabel("Listo.  |  Doble clic o tecla para editar  |  Ctrl+Click = selección múltiple");
    lblStatus->setContentsMargins(8, 2, 8, 2);
    statusBar()->addWidget(lblStatus, 1);

    // ── Conexiones barra superior ─────────────────────────────────────────
    connect(btnIr,        &QPushButton::clicked, this, &MainWindow::onIr);
    connect(btnInsertar,  &QPushButton::clicked, this, &MainWindow::onInsertar);
    connect(btnModif,     &QPushButton::clicked, this, &MainWindow::onModificar);
    connect(btnOperar,    &QPushButton::clicked, this, &MainWindow::onOpSeleccion);
    connect(btnBorrR,     &QPushButton::clicked, this, &MainWindow::onBorrarRango);
    connect(btnLimpiar,   &QPushButton::clicked, this, &MainWindow::onLimpiarTodo);

    // ── Conexiones barra lateral ──────────────────────────────────────────
    connect(sbtnConsultar,  &QPushButton::clicked, this, &MainWindow::onConsultar);
    connect(sbtnVerFila,    &QPushButton::clicked, this, &MainWindow::onVerFila);
    connect(sbtnVerCol,     &QPushButton::clicked, this, &MainWindow::onVerColumna);
    connect(sbtnElimCelda,  &QPushButton::clicked, this, &MainWindow::onEliminarCelda);
    connect(sbtnElimFila,   &QPushButton::clicked, this, &MainWindow::onEliminarFila);
    connect(sbtnElimCol,    &QPushButton::clicked, this, &MainWindow::onEliminarColumna);

    // ── Conexiones tabla ──────────────────────────────────────────────────
    connect(table, &QTableWidget::cellClicked,  this, &MainWindow::onCellClicked);
    connect(table, &QTableWidget::cellChanged,  this, &MainWindow::onCellChanged);

    // ── Datos de ejemplo ──────────────────────────────────────────────────
    sheet.insertCell(1,1,"42");  sheet.insertCell(1,2,"58");
    sheet.insertCell(1,3,"Hola");sheet.insertCell(2,1,"15");
    sheet.insertCell(2,2,"30"); sheet.insertCell(3,1,"100");
    refreshGrid();
    log("Hoja lista — doble clic para editar | panel lateral: consultar, ver y eliminar.");
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
            QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(n->value));
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(n->row, n->col, item);
        }
    }
    ignorarCambios = false;
}

void MainWindow::log(const QString &msg)
{
    lblStatus->setText(msg);
}

// ── showError: cuadro de diálogo de error ─────────────────────────────────────
void MainWindow::showError(const QString &msg)
{
    log("⚠ " + msg);   // también aparece en la status bar

    QMessageBox *mb = new QMessageBox(this);
    mb->setWindowTitle("Error de operación");
    mb->setIcon(QMessageBox::Warning);
    mb->setText("<b>No se puede completar la operación</b>");
    mb->setInformativeText(msg);
    mb->setStandardButtons(QMessageBox::Ok);
    mb->setDefaultButton(QMessageBox::Ok);
    mb->exec();
    delete mb;
}

// ── resolverRango ─────────────────────────────────────────────────────────────
QList<QPair<int,int>> MainWindow::resolverRango(const QString &texto)
{
    QList<QPair<int,int>> lista;
    QString t = texto.trimmed().toUpper();
    if (t.isEmpty()) return lista;

    if (t.contains(':') && !t.contains(',')) {
        QStringList p = t.split(':');
        if (p.size() != 2) return lista;
        int r1,c1,r2,c2;
        traducirCoordenada(p[0],r1,c1); traducirCoordenada(p[1],r2,c2);
        if (r1<0||c1<0||r2<0||c2<0) return lista;
        if (r1>r2) std::swap(r1,r2); if (c1>c2) std::swap(c1,c2);
        for (int r=r1;r<=r2;++r) for (int c=c1;c<=c2;++c) lista.append({r,c});
    } else {
        for (const QString &tok : t.split(',', Qt::SkipEmptyParts)) {
            int r,c; traducirCoordenada(tok.trimmed(),r,c);
            if (r>=0 && c>=0) lista.append({r,c});
        }
    }
    return lista;
}

// ── operarSobreCeldas ─────────────────────────────────────────────────────────
//  Retorna false si hay mezcla de tipos (strings + números), true si OK.
bool MainWindow::operarSobreCeldas(const QList<QPair<int,int>> &celdas,
                                   int op, double &result, int &count)
{
    double acum = 0, maxV = -DBL_MAX, minV = DBL_MAX;
    count = 0;
    int strCount = 0;   // ← contamos strings no vacíos

    for (auto &[r,c] : celdas) {
        std::string v = sheet.getCell(r,c);
        if (v.empty()) continue;

        bool isNum = false;
        double d = 0.0;
        try { d = std::stod(v); isNum = true; } catch(...) {}

        if (isNum) {
            acum += d;
            if (d > maxV) maxV = d;
            if (d < minV) minV = d;
            count++;
        } else {
            strCount++;
        }
    }

    // ── Validación de tipos mixtos ────────────────────────────────────────
    if (count > 0 && strCount > 0) {
        // Construir lista de celdas problemáticas para el mensaje
        QStringList textCells;
        for (auto &[r,c] : celdas) {
            std::string v = sheet.getCell(r,c);
            if (v.empty()) continue;
            try { std::stod(v); } catch(...) {
                textCells << coordToLabel(r,c) + " (\"" + QString::fromStdString(v) + "\")";
            }
        }
        showError(
            QString("El rango contiene %1 celda(s) numérica(s) y %2 celda(s) con texto.\n\n"
                    "Celda(s) no numéricas: %3\n\n"
                    "No se pueden mezclar números y texto en una operación aritmética.\n"
                    "Solución: selecciona solo celdas numéricas, o elimina el texto del rango.")
                .arg(count)
                .arg(strCount)
                .arg(textCells.join(", "))
            );
        count = 0;
        return false;
    }

    if (!count) { result = 0.0; return true; }

    switch(op) {
    case 0: result = acum;         break;
    case 1: result = acum / count; break;
    case 2: result = maxV;         break;
    case 3: result = minV;         break;
    default: result = 0.0;
    }
    return true;
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

// ── Slot: click en celda ──────────────────────────────────────────────────────
void MainWindow::onCellClicked(int row, int col)
{
    edtCelda->setText(coordToLabel(row, col));
    edtValor->setText(QString::fromStdString(sheet.getCell(row, col)));

    // Actualiza el label de info en el panel lateral
    std::string val = sheet.getCell(row, col);
    sideInfoLabel->setText(val.empty()
                               ? QString("(%1 vacía)").arg(coordToLabel(row, col))
                               : QString("%1 = %2").arg(coordToLabel(row, col)).arg(QString::fromStdString(val)));

    auto sel = table->selectedItems();
    if (sel.size() > 1)
        log(QString("%1 celdas seleccionadas. Elige operación y pulsa 'Calcular →'").arg(sel.size()));
}

// ── BARRA SUPERIOR ─────────────────────────────────────────────────────────────

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
    if (sheet.getCell(r,c).empty()) {
        log(QString("%1 está vacía — usa Insertar.").arg(coordToLabel(r,c)));
        return;
    }
    std::string prev = sheet.getCell(r,c);
    sheet.modifyCell(r,c,val.toStdString());
    refreshGrid();
    log(QString("Modificada %1: \"%2\" → \"%3\"").arg(coordToLabel(r,c))
            .arg(QString::fromStdString(prev)).arg(val));
}

void MainWindow::onBorrarRango()
{
    auto cel = resolverRango(edtRango->text());
    if (cel.isEmpty()) { log("Rango inválido."); return; }
    for (auto &[r,cv] : cel) sheet.deleteCell(r,cv);
    refreshGrid();
    log(QString("Borradas %1 celdas.").arg(cel.size()));
}

static std::string fmt(double v) {
    std::ostringstream o; o << std::fixed << std::setprecision(2) << v; return o.str();
}

void MainWindow::onOpSeleccion()
{
    int op = cmbOpSel->currentIndex();
    QList<QPair<int,int>> celdas;
    QStringList ops = {"SUMA","PROMEDIO","MÁXIMO","MÍNIMO"};

    auto selItems = table->selectedItems();
    if (selItems.size() > 1)
        for (QTableWidgetItem *it : selItems) celdas.append({it->row(), it->column()});

    if (celdas.isEmpty() && !edtRango->text().trimmed().isEmpty())
        celdas = resolverRango(edtRango->text());

    if (celdas.isEmpty()) {
        log("Escribe un rango (ej: A1:C3) o selecciona celdas con Ctrl+Click.");
        return;
    }

    double res = 0.0;
    int cnt = 0;
    if (!operarSobreCeldas(celdas, op, res, cnt)) return;  // error ya mostrado
    if (!cnt) { log("Ninguna celda del rango tiene valor numérico."); return; }

    std::string s = fmt(res);

    int r,c; traducirCoordenada(edtCelda->text(), r, c);
    if (r>=0 && c>=0) {
        sheet.insertCell(r, c, s); expandIfNeeded(r, c); refreshGrid();
        log(QString("%1 [%2 celdas] = %3 → guardado en %4")
                .arg(ops[op]).arg(cnt).arg(QString::fromStdString(s)).arg(coordToLabel(r,c)));
    } else {
        log(QString("%1 [%2 celdas] = %3  (escribe celda destino en 'Celda' para guardar)")
                .arg(ops[op]).arg(cnt).arg(QString::fromStdString(s)));
    }
}

void MainWindow::onLimpiarTodo()
{
    auto nodes = sheet.getAllNodes();
    std::vector<std::pair<int,int>> coords;
    for (Node *n : nodes) coords.push_back({n->row, n->col});
    for (auto &[r,c] : coords) sheet.deleteCell(r,c);
    ignorarCambios = true;
    table->clearContents();
    ignorarCambios = false;
    sideInfoLabel->setText("—");
    log("Hoja limpiada.");
}

// ── BARRA LATERAL ─────────────────────────────────────────────────────────────

void MainWindow::onConsultar()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0||c<0) { log("Celda inválida."); return; }
    std::string val = sheet.getCell(r,c);
    if (val.empty()) {
        sideInfoLabel->setText(QString("(%1 vacía)").arg(coordToLabel(r,c)));
        log(QString("Consulta %1: (vacía)").arg(coordToLabel(r,c)));
    } else {
        sideInfoLabel->setText(
            QString("%1 = %2").arg(coordToLabel(r,c)).arg(QString::fromStdString(val)));
        log(QString("Consulta %1 = \"%2\"").arg(coordToLabel(r,c)).arg(QString::fromStdString(val)));
    }
}

void MainWindow::onVerFila()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0) { log("Fila inválida."); return; }
    QString res = QString("Fila %1:").arg(r+1); bool found=false;
    for (Node *n : sheet.getAllNodes())
        if (n->row==r){
            res += QString("  %1=%2").arg(coordToLabel(r,n->col)).arg(QString::fromStdString(n->value));
            found=true;
        }
    log(found ? res : res+"  (vacía)");
}

void MainWindow::onVerColumna()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (c<0) { log("Columna inválida."); return; }
    QString res = QString("Col %1:").arg(colLetter(c)); bool found=false;
    for (Node *n : sheet.getAllNodes())
        if (n->col==c){
            res += QString("  %1=%2").arg(coordToLabel(n->row,c)).arg(QString::fromStdString(n->value));
            found=true;
        }
    log(found ? res : res+"  (vacía)");
}

void MainWindow::onEliminarCelda()
{
    int r,c; traducirCoordenada(edtCelda->text(),r,c);
    if (r<0||c<0) { log("Celda inválida."); return; }
    sheet.deleteCell(r,c); refreshGrid();
    sideInfoLabel->setText(QString("(%1 eliminada)").arg(coordToLabel(r,c)));
    log(QString("Eliminada celda %1").arg(coordToLabel(r,c)));
}

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