#include "ac.h"
#include "ui_ac.h"

#include "qcustomplot/qcustomplot.h"

#include <iostream>
#include <algorithm>

// Sobrecarga del operador << para el TDA QVector.

template<class T>
std::ostream & operator << (std::ostream &salida, const QVector<T> &v)
{
    salida<<"[";
    for (int i = 0; i < v.size(); i++)
    {
        salida<<v.at(i);

        if (i == v.size() - 1)
            salida<<"]";
        else
            salida<<"; ";
    }

    return salida;
}

// Función necesaria para ordenar un tipo de dato T, de mayor a menor.

template <class T>
bool MayorAMenor(const T &a, const T &b)
{
    return a > b;
}

//=======================================================================

QVector< Individuo > EspaciodePoblacion;
ConocimientoNormativo cn;
ConocimientoSituacional cs;

//=======================================================================

void AC::verP(QVector<Individuo> poblacion)
{
    for (int i = 0; i < poblacion.size(); i++)
    {
        std::cout<<i+1<<": "<<poblacion.at(i)<<"\n";
    }
}

QVector<Individuo> AC::obtenerAceptados(QVector<Individuo> EspaciodePoblacion, int numAceptados)
{
    QVector<Individuo> losAceptados;

    for (int i = 0; i < numAceptados; i++)
    {
        losAceptados.push_back(EspaciodePoblacion.at(i));
    }

    return losAceptados;
}

void AC::actualizarCN(ConocimientoNormativo &cn, QVector<Individuo> aceptados)
{
    /*
    En el caso del conocimiento normativo, los valores de LIj y LSj se actualizan si en la actual población,
    alguno de sus individuos tiene un valor superior a LSj o inferior a LIj.
    De esta manera, cada vez que llega una nueva experiencia de la población, los límites de cada parámetro se actualizan.

    Adjust (Bt, Accept (ESGt)) // Adjust the belief space with the best individuals

    In the case of normative, for P3,
    since this value is between 2 and 10,
    it is not necessary to change the limits of that parameter.
    The same update procedure is done for the rest of the parameters.
    */

    if (aceptados.empty())
        return;

    //cout<<"\tActualizando CN\n";

    for (int i = 0; i < aceptados.size(); i++)
    {
        //cout<<"\tAceptado: "<<aceptados.at(i)<<endl;
        for (int u = 0; u < cn.tamanio(); u++)
        {
            // Obteniendo el limite actual del parámetro u

            Limite li = cn.obtenerLimite(u);

            // Obteniendo el parámetro i del individuo j

            int Parametro = aceptados[i].obtenerParametro(u);

            /* Dentro de los límites
            li.getInferior() <= Parametro <= li.getSuperior()

               Fuera de los límites
            !(li.getInferior() <= Parametro &&  Parametro <= li.getSuperior())
            */

            if (!(li.getInferior() <= Parametro &&  Parametro <= li.getSuperior()))
            {
                if (Parametro < li.getInferior())
                    li.setInferior(Parametro);

                if (Parametro > li.getSuperior())
                    li.setSuperior(Parametro);

                cn.editarLimite(u, li);
            }
            /*else
            {
                cout<<"El parametro "<<u+1<<": "<<aceptados.at(i).obtenerParametro(u)<<", esta dentro de los limites\n";
            }*/
        }
    }

}

