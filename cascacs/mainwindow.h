#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Publisher_class.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,QVector<Publisher> publisher_list_ = {});
    ~MainWindow();

private slots:
    void on_crear_sub_btn_clicked();

    void on_listar_subcriptores_btn_clicked(std::vector<int> &int_list);

    void on_servicios_subscritos_btn_clicked();

    void on_subscribir_btn_clicked();

    void on_listar_publicaciones_btn_clicked();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

