#ifndef AC_H
#define AC_H

#include <QWidget>

#include "qdebugstream.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "EspaciodeCreencias.h"

#include "random.h"


namespace Ui {
class AC;
}

class AC : public QWidget
{
    Q_OBJECT

public:
    explicit AC(MainWindow &mw, QWidget *parent = nullptr);
    ~AC();    

    void verP(QVector<Individuo> poblacion);
    QVector<Individuo> obtenerAceptados(QVector<Individuo> EspaciodePoblacion, int numAceptados);
    void actualizarCN(ConocimientoNormativo &cn, QVector<Individuo> aceptados);
    void actualizarCS(ConocimientoSituacional &cs, QVector<Individuo> aceptados, int generacion, float u);
    void evaluarP(QVector<Individuo> &Poblacion, float a, float minPA, float maxPA, float b, float minLE, float maxLE);
    void cruce(QVector<Individuo> &Poblacion, QVector<Individuo> aceptados, float probCruce, int presion);
    void mutacion(QVector<Individuo> &Poblacion, float probMutacion, int presion);


private slots:
    void on_botonRegresar_clicked();

    void on_botonSalir_clicked();

private:
    Ui::AC *ui;

    MainWindow &mw;   

};

#endif // AC_H
