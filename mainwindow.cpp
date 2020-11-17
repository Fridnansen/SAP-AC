#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ac.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->statusbar->showMessage("Listo", 3000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_botonReiniciar_clicked()
{
    ui->numIndividuos->setValue(3);
    ui->numGeneraciones->setValue(10);
    ui->probCruce->setValue(0.1);
    ui->probMutacion->setValue(0.1);
    ui->valorMiu->setValue(0.4);

    ui->minP1->setValue(1);
    ui->maxP1->setValue(5);

    ui->minP2->setValue(4);
    ui->maxP2->setValue(7);

    ui->minP3->setValue(2);
    ui->maxP3->setValue(10);

    ui->minP4->setValue(5);
    ui->maxP4->setValue(15);

    ui->valorA->setValue(1000.0);
    ui->minPA->setValue(0);
    ui->maxPA->setValue(1);

    ui->valorB->setValue(100.0);
    ui->minLE->setValue(0);
    ui->maxLE->setValue(1);

}

void MainWindow::on_botonSalir_clicked()
{
    close();
}

void MainWindow::on_botonEjecutar_clicked()
{
    AC *pAC = new AC(getMW(), nullptr);
    pAC->setWindowTitle("Simulador del SAP v1.0");
    pAC->show();

    this->hide();

}

void MainWindow::on_actionAcerca_de_triggered()
{
    QMessageBox::information(this, tr("Simulador del SAP v1.0"),
                             tr("Sistema Adaptativo de Parámetros para Juegos Serios Emergentes,\nutilizando Algoritmo Cultural.\n\n"
                                "Versión: 1.0"));
}

void MainWindow::on_actionSalir_triggered()
{
    close();
}
