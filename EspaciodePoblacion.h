#ifndef ESPACIODEPOBLACION_H
#define ESPACIODEPOBLACION_H

#include <QVector>
#include <iostream>

class Individuo
{
    QVector<int> p;
    float fo;
    bool evaluado;

public:

    //Constructor por defecto

    Individuo()
    {
        this->fo = 0.0f;
        this->evaluado = false;
    }

    //Constructor con 4 parámetros

    Individuo(int p1, int p2, int p3, int p4)
    {
        p.push_back(p1);
        p.push_back(p2);
        p.push_back(p3);
        p.push_back(p4);

        this->fo = 0.0f;
        this->evaluado = false;
    }

    ~Individuo()
    {
        this->p.clear();
        this->fo = 0.0f;
        this->evaluado = false;
    }

    void insertarParametro(int val)
    {
        this->p.push_back(val);
    }

    void editarParametro(int posicion, int val)
    {
        this->p.replace(posicion, val);
    }

    int obtenerParametro(int posicion)
    {
        return this->p.at(posicion);
    }

    int cantidadParametros()
    {
        return this->p.size();
    }

    void insertarFO(float FO)
    {
        this->fo = FO;
        this->evaluado = true;
    }

    float obtenerFO()
    {
        return this->fo;
    }

    void limpiar()
    {
        this->p.clear();
        this->fo = 0.0f;
    }

    bool operator<(const Individuo& i)const
    {
        return this->fo < i.fo;
    }

    bool operator<=(const Individuo& i)const
    {
        if (this->fo < i.fo)
            return true;

        if (this->fo == i.fo && this->p == i.p)
            return true;

        return false;
    }

    bool operator>(const Individuo& i)const
    {
        return this->fo > i.fo;
    }

    bool operator>=(const Individuo& i)const
    {
        if (this->fo > i.fo)
            return true;

        if (this->fo == i.fo && this->p == i.p)
            return true;

        return false;
    }

    bool operator==(const Individuo& i)const
    {
        return
                this->p == i.p &&
                this->fo == i.fo;
    }

    friend std::ostream & operator<<(std::ostream &salida, const Individuo &ind)
    {
        for (int i = 0; i < ind.p.size(); i++)
        {
            salida << ind.p.at(i) << " ";
        }
        salida << "| " << ind.fo;

        return salida;
    }


    QString aQString()
    {
        QString salida;

        salida.push_back("[");

        for (int i = 0; i < p.size(); i++)
        {
            salida.push_back(QString::number(p.at(i)));

            if (i == p.size() - 1)
                salida.push_back("]");
            else
                salida.push_back(", ");
        }

        return salida;
    }

    bool estaEvaluado() const
    {
        return this->evaluado;
    }

};

extern QVector< Individuo > EspaciodePoblacion;


#endif // ESPACIODEPOBLACION_H
