/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTableWidget *tableWidget;
    QPushButton *pushButton;
    QLineEdit *inputFila;
    QLineEdit *inputValor;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *inputRango;
    QPushButton *sumarR;
    QPushButton *borrarR;
    QPushButton *Ir_Button;
    QPushButton *btnPromedio;
    QPushButton *btnLimpiarTodo;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1438, 716);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        tableWidget = new QTableWidget(centralwidget);
        if (tableWidget->columnCount() < 20)
            tableWidget->setColumnCount(20);
        if (tableWidget->rowCount() < 20)
            tableWidget->setRowCount(20);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setGeometry(QRect(0, 60, 1431, 681));
        tableWidget->setRowCount(20);
        tableWidget->setColumnCount(20);
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(10, 30, 101, 29));
        inputFila = new QLineEdit(centralwidget);
        inputFila->setObjectName("inputFila");
        inputFila->setGeometry(QRect(120, 30, 113, 28));
        inputValor = new QLineEdit(centralwidget);
        inputValor->setObjectName("inputValor");
        inputValor->setGeometry(QRect(330, 30, 421, 28));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(120, 10, 111, 20));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(760, 30, 63, 20));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(940, 0, 151, 20));
        inputRango = new QLineEdit(centralwidget);
        inputRango->setObjectName("inputRango");
        inputRango->setGeometry(QRect(940, 30, 141, 28));
        sumarR = new QPushButton(centralwidget);
        sumarR->setObjectName("sumarR");
        sumarR->setGeometry(QRect(1120, 0, 101, 29));
        borrarR = new QPushButton(centralwidget);
        borrarR->setObjectName("borrarR");
        borrarR->setGeometry(QRect(1120, 30, 101, 29));
        Ir_Button = new QPushButton(centralwidget);
        Ir_Button->setObjectName("Ir_Button");
        Ir_Button->setGeometry(QRect(10, 0, 41, 29));
        btnPromedio = new QPushButton(centralwidget);
        btnPromedio->setObjectName("btnPromedio");
        btnPromedio->setGeometry(QRect(1220, 10, 90, 29));
        btnLimpiarTodo = new QPushButton(centralwidget);
        btnLimpiarTodo->setObjectName("btnLimpiarTodo");
        btnLimpiarTodo->setGeometry(QRect(1330, 20, 90, 29));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1438, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Insertar Dato", nullptr));
        inputFila->setText(QString());
        inputValor->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "Celda", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Valor", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Operaciones de rango", nullptr));
        sumarR->setText(QCoreApplication::translate("MainWindow", "Sumar Rango", nullptr));
        borrarR->setText(QCoreApplication::translate("MainWindow", "Borrar Rango", nullptr));
        Ir_Button->setText(QCoreApplication::translate("MainWindow", "Ir", nullptr));
        btnPromedio->setText(QCoreApplication::translate("MainWindow", "Promedio", nullptr));
        btnLimpiarTodo->setText(QCoreApplication::translate("MainWindow", "LIMPIAR", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
