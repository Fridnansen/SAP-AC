#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();    

    MainWindow &getMW() {
        return *this;
    }

    Ui::MainWindow &getUI()
    {
        return *ui;
    }

private slots:
    void on_botonReiniciar_clicked();

    void on_botonSalir_clicked();

    void on_botonEjecutar_clicked();

    void on_actionAcerca_de_triggered();

    void on_actionSalir_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