void AC::actualizarCS(ConocimientoSituacional &cs, QVector<Individuo> aceptados, int generacion, float u)
{
    /*
    ver si existe el parámetro

        si existe: actualizar io y c
        si no existe: insertar por primera vez con io = 1 y c: del individuo

    */

    //cout<<"\tActualizando CS:\n"<<endl;

    for (int aceptado = 0; aceptado < aceptados.size(); aceptado++)
    {
        Individuo elAceptado = aceptados.at(aceptado);

        for (int i = 0; i < elAceptado.cantidadParametros(); i++)
        {
            //Buscar fila con V = mejorIndividuo.obtenerParametro(i)

            Fila filaAbuscar(elAceptado.obtenerParametro(i));

            //Se obtiene el vector de filas, del parámetro i

            QVector<Fila> vFilas = cs.obtenerFilas(i);
            QVector<Fila>::iterator it = std::find(vFilas.begin(), vFilas.end(), filaAbuscar);

            if (it != vFilas.end())
            {
                int posicion = std::distance(vFilas.begin(), it);

                //cout << "i: "<<i<<" Existe en la posicion: " << posicion <<endl;

                Fila filaAeditar = vFilas.at(posicion);

                // IOj = NOj + IOj

                filaAeditar.setIO(1 + filaAeditar.getIO());

                // m = u/generacion;

                /*cout<<"Valor encontrado: "<<mejorIndividuo.obtenerParametro(i)<<" en la generacion: "<<generacion<<endl;

                cout<<"Valor de u: "<<u<<endl;*/

                float m = 0.0f;
                if (!generacion)
                {
                    m = u;
                }
                else
                {
                    m = u/generacion;
                }

                //cout<<"m: "<<m<<endl;

                float m_rayaArriba = 1.0f - m;

                //cout<<"m_rayaArriba: "<<m_rayaArriba<<endl;

                /*
                    Cj raya Arriba: es el promedio del valor Cj de todos los individuos dentro del 20% aceptados proveniente de la población actual con el valor de Vj.

                    Cj raya Arriba, es lo que llevo guardado de Cj, después de la primera vez.
                    Y el Cj sin la raya, es el valor de la FO del individuo donde apareció nuevamente el valor V
                */

                float Cj_rayaArriba = filaAeditar.getC();

                //cout<<"Cj_rayaArriba: "<<Cj_rayaArriba<<endl;

                float Cj_sinRaya = elAceptado.obtenerFO();

                //cout<<"Cj_sinRaya: "<<Cj_sinRaya<<endl;

                Cj_sinRaya = Cj_sinRaya * m_rayaArriba + Cj_rayaArriba * m;

                //cout<<"Cj sin raya, final: "<<Cj_sinRaya<<endl;

                filaAeditar.setC(Cj_sinRaya);

                cs.editarFila(i, posicion, filaAeditar);


                //Actualizando tablas del Conocimiento Situacional:

                if (i == 0)
                {
                    ui->tabCSP1->setItem(posicion, 0, new QTableWidgetItem(QString::number(filaAeditar.getV())));
                    ui->tabCSP1->setItem(posicion, 1, new QTableWidgetItem(QString::number(filaAeditar.getIO())));
                    ui->tabCSP1->setItem(posicion, 2, new QTableWidgetItem(QString::number(filaAeditar.getC())));
                }
                else if (i == 1)
                {
                    ui->tabCSP2->setItem(posicion, 0, new QTableWidgetItem(QString::number(filaAeditar.getV())));
                    ui->tabCSP2->setItem(posicion, 1, new QTableWidgetItem(QString::number(filaAeditar.getIO())));
                    ui->tabCSP2->setItem(posicion, 2, new QTableWidgetItem(QString::number(filaAeditar.getC())));
                }
                else if (i == 2)
                {
                    ui->tabCSP3->setItem(posicion, 0, new QTableWidgetItem(QString::number(filaAeditar.getV())));
                    ui->tabCSP3->setItem(posicion, 1, new QTableWidgetItem(QString::number(filaAeditar.getIO())));
                    ui->tabCSP3->setItem(posicion, 2, new QTableWidgetItem(QString::number(filaAeditar.getC())));
                }
                else if (i == 3)
                {
                    ui->tabCSP4->setItem(posicion, 0, new QTableWidgetItem(QString::number(filaAeditar.getV())));
                    ui->tabCSP4->setItem(posicion, 1, new QTableWidgetItem(QString::number(filaAeditar.getIO())));
                    ui->tabCSP4->setItem(posicion, 2, new QTableWidgetItem(QString::number(filaAeditar.getC())));
                }

            }
            else
            {
                //cout << "i: "<<i<<" No existe." << endl;

                cs.insertarFila(i, Fila(filaAbuscar.getV(), 1, elAceptado.obtenerFO()));

                //Insertando filas en las tablas del Conocimiento Situacional:

                int indiceFila = 0;

                if (i == 0)
                {
                    ui->tabCSP1->insertRow(ui->tabCSP1->rowCount());
                    indiceFila = ui->tabCSP1->rowCount() - 1;
                    ui->tabCSP1->setItem(indiceFila, 0, new QTableWidgetItem(QString::number(filaAbuscar.getV())));
                    ui->tabCSP1->setItem(indiceFila, 1, new QTableWidgetItem(QString::number(1)));
                    ui->tabCSP1->setItem(indiceFila, 2, new QTableWidgetItem(QString::number(elAceptado.obtenerFO())));
                }
                else if (i == 1)
                {
                    ui->tabCSP2->insertRow(ui->tabCSP2->rowCount());
                    indiceFila = ui->tabCSP2->rowCount() - 1;
                    ui->tabCSP2->setItem(indiceFila, 0, new QTableWidgetItem(QString::number(filaAbuscar.getV())));
                    ui->tabCSP2->setItem(indiceFila, 1, new QTableWidgetItem(QString::number(1)));
                    ui->tabCSP2->setItem(indiceFila, 2, new QTableWidgetItem(QString::number(elAceptado.obtenerFO())));
                }
                else if (i == 2)
                {
                    ui->tabCSP3->insertRow(ui->tabCSP3->rowCount());
                    indiceFila = ui->tabCSP3->rowCount() - 1;
                    ui->tabCSP3->setItem(indiceFila, 0, new QTableWidgetItem(QString::number(filaAbuscar.getV())));
                    ui->tabCSP3->setItem(indiceFila, 1, new QTableWidgetItem(QString::number(1)));
                    ui->tabCSP3->setItem(indiceFila, 2, new QTableWidgetItem(QString::number(elAceptado.obtenerFO())));
                }
                else if (i == 3)
                {
                    ui->tabCSP4->insertRow(ui->tabCSP4->rowCount());
                    indiceFila = ui->tabCSP4->rowCount() - 1;
                    ui->tabCSP4->setItem(indiceFila, 0, new QTableWidgetItem(QString::number(filaAbuscar.getV())));
                    ui->tabCSP4->setItem(indiceFila, 1, new QTableWidgetItem(QString::number(1)));
                    ui->tabCSP4->setItem(indiceFila, 2, new QTableWidgetItem(QString::number(elAceptado.obtenerFO())));
                }
            }
        }
    }

}


