#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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


void MainWindow::on_listar_subcriptores_btn_clicked()
{

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

