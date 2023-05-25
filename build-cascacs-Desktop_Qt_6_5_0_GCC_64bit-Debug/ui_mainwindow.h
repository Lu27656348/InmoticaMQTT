/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
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
    QPushButton *crear_sub_btn;
    QLineEdit *crear_input;
    QLabel *label;
    QPushButton *listar_subcriptores_btn;
    QPushButton *servicios_subscritos_btn;
    QPushButton *subscribir_btn;
    QPushButton *listar_publicaciones_btn;
    QPushButton *on_btn;
    QPushButton *off_btn;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *idsubcriptor_input;
    QTableWidget *publisherTable;
    QMenuBar *menubar;
    QMenu *menuProyecto_distribuidos;
    QMenu *menu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(798, 330);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        crear_sub_btn = new QPushButton(centralwidget);
        crear_sub_btn->setObjectName("crear_sub_btn");
        crear_sub_btn->setGeometry(QRect(260, 30, 111, 24));
        crear_input = new QLineEdit(centralwidget);
        crear_input->setObjectName("crear_input");
        crear_input->setGeometry(QRect(120, 30, 131, 24));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 30, 91, 16));
        listar_subcriptores_btn = new QPushButton(centralwidget);
        listar_subcriptores_btn->setObjectName("listar_subcriptores_btn");
        listar_subcriptores_btn->setGeometry(QRect(20, 90, 191, 24));
        servicios_subscritos_btn = new QPushButton(centralwidget);
        servicios_subscritos_btn->setObjectName("servicios_subscritos_btn");
        servicios_subscritos_btn->setGeometry(QRect(20, 130, 191, 24));
        subscribir_btn = new QPushButton(centralwidget);
        subscribir_btn->setObjectName("subscribir_btn");
        subscribir_btn->setGeometry(QRect(20, 170, 191, 24));
        listar_publicaciones_btn = new QPushButton(centralwidget);
        listar_publicaciones_btn->setObjectName("listar_publicaciones_btn");
        listar_publicaciones_btn->setGeometry(QRect(20, 210, 191, 24));
        on_btn = new QPushButton(centralwidget);
        on_btn->setObjectName("on_btn");
        on_btn->setGeometry(QRect(260, 250, 71, 24));
        off_btn = new QPushButton(centralwidget);
        off_btn->setObjectName("off_btn");
        off_btn->setGeometry(QRect(340, 250, 71, 24));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(290, 220, 111, 16));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(270, 180, 111, 16));
        idsubcriptor_input = new QLineEdit(centralwidget);
        idsubcriptor_input->setObjectName("idsubcriptor_input");
        idsubcriptor_input->setGeometry(QRect(370, 180, 51, 24));
        publisherTable = new QTableWidget(centralwidget);
        publisherTable->setObjectName("publisherTable");
        publisherTable->setGeometry(QRect(440, 30, 321, 251));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 798, 23));
        menuProyecto_distribuidos = new QMenu(menubar);
        menuProyecto_distribuidos->setObjectName("menuProyecto_distribuidos");
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuProyecto_distribuidos->menuAction());
        menubar->addAction(menu->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        crear_sub_btn->setText(QCoreApplication::translate("MainWindow", "Crear Subscriptor ", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Crear Subcriptor ", nullptr));
        listar_subcriptores_btn->setText(QCoreApplication::translate("MainWindow", "Listar todos los subcriptores ", nullptr));
        servicios_subscritos_btn->setText(QCoreApplication::translate("MainWindow", "Servicios a los que esta subscrito", nullptr));
        subscribir_btn->setText(QCoreApplication::translate("MainWindow", "Subscribir un subscriptor", nullptr));
        listar_publicaciones_btn->setText(QCoreApplication::translate("MainWindow", "Lista de publicaciones", nullptr));
        on_btn->setText(QCoreApplication::translate("MainWindow", "Boton ON", nullptr));
        off_btn->setText(QCoreApplication::translate("MainWindow", "Boton OFF", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Switch de bombillo ", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "id del subscriptor", nullptr));
        menuProyecto_distribuidos->setTitle(QCoreApplication::translate("MainWindow", "Proyecto distribuidos ", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", ".", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