void AC::evaluarP(QVector<Individuo> &Poblacion, float a, float minPA, float maxPA, float b, float minLE, float maxLE)
{
    for (int i = 0; i < Poblacion.size(); i++)
    {
        if ( Poblacion.at(i).estaEvaluado() )
            continue;

        float PA = numAleatorioReal(minPA, maxPA);
        float LE = numAleatorioReal(minLE, maxLE);


        EspaciodePoblacion[i].insertarFO(a * PA
                                         -
                                         b * LE);

        //std::cout<<"PA: ["<<PA<<"] LE: ["<<LE<<"] FO: ["<<EspaciodePoblacion[i].obtenerFO()<<"]\n";

    }
}


void AC::cruce(QVector<Individuo> &Poblacion, QVector<Individuo> aceptados, float probCruce, int presion)
{
    //Deben existir dos padres de entre los aceptados.

    if (aceptados.size() <= 1)
        return;

    std::cout<<"\n   Aplicando el operador de cruce:\n";

    //std::cout<<"La resta: "<< EspaciodePoblacion.size() - presion<<"\n";

    int counter = 0;
    //std::cout<<"La población restante:\n";
    for (int i = presion; i < EspaciodePoblacion.size(); i++)
    {

        if (numAleatorioReal(0,1) <= probCruce)
        {
            std::cout<<++counter;
            //std::cout<</*". Presion ["<<i<<"]: "<<*/EspaciodePoblacion.at(i)<<"\n";

            //El punto, entre 1 y (número de parámetros - 1)
            int punto = numAleatorio(1, 3);

            std::cout<<". El punto de cruce: ["<< punto<<"]\n";

            //Deben existir dos padres de entre los aceptados.

            int padre = numAleatorio(0, aceptados.size() - 1);
            int madre = numAleatorio(0, aceptados.size() - 1);
            while (padre == madre)
                padre = numAleatorio(0, aceptados.size() - 1);

            Individuo indPadre = aceptados.at(padre);
            Individuo indMadre = aceptados.at(madre);

            std::cout<<"   EL Padre: "<<indPadre<<";\t";
            std::cout<<"La Madre: "<<indMadre<<"\n";

            // population[i][:punto] = padre[:punto] #Se mezcla el material genetico de los padres en cada nuevo individuo

            Individuo hijo;

            for (int y = 0; y < punto; y++)
            {
                hijo.insertarParametro( indMadre.obtenerParametro(y) );
            }

            for (int x = punto; x < indPadre.cantidadParametros(); x++)
            {
                hijo.insertarParametro( indPadre.obtenerParametro(x) );
            }

            std::cout<<"\tEl Hijo es: "<<hijo<<"\n";

            EspaciodePoblacion[i] = hijo;
        }
    }
}


