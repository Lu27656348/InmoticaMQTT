#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent,QVector<Publisher> publisher_list)
    : QMainWindow(parent)

    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Accede al QTableWidget con el nombre "myTableWidget"
    QTableWidget *myTableWidget = ui->publisherTable;

    // Haz algo con el QTableWidget
    myTableWidget->setRowCount(publisher_list.size());
    myTableWidget->setColumnCount(2);
    myTableWidget->setItem(0, 0, new QTableWidgetItem("Hello"));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_crear_sub_btn_clicked()
{
    QString campo1 = ui->crear_input->text();
    QMessageBox::information(nullptr,"mensaje enviado",campo1);
}


void MainWindow::on_listar_subcriptores_btn_clicked(std::vector<int> &int_list)
{
    std::cout << "Hola mundo" << std::endl;
    for (int elemento : int_list) {
        std::cout << elemento << std::endl;
    }

}

void MainWindow::on_servicios_subscritos_btn_clicked()
{

}


void MainWindow::on_subscribir_btn_clicked()
{

}


void MainWindow::on_listar_publicaciones_btn_clicked()
{

}

