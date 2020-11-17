#ifndef ESPACIODECREENCIAS_H
#define ESPACIODECREENCIAS_H

#include "EspaciodePoblacion.h"

#include <Qmap>

class Limite
{
    int inferior;
    int superior;

public:
    Limite()
    {
        this->inferior = 0;
        this->superior = 0;
    }

    Limite(int inferior, int superior)
    {
        this->inferior = inferior;
        this->superior = superior;
    }

    void setInferior(int valor)
    {
        this->inferior = valor;
    }

    void setSuperior(int valor)
    {
        this->superior = valor;
    }

    int getInferior() const
    {
        return this->inferior;
    }

    int getSuperior() const
    {
        return this->superior;
    }
};

/*
    Conocimiento Normativo:
        cN: vector de límites de tipo entero
*/

class ConocimientoNormativo
{
    QVector< Limite > cN;

public:

    ConocimientoNormativo()
    {
    }

    //Constructor con 4 límites

    ConocimientoNormativo(Limite a, Limite b, Limite c, Limite d)
    {
        cN.push_back(a);
        cN.push_back(b);
        cN.push_back(c);
        cN.push_back(d);
    }

    ~ConocimientoNormativo()
    {
        this->cN.clear();
    }

    void insertarLimite(Limite l)
    {
        this->cN.push_back(l);
    }

    void editarLimite(int posicion, Limite l)
    {
        this->cN.replace(posicion, l);
    }

    Limite obtenerLimite(int posicion)
    {
        return this->cN.at(posicion);
    }

    int tamanio()
    {
        return this->cN.size();
    }

    bool estaVacio()
    {
        return this->cN.empty();
    }

    friend std::ostream & operator << (std::ostream &salida, const ConocimientoNormativo &cn)
    {
        salida<<"[";
        for (int i = 0; i < cn.cN.size(); i++)
        {
            salida<<"["<<cn.cN.at(i).getInferior()<<","<<cn.cN.at(i).getSuperior();

            if (i == cn.cN.size() - 1 )
                salida<<"]";
            else
                salida<<"] ";
        }
        salida<<"]";

        return salida;
    }


    QString aQString()
    {
        QString salida;

        salida.push_back("[");
        for (int i = 0; i < cN.size(); i++)
        {
            salida.push_back("[" + QString::number(cN.at(i).getInferior()) + "," + QString::number(cN.at(i).getSuperior()));

            if (i == cN.size() - 1 )
                salida.push_back("]");
            else
                salida.push_back("] ");
        }
        salida.push_back("]");

        return salida;
    }


    void limpiar()
    {
        this->cN.clear();
    }


};

extern ConocimientoNormativo cn;

//==================================================

class Fila
{
    int v;
    size_t io;
    float c;

public:

    // Constructor por defecto

    Fila()
    {
        this->v = 0;
        this->io = 0;
        this->c = 0.0f;
    }

    // Constructor necesario para realizar una busqueda del valor v

    Fila(int v)
    {
        this->v = v;
        this->io = 0;
        this->c = 0.0f;
    }

    Fila(int v, size_t io, float c)
    {
        this->v = v;
        this->io = io;
        this->c = c;
    }

    void setValor(int v)
    {
        this->v = v;
    }

    void setIO(int io)
    {
        this->io = io;
    }

    void setC(float c)
    {
        this->c = c;
    }

    int getV()
    {
        return this->v;
    }

    int getIO()
    {
        return this->io;
    }

    float getC()
    {
        return this->c;
    }

    // Sobrecarga del operador == para la busqueda del valor V

    bool operator==(const Fila& f)const
    {
        return this->v == f.v;
    }

    friend std::ostream & operator << (std::ostream &salida, const Fila &f)
    {
        salida<<"["<<f.v<<", "<<f.io<<", "<<f.c<<"]";

        return salida;
    }

    QString aQString()
    {
        QString salida;

        salida.push_back("[" + QString::number(v) + ", " + QString::number(io) + ", " + QString::number(c) + "]");

        return salida;
    }

};

class ConocimientoSituacional
{
    /*
     * diccionario basado en árbol rojo-negro.
     * clave: id del Parámetro, valor: vector de filas (o una tabla/matriz),
     * cada fila tiene como columnas: v, io, c.
    */

    QMap<int, QVector<Fila> > cS;

public:

    ConocimientoSituacional()
    {
    }

    ~ConocimientoSituacional()
    {
        cS.clear();
    }

    void insertarFila(int parametro, Fila fila)
    {
        this->cS[parametro].push_back(fila);
    }

    void editarFila(int parametro, int posicion, Fila fila)
    {
        this->cS[parametro].replace(posicion, fila);
    }

    Fila obtenerFila(int parametro, int posicion)
    {
        return this->cS[parametro].at(posicion);
    }

    QVector<Fila> obtenerFilas(int parametro)
    {
        return this->cS[parametro];
    }

    int tamanioTabla(int parametro)
    {
        return this->cS[parametro].size();
    }

    bool tablaVacia(int parametro)
    {
        return this->cS[parametro].empty();
    }

    QString aQString()
    {
        QString salida;

        QMap<int, QVector<Fila> >::const_iterator itor = cS.constBegin();

        while (itor != cS.constEnd()) {

            salida.push_back("P" + QString::number(itor.key() + 1) + ": [");

            for (int i = 0; i < itor.value().size(); i++)
            {
                Fila fila = itor.value().at(i);

                salida.push_back(fila.aQString());

                if (i == itor.value().size() - 1)
                    salida.push_back("]");
                else
                    salida.push_back(" ");
            }

            if (itor != cS.constEnd() - 1)
                salida.push_back(", ");

            ++itor;
        }

        return salida;
    }


    friend std::ostream & operator << (std::ostream &salida, const ConocimientoSituacional &cs)
    {

        QMap<int, QVector<Fila> >::const_iterator itor = cs.cS.constBegin();

        salida << "Parametro\tFilas/Parametro\n";

        while (itor != cs.cS.constEnd()) {

            salida << itor.key() + 1 << "\t[";

            for (int i = 0; i < itor.value().size(); i++)
            {
                salida << itor.value().at(i);

                if (i == itor.value().size() - 1)
                    salida << "]";
                else
                    salida << " ";
            }

            salida << "\n";

            ++itor;
        }

        return salida;
    }


    bool estaVacio()
    {
        return this->cS.isEmpty();
    }

    void limpiar()
    {
        this->cS.clear();
    }


};

extern ConocimientoSituacional cs;

#endif // ESPACIODECREENCIAS_H