void AC::mutacion(QVector<Individuo> &Poblacion, float probMutacion, int presion)
{
    //Influencia, caso mutación:

    //Cada individuo de la poblacion (menos los padres, que están en los aceptados) tienen una probabilidad de mutar

    //Selección de la mutación, 0: situacional, 1, normal.

    std::cout<<"\n   Aplicando el operador de mutación:\n";

    int counter = 0;

    for (int i = presion; i < EspaciodePoblacion.size(); i++)
    {
        std::cout<<++counter<<". ";

        if (numAleatorioReal(0,1) <= probMutacion)
        {
            Individuo indMutar = EspaciodePoblacion.at(i);
            Individuo indMutado;
            int tipoMutacion = numAleatorio(0,1);

            std::cout<<"Individuo a Mutar: "<<indMutar<<"\n";
            std::cout<<"    Mutación elegida: "<<(tipoMutacion ? "normativo":"situacional")<<"\n";

            int parametroElegido = numAleatorio(0, indMutar.cantidadParametros() - 1);

            if (tipoMutacion == 0)
            {
                // Obtener el valor, basado en Cj, del conocimiento situacional

                QVector<Fila> vFilas = cs.obtenerFilas(parametroElegido);

                float Cj = 0.0f;
                int valorElegido = 0;

                for (int f = 0; f < vFilas.size(); f++)
                {
                    Fila fila = vFilas.at(f);

                    if (fila.getC() > Cj)
                    {
                        Cj = fila.getC();
                        valorElegido = fila.getV();
                    }
                }
                std::cout<<"    Parametro: "<<parametroElegido+1<<". Mejor valor Cj: "<<Cj<<", Valor Elegido: "<<valorElegido<<"\n";

                //Ahora procedo con la mutación dirigida

                //Copio los parámetros del individuo a mutar en el individuo mutado.

                for (int p = 0; p < indMutar.cantidadParametros(); p++)
                {
                    indMutado.insertarParametro(indMutar.obtenerParametro(p));
                }

                //Edito el parámetro elegido para mutar.

                indMutado.editarParametro(parametroElegido, valorElegido);

                std::cout<<"    Individuo Mutado (usando conocimiento situacional): "<<indMutado<<"\n";

                EspaciodePoblacion[i] = indMutado;

            }
            else
            {
                int aleatorio = numAleatorio(cn.obtenerLimite(parametroElegido).getInferior(), cn.obtenerLimite(parametroElegido).getSuperior());

                while (aleatorio == indMutar.obtenerParametro(parametroElegido))
                    aleatorio = numAleatorio(cn.obtenerLimite(parametroElegido).getInferior(), cn.obtenerLimite(parametroElegido).getSuperior());

                std::cout<<"    Parametro: "<<parametroElegido+1<<". Valor Elegido: "<<aleatorio<<"\n";

                //Ahora procedo con la mutación clásica

                //Copio los parámetros del individuo a mutar en el individuo mutado.

                for (int p = 0; p < indMutar.cantidadParametros(); p++)
                {
                    indMutado.insertarParametro(indMutar.obtenerParametro(p));
                }

                //Edito el parámetro elegido para mutar.

                indMutado.editarParametro(parametroElegido, aleatorio);

                std::cout<<"    Individuo Mutado (usando conocimiento normativo): "<<indMutado<<"\n";

                EspaciodePoblacion[i] = indMutado;
            }
        }
        else
        {
            std::cout<<"\n";
        }
    }

}



//=======================================================================


/*
 *
 * Algoritmo Cultural
 *
*/


AC::AC(MainWindow &mw, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AC),
    mw(mw)
{
    ui->setupUi(this);

    new QDebugStream(std::cout, ui->Salida);

    //Datos de entrada
    {

        std::cout <<"=== DATOS DE ENTRADA ===\n\n";

        std::cout << "Número de individuos: " << mw.getUI().numIndividuos->value() <<"\n";
        std::cout << "Número de generaciones: " << mw.getUI().numGeneraciones->value() <<"\n";
        std::cout << "Probabilidad de cruce: " << mw.getUI().probCruce->value() <<"\n";
        std::cout << "Probabilidad de mutación: " << mw.getUI().probMutacion->value() <<"\n";
        std::cout << "Valor de miu: " << mw.getUI().valorMiu->value() <<"\n\n";

        std::cout <<"--- Conocimiento Normativo ---\n";

        std::cout << "P1 minimo: " << mw.getUI().minP1->value() <<"\n";
        std::cout << "P1 maximo: " << mw.getUI().maxP1->value() <<"\n\n";

        std::cout << "P2 minimo: " << mw.getUI().minP2->value() <<"\n";
        std::cout << "P2 maximo: " << mw.getUI().maxP2->value() <<"\n\n";

        std::cout << "P3 minimo: " << mw.getUI().minP3->value() <<"\n";
        std::cout << "P3 maximo: " << mw.getUI().maxP3->value() <<"\n\n";

        std::cout << "P4 minimo: " << mw.getUI().minP4->value() <<"\n";
        std::cout << "P4 maximo: " << mw.getUI().maxP4->value() <<"\n\n";

        std::cout <<"--- Función objetivo ---\n";

        std::cout << "Valor de a: " << mw.getUI().valorA->value() <<"\n";
        std::cout << "Valor de PA minimo: " << mw.getUI().minPA->value() <<"\n";
        std::cout << "Valor de PA maximo: " << mw.getUI().maxPA->value() <<"\n\n";

        std::cout << "Valor de b: " << mw.getUI().valorB->value() <<"\n";
        std::cout << "Valor de LE minimo: " << mw.getUI().minLE->value() <<"\n";
        std::cout << "Valor de LE maximo: " << mw.getUI().maxLE->value() <<"\n\n";

    }

    //Tablas
    {
        //Definiendo la tabla de Conocimiento Normativo

        QStringList m_TableHeader;
        ui->tabCN->setColumnCount(2);
        m_TableHeader<<"Generación"<<"CN";
        ui->tabCN->setHorizontalHeaderLabels(m_TableHeader);
        ui->tabCN->verticalHeader()->setVisible(false);
        ui->tabCN->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tabCN->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tabCN->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tabCN->setShowGrid(true);
        //ui->tabCN->setStyleSheet("QTableView {selection-background-color: cyan;}");
        ui->tabCN->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        m_TableHeader.clear();

        //Definiendo la tabla del mejor individuo

        ui->tabMI->setColumnCount(3);
        m_TableHeader<<"Generación"<<"Individuo"<<"FO";
        ui->tabMI->setHorizontalHeaderLabels(m_TableHeader);
        ui->tabMI->verticalHeader()->setVisible(false);
        ui->tabMI->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tabMI->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tabMI->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tabMI->setShowGrid(true);
        //ui->tabMI->setStyleSheet("QTableView {selection-background-color: cyan;}");
        ui->tabMI->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        //http://www.programmersought.com/article/8103151052/
        QHeaderView *headerGoods = ui->tabMI->horizontalHeader();
        headerGoods->setSortIndicator(2, Qt::AscendingOrder);
        headerGoods->setSortIndicatorShown(true);
        connect(headerGoods, SIGNAL(sectionClicked(int)), ui->tabMI, SLOT (sortByColumn(int)));

        m_TableHeader.clear();

        //Definiendo la tabla del CS, P1

        ui->tabCSP1->setColumnCount(3);
        m_TableHeader<<"Vj"<<"IOj"<<"Cj";
        ui->tabCSP1->setHorizontalHeaderLabels(m_TableHeader);
        ui->tabCSP1->verticalHeader()->setVisible(false);
        ui->tabCSP1->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tabCSP1->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tabCSP1->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tabCSP1->setShowGrid(true);
        //ui->tabCSP1->setStyleSheet("QTableView {selection-background-color: cyan;}");

        headerGoods = ui->tabCSP1->horizontalHeader();
        headerGoods->setSortIndicator(2, Qt::AscendingOrder);
        headerGoods->setSortIndicatorShown(true);
        connect(headerGoods, SIGNAL(sectionClicked(int)), ui->tabCSP1, SLOT (sortByColumn(int)));


        //Definiendo la tabla del CS, P2

        ui->tabCSP2->setColumnCount(3);
        ui->tabCSP2->setHorizontalHeaderLabels(m_TableHeader);
        ui->tabCSP2->verticalHeader()->setVisible(false);
        ui->tabCSP2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tabCSP2->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tabCSP2->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tabCSP2->setShowGrid(true);
        //ui->tabCSP2->setStyleSheet("QTableView {selection-background-color: cyan;}");

        headerGoods = ui->tabCSP2->horizontalHeader();
        headerGoods->setSortIndicator(2, Qt::AscendingOrder);
        headerGoods->setSortIndicatorShown(true);
        connect(headerGoods, SIGNAL(sectionClicked(int)), ui->tabCSP2, SLOT (sortByColumn(int)));

        //Definiendo la tabla del CS, P3

        ui->tabCSP3->setColumnCount(3);
        ui->tabCSP3->setHorizontalHeaderLabels(m_TableHeader);
        ui->tabCSP3->verticalHeader()->setVisible(false);
        ui->tabCSP3->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tabCSP3->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tabCSP3->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tabCSP3->setShowGrid(true);
        //ui->tabCSP3->setStyleSheet("QTableView {selection-background-color: cyan;}");

        headerGoods = ui->tabCSP3->horizontalHeader();
        headerGoods->setSortIndicator(2, Qt::AscendingOrder);
        headerGoods->setSortIndicatorShown(true);
        connect(headerGoods, SIGNAL(sectionClicked(int)), ui->tabCSP3, SLOT (sortByColumn(int)));


        //Definiendo la tabla del CS, P4

        ui->tabCSP4->setColumnCount(3);
        ui->tabCSP4->setHorizontalHeaderLabels(m_TableHeader);
        ui->tabCSP4->verticalHeader()->setVisible(false);
        ui->tabCSP4->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tabCSP4->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tabCSP4->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tabCSP4->setShowGrid(true);
        //ui->tabCSP4->setStyleSheet("QTableView {selection-background-color: cyan;}");

        headerGoods = ui->tabCSP4->horizontalHeader();
        headerGoods->setSortIndicator(2, Qt::AscendingOrder);
        headerGoods->setSortIndicatorShown(true);
        connect(headerGoods, SIGNAL(sectionClicked(int)), ui->tabCSP4, SLOT (sortByColumn(int)));

    }

    //Gráfica

    QCustomPlot *customPlot = new QCustomPlot();
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setInteraction(QCP::iRangeZoom, true);
    customPlot->axisRect()->setupFullAxesBox();

    customPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(customPlot, "Algoritmo Cultural");
    customPlot->plotLayout()->addElement(0, 0, title);

    customPlot->xAxis->setLabel("Generación");
    customPlot->xAxis->setRange(0, mw.getUI().numGeneraciones->value());

    customPlot->yAxis->setLabel("FO");
    customPlot->yAxis->setRange(0, mw.getUI().valorA->value() * mw.getUI().maxPA->value());

    customPlot->legend->setVisible(true);

    customPlot->addGraph();
    customPlot->graph()->setName("Mejor Individuo");


    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(customPlot, 0, 0);   
    ui->tabGrafica->setLayout(gridLayout);


    //Corrección para múltiples ejecuciones del AC.
    {
        if (!EspaciodePoblacion.isEmpty())
        {
            EspaciodePoblacion.clear();
        }

        if (!cn.estaVacio())
        {
            cn.limpiar();
        }

        if (!cs.estaVacio())
        {
            cs.limpiar();
        }

    }

    std::cout <<"=== INICIO DEL ALGORITMO CULTURAL ===\n\n";

    randomize();

    int t = 0;

    //Inicializando el Espacio de población

    for (int i = 0; i < mw.getUI().numIndividuos->value(); i++)
    {
        EspaciodePoblacion.push_back
                (Individuo(
                     numAleatorio(mw.getUI().minP1->value(), mw.getUI().maxP1->value()),
                     numAleatorio(mw.getUI().minP2->value(), mw.getUI().maxP2->value()),
                     numAleatorio(mw.getUI().minP3->value(), mw.getUI().maxP3->value()),
                     numAleatorio(mw.getUI().minP4->value(), mw.getUI().maxP4->value())));
    }

    //El 20% de la población

    int presion = round(EspaciodePoblacion.size() * 20./100.);

    //Inicializando el Conocimiento Normativo

    cn.insertarLimite(Limite(mw.getUI().minP1->value(),mw.getUI().maxP1->value()));
    cn.insertarLimite(Limite(mw.getUI().minP2->value(),mw.getUI().maxP2->value()));
    cn.insertarLimite(Limite(mw.getUI().minP3->value(),mw.getUI().maxP3->value()));
    cn.insertarLimite(Limite(mw.getUI().minP4->value(),mw.getUI().maxP4->value()));

    do
    {
        std::cout<<"\tGeneración: "<<t<<"\n";

        //Evaluando la población

        evaluarP(EspaciodePoblacion, mw.getUI().valorA->value(), mw.getUI().minPA->value(),mw.getUI().maxPA->value(), mw.getUI().valorB->value()
                 , mw.getUI().minLE->value(),mw.getUI().maxLE->value());

        //Ordenando el Espacio de población de Mayor a Menor

        std::sort(EspaciodePoblacion.begin(), EspaciodePoblacion.end(), &MayorAMenor<Individuo>);

        std::cout<<"   Población Evaluada y Ordenada: \n";
        verP(EspaciodePoblacion);

        //https://robologs.net/2015/09/01/como-programar-un-algoritmo-genetico-parte-ii-implementacion-en-python/

        //Seleccionando el 20% del espacio de población

        QVector<Individuo> aceptados = obtenerAceptados(EspaciodePoblacion, presion);

        std::cout<<"   Los aceptados: ["<<presion<<"]\n";
        std::cout<<aceptados<<std::endl;

        //Actualizando el Conocimiento Normativo

        actualizarCN(cn, aceptados);

        //Actualizando el Conocimiento Situacional

        actualizarCS(cs, aceptados, t, mw.getUI().valorMiu->value());

        std::cout<<"   Conocimiento normativo (después de la actualización):\n"<<cn<<std::endl;
        std::cout<<"   Conocimiento situacional:\n"<<cs;

        //Actualizando Tablas
        {            
            //Actualizando tabla Conocimiento Normativo:

            ui->tabCN->insertRow(t);
            ui->tabCN->setItem(t, 0, new QTableWidgetItem(QString::number(t)));
            ui->tabCN->setItem(t, 1, new QTableWidgetItem(cn.aQString()));

            //Actualizando tabla Mejor Individuo

            ui->tabMI->insertRow(t);
            ui->tabMI->setItem(t, 0, new QTableWidgetItem(QString::number(t)));
            ui->tabMI->setItem(t, 1, new QTableWidgetItem(aceptados[0].aQString()));
            ui->tabMI->setItem(t, 2, new QTableWidgetItem(QString::number(aceptados[0].obtenerFO())));
        }

        // Influencia

        // Cruce...

        cruce(EspaciodePoblacion, aceptados, mw.getUI().probCruce->value(), presion);

        std::cout<<"\n   Nueva Población (después del cruce):\n";
        verP(EspaciodePoblacion);

        // Mutación...

        mutacion(EspaciodePoblacion, mw.getUI().probMutacion->value(), presion);

        std::cout<<"\n   Nueva Población (después de la mutación):\n";
        verP(EspaciodePoblacion);

        t++;

        std::cout<<"\n";

    }while(t < mw.getUI().numGeneraciones->value());

    std::cout <<"=== FIN DEL ALGORITMO CULTURAL ===\n\n";

    // Dibujando la curva del mejor individuo

    QVector<double> x, y;

    for (int i = 0; i < ui->tabMI->model()->rowCount(); ++i)
    {
        x.push_back(ui->tabMI->model()->index(i,0).data().toDouble());
        y.push_back(ui->tabMI->model()->index(i,2).data().toDouble());
    }

    customPlot->graph()->setData(x, y);

}

AC::~AC()
{    
    delete ui;
}

void AC::on_botonRegresar_clicked()
{    
    close();
    mw.show();
}

void AC::on_botonSalir_clicked()
{    
    close();
    mw.close();
}
