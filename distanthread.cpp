#include "distanthread.h"
#include "QDialog"
#include <QDebug>
#include <QtMath>
#include <cstdlib>
#include "defcomunes.h"
#include <QProcess>
#include <QFile>
#include<QTextStream>
#include <QDir>
#include <QHash>
#include <QPair>

// Estructura para almacenar los coeficientes de Sigma.txt
// Clave: medio (int), Valor: QHash<Sigma, coeficiente>
static QHash<int, QHash<double, double>> coeficientesSigma;

distanThread::distanThread(QObject *parent) :
    QThread(parent)
{
    ObjLoc = false;
    Vuelo=0;
    fichero="";
    detener=true;
    probabilidad = 0;
    distancia = 0;
    distanciamax = 0;
    distanciamaxOficial = 0;
    zonamuerta = 0;
    muis = false;
    alcance = false;
    sigmaSeleccionado = 1.0;   // Por defecto: lectura normal
    tipoOnda = "";             // Por defecto: sin tipo de onda

    inicializarCoeficientesSigma();
}
distanThread::~distanThread()
{
    detener=false;
    for (int i = 0; i < 360; ++i) {
        //      disLimites[i].clear();
        disVDirecta[i].clear();
        disInternas[i].clear();
        ZonaMuertaMuis[i].clear();
        ZonaMuertaMuis2[i].clear();
    }
}
void distanThread::stop()
{
    detener=false;
}
void distanThread::calcDistancia()
{
    disMax.clear();
    for (int a = 0; a < 360; a++)
    {
        disInternas[a].clear();
        disVDirecta[a].clear();
        ZonaMuertaMuis[a].clear();
        ZonaMuertaMuis2[a].clear();
        ZonaMuertaCCAA[a].clear();
    }
    parametro = 0;

    // Construcción de nombre para medios especiales (grupo 1)
    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 ||
        calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 ||
        calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 ||
        calculos->medio == 27 || calculos->medio == 26 || calculos->medio == 25 ||
        calculos->medio == 24 || calculos->medio == 23 || calculos->medio == 22 ||
        calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 ||
        calculos->medio == 44 || calculos->medio == 40) && calculos->grupo == 1)
    {
        fichero.append("_" + QString::number(velocidad) + "_" +
                       QString::number(tiempo) + "_" + QString::number(parametro));
        nombreAux.append("_" + QString::number(velocidad) + "_" +
                         QString::number(tiempo) + "_" + QString::number(parametro));
    }

    // === GUARDAR NOMBRE LIMPIO ANTES DE AÑADIR SUFIJOS SIGMA ===
    QString nombreBaseLimpio = fichero;

    if(!ObjLoc)
    {
        QString extension = obtenerExtension(Vuelo);
        QString sufijoSigma;
        QString sufijoTipo;

        if (sigmaSeleccionado == 5.0) sufijoSigma = "_S5";
        else if (sigmaSeleccionado == 0.5) sufijoSigma = "_S05";
        else if (sigmaSeleccionado == 0.1) sufijoSigma = "_S01";

        if (calculos->medio == 40 && sigmaSeleccionado != 1.0) {
            sufijoTipo = "_" + tipoOnda;
        }

        nombreAux = nombreAux + sufijoSigma + sufijoTipo + extension;
        fichero = fichero + sufijoSigma + sufijoTipo + extension;
    }
    else
    {
        QString sufijoSigma;
        QString sufijoTipo;

        if (sigmaSeleccionado == 5.0) sufijoSigma = "_S5";
        else if (sigmaSeleccionado == 0.5) sufijoSigma = "_S05";
        else if (sigmaSeleccionado == 0.1) sufijoSigma = "_S01";

        if (calculos->medio == 40 && sigmaSeleccionado != 1.0) {
            sufijoTipo = "_" + tipoOnda;
        }

        nombreAux = nombreAux + sufijoSigma + sufijoTipo + ".ObjL";
        fichero = fichero + sufijoSigma + sufijoTipo + ".ObjL";
    }

    emit progreso(0, "Calculando datos para " + QString::number(Vuelo) +
                  "m. Esta operación puede tardar unos minutos");


    if(!leer())
    {
        tangente(calculos->angulositio);

        double AltuCurva=0;
        double Alpos = calculos->datoterrenoMiPos.altura;

        double posicion = Alpos + calculos->AlAnt;
        double curva=0;
        double recta=0;
        double recta1=0;
        double recta2=0;

        if(calculos->medio != 50)
        {
            if(Vuelo >= 0 && Vuelo <= 1000)//100 a 2000
                Disvuelo=1000*(4.12*(sqrt(Vuelo)+sqrt(posicion)));
            else if(Vuelo == 2000)
                Disvuelo=1000*(4.12*0.98*(sqrt(Vuelo)+sqrt(posicion)));
            else if(Vuelo == 4000)
                Disvuelo=1000*(4.12*0.95*(sqrt(Vuelo)+sqrt(posicion)));
        }
        else
        {
            Disvuelo=1000*(4.12*(sqrt(Vuelo)+sqrt(posicion)));
        }
        if(Vuelo == 6000)
        {
            Disvuelo=1000*(4.12*0.9*(sqrt(Vuelo)+sqrt(posicion)));
            if(calculos->medio == 50 )
                Disvuelo=1000*(4.12*0.99*(sqrt(Vuelo)+sqrt(posicion)));
        }
        else if(Vuelo == 10000)
        {
            Disvuelo=1000*(4.12*0.8*(sqrt(Vuelo)+sqrt(posicion)));
            if(calculos->medio == 50 )
                Disvuelo=1000*(4.12*0.94*(sqrt(Vuelo)+sqrt(posicion)));
        }
        else if(Vuelo == 17000)
        {
            Disvuelo=1000*(4.12*0.7*(sqrt(Vuelo)+sqrt(posicion)));
            if(calculos->medio == 50)
                Disvuelo=1000*(4.12*0.91*(sqrt(Vuelo)+sqrt(posicion)));
        }
        else if(Vuelo == 30000)
        {
            Disvuelo=1000*(4.12*0.6*(sqrt(Vuelo)+sqrt(posicion )));
            if(calculos->medio == 50)
                Disvuelo=1000*(4.12*0.9*(sqrt(Vuelo)+sqrt(posicion)));
        }
        if(calculos->medio == 41 && Disvuelo > 180000)
            Disvuelo = 180000;

        if(!ObjLoc)
            if(Disvuelo < distanciamax)
            {
                distanciamax = Disvuelo;
            }

        disMax.append (distanciamax);//0.5

        double disaux = 0;
        disaux = distanciamax*0.68630039*1.35;
        disMax.append (disaux);//0.6
        disaux = distanciamax*0.62735625*1.35;
        disMax.append (disaux);//0.7
        disaux = distanciamax*0.5576305*1.35;
        disMax.append (disaux);//0.8
        disaux = distanciamax*0.46250405*1.35;
        disMax.append (disaux);//0.9
        qDebug()<<"parar"<<detener;
        for (int a=0; a < 360; a++)
        {
            listaASTodos.clear ();
            LPinterceptosAS1.clear();
            LPinterceptosAS2.clear();
            LPinterceptosRecta.clear();

            int d = 0;
            if(!detener)
            {
                emit findehilo(false);
                detener=true;
                return;
            }


            emit progreso(a,"Calculando datos para "+ QString::number(Vuelo)+"m. Esta operación puede tardar unos minutos");

            QList<CDatoTerreno*> *datosterreno = &calculos->DatosAzimut[a].datosterreno;
            QList<double> datosCurva;
            QList<double> datosRecta;

            QList<double> datosRectaASitio1;//datos de la recta  de angulo de sitio 1
            QList<double> datosRectaASitio2;//datos de la recta  de angulo de sitio -1

            int ac = 0;
            int length = calculos->DatosAzimut[a].datosAngulosCierre.length();//tamano de la lista de angulos de cierre(picos maximos)
            double dlimt = 0;
            if(length != 0)
                dlimt = calculos->DatosAzimut[a].datosAngulosCierre.at(length>1?1:0)->distancia;//distancia de los picos maximos
            AltuCurva = Alpos;
            int ciclo = distancia/90;
            if(calculos->medio == 50)
                ciclo = 7777;
            for(int i=0; i < ciclo; i++)
            {
                if(Vuelo > 1000)
                {
                    curva = -(pow(d,2))/(17000000) + Vuelo;//funcion que define la curva a diferentes alturas mayores que 1000m sin el relieve del terreno
                    datosCurva.append(curva);
                }
                else
                {
                    curva = AltuCurva+Vuelo;//funcion que define la curva a diferentes alturas menores que 1000m con el relieve del terreno
                    datosCurva.append(curva);
                    if(i<3556)
                        AltuCurva=datosterreno->at(i)->altura;
                }
                if(ac < length)
                {
                    if(d > dlimt)
                    {
                        if(ac < length - 1)
                        {
                            ac++;
                            if(ac < length - 1)
                                dlimt = calculos->DatosAzimut[a].datosAngulosCierre.at(ac+1)->distancia;
                        }
                    }
                    if(d>0)
                        recta = calculos->DatosAzimut[a].datosAngulosCierre.at(ac)->tangente * d +posicion;
                    else
                    {
                        recta = Alpos;
                    }
                    if(calculos->medio == 41 || calculos->medio == 48 )
                    {
                        recta1 = tang1 * d +posicion;
                        recta2 = tang2 * d +posicion;
                        datosRectaASitio1.append(recta1);
                        datosRectaASitio2.append(recta2);
                    }
                    if(calculos->medio == 39  && calculos->grupo == 1)
                    {
                        //tan -3 == -0.052407779
                        recta1 = -0.052407779 *d +posicion;
                        datosRectaASitio1.append(recta1);

                    }
                    if(calculos->medio == 38 && calculos->grupo == 1 )
                    {
                        //tan -2 == -0.034920769
                        recta1 = -0.034920769 *d +posicion;
                        datosRectaASitio1.append(recta1);
                    }

                }
                datosRecta.append(recta);
                d+=90;
            }

            if(calculos->medio == 41 || calculos->medio == 48)
            {
                muis = true;

                LPinterceptosAS1 = Pointinterceptos (false,a,datosCurva,datosRectaASitio1);
                LPinterceptosAS2 = Pointinterceptos (false,a,datosCurva,datosRectaASitio2);

                if(!LPinterceptosAS1.isEmpty () && Vuelo == 0)
                    disInternas[a].append (LPinterceptosAS1.first ().x ());
            }
            else if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio==25 || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1)
            {
                ZonaMuertaMuis[a].append(0);
                LPinterceptosAS1 = Pointinterceptos (false,a,datosCurva,datosRectaASitio1);
            }

            else
            {
                ZonaMuertaMuis[a].append(0);
                ZonaMuertaMuis2[a].append(0);
            }
            muis = false;

            LPinterceptosRecta .append (Pointinterceptos (true,a,datosCurva,datosRecta));

            if(!LPinterceptosRecta.isEmpty ())
                LPinterceptosRecta = listaPuntos(LPinterceptosRecta);//ordenar lista de puntos por distancias
            /*m41 48*/
            if(calculos->medio == 41 || calculos->medio == 48)
            {
                if(Vuelo == 0)
                {
                    if(!LPinterceptosAS1.isEmpty ())
                        ZonaMuertaMuis[a].append (LPinterceptosAS1.first ().x ());
                    if(!LPinterceptosAS2.isEmpty ())
                        ZonaMuertaMuis2[a].append (LPinterceptosAS2.first ().x ());


                    if(LPinterceptosAS2.isEmpty())
                    {
                        LPinterceptosRecta.clear();
                        disVDirecta[a].append(0);
                    }
                    else
                    {
                        double o = 0.0;
                        double o1 = 0.0;
                        double o2 = 0.0;
                        double o3 = 0.0;
                        double A1y = posicion;
                        double A1x = 0;
                        double A2y = datosRectaASitio1.last();
                        double A2x = datosRectaASitio1.length()*90;
                        double A3x = datosRectaASitio2.length()*90;
                        double A3y = datosRectaASitio2.last();

                        for (int i = 0; i < LPinterceptosRecta.length(); ++i)
                        {
                            if(LPinterceptosRecta.at (i).x () <= distanciamax)
                            {
                                double px = LPinterceptosRecta.at(i).x();
                                double py = LPinterceptosRecta.at(i).y();

                                o = (A1x-A3x)*(A2y-A3y)-(A1y-A3y)*(A2x-A3x);

                                o1 = (A1x-px)*(A2y-py)-(A1y-py)*(A2x-px);

                                o2 = (A2x-px)*(A3y-py)-(A2y-py)*(A3x-px);

                                o3 = (A3x-px)*(A1y-py)-(A3y-py)*(A1x-px);

                                if(o<0)
                                {
                                    if(o1<0 && o2<0 && o3<0)//adentro
                                    {
                                        disInternas[a].append(LPinterceptosRecta.at(i).x ());
                                    }
                                }
                                else
                                {
                                    if(o1>0 && o2>0 && o3>0)//adentro
                                    {
                                        disInternas[a].append(LPinterceptosRecta.at(i).x ());
                                    }
                                }
                            }
                            else
                            {
                                LPinterceptosRecta.removeAt (i);
                            }
                        }//fin ciclo
                        if(!disInternas[a].isEmpty ())
                        {
                            disInternas[a] = listaNumeros (disInternas[a]);
                            disVDirecta[a].append(disInternas[a].last ());
                            disInternas[a].removeLast ();
                        }
                        else
                        {
                            if(!LPinterceptosAS1.isEmpty ())
                                disVDirecta[a].append (LPinterceptosAS1.first ().x ());
                            else
                            {
                                for (int i = 0; i < datosCurva.size (); ++i)
                                {
                                    if(i*90 > distanciamax)
                                    {
                                        if(datosCurva.at (i-1) > datosRecta.at (i-1) && datosCurva.at (i-1) < datosRectaASitio1.at (i-1) && datosCurva.at (i-1) > datosRectaASitio2.at (i-1))
                                        {
                                            disVDirecta[a].append ((i-1)*90);
                                            break;
                                        }
                                    }
                                }
                                if(disVDirecta[a].isEmpty ())
                                    disVDirecta[a].append (0);

                            }
                        }
                        //                            }
                    }
                }
                else//vuelo>0
                {
                    if(LPinterceptosAS1.isEmpty() && LPinterceptosAS2.isEmpty())
                    {
                        LPinterceptosRecta.clear();
                        disVDirecta[a].append(0);
                    }
                    else
                    {
                        double o = 0.0;
                        double o1 = 0.0;
                        double o2 = 0.0;
                        double o3 = 0.0;
                        double A1y = posicion;
                        double A1x = 0;
                        double A2y = datosRectaASitio1.last();
                        double A2x = datosRectaASitio1.length()*90;
                        double A3x = datosRectaASitio2.length()*90;
                        double A3y = datosRectaASitio2.last();

                        for (int i = 0; i < LPinterceptosRecta.length(); ++i)
                        {
                            if(i < LPinterceptosRecta.length()-1)
                            {
                                double px = LPinterceptosRecta.at(i).x();
                                double py = LPinterceptosRecta.at(i).y();

                                o = (A1x-A3x)*(A2y-A3y)-(A1y-A3y)*(A2x-A3x);

                                o1 = (A1x-px)*(A2y-py)-(A1y-py)*(A2x-px);

                                o2 = (A2x-px)*(A3y-py)-(A2y-py)*(A3x-px);

                                o3 = (A3x-px)*(A1y-py)-(A3y-py)*(A1x-px);

                                if(o<0)
                                {
                                    if(o1<0 && o2<0 && o3<0)
                                    {
                                        // qDebug()<<a<<"adentro";
                                        if( LPinterceptosRecta.at(i).x () < distanciamax )
                                            disInternas[a].append(LPinterceptosRecta.at(i).x ());

                                    }
                                    else
                                    {
                                        // qDebug()<<a<<"fuera";
                                    }
                                }
                                else
                                {
                                    if(o1>0 && o2>0 && o3>0)
                                    {
                                        // qDebug()<<a<<"adentro";
                                        if(LPinterceptosRecta.at(i).x () < distanciamax)
                                            disInternas[a].append(LPinterceptosRecta.at(i).x ());
                                    }
                                    else
                                    {}// qDebug()<<a<<"fuera";
                                }
                            }
                            else
                            {
                                double px = LPinterceptosRecta.at(i).x();
                                double py = LPinterceptosRecta.at(i).y();

                                o = (A1x-A3x)*(A2y-A3y)-(A1y-A3y)*(A2x-A3x);

                                o1 = (A1x-px)*(A2y-py)-(A1y-py)*(A2x-px);

                                o2 = (A2x-px)*(A3y-py)-(A2y-py)*(A3x-px);

                                o3 = (A3x-px)*(A1y-py)-(A3y-py)*(A1x-px);

                                if(o<0)
                                {
                                    if(o1<0 && o2<0 && o3<0)
                                    {
                                        // qDebug()<<a<<"adentro";
                                        if(LPinterceptosRecta.last ().x ()< distanciamax)
                                        {
                                            if(datosCurva.at (distanciamax/90-1) > datosRecta.at (distanciamax/90-1))
                                            {
                                                disInternas[a].append(LPinterceptosRecta.last().x ());
                                                disVDirecta[a].append (distanciamax);
                                            }
                                            else
                                            {
                                                if( disInternas[a].size ()>1)
                                                {
                                                    if(LPinterceptosRecta.size ()%2 == 0)
                                                    {
                                                        disVDirecta[a].append (disInternas[a].last ());
                                                        disInternas[a].removeLast ();
                                                    }
                                                    else
                                                    {
                                                        disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                                    }

                                                }
                                                else
                                                {
                                                    disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                                    disInternas[a].clear ();
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if( disInternas[a].size ()>1)
                                            {
                                                if(LPinterceptosRecta.size ()%2 == 0)
                                                {
                                                    disVDirecta[a].append (disInternas[a].last ());
                                                    disInternas[a].removeLast ();
                                                }
                                                else
                                                {
                                                    disVDirecta[a].append (distanciamax);
                                                }
                                            }
                                            else
                                            {
                                                disVDirecta[a].append (distanciamax);
                                                disInternas[a].clear ();
                                            }
                                        }
                                    }
                                    else
                                    {
                                        // qDebug()<<a<<"fuera";
                                    }
                                }
                                else
                                {
                                    if(o1>0 && o2>0 && o3>0)
                                    {
                                        //qDebug()<<a<<"adentro";
                                        if(LPinterceptosRecta.last ().x ()< distanciamax)
                                        {
                                            if(datosCurva.at (distanciamax/90-1) > datosRecta.at (distanciamax/90-1))
                                            {
                                                disInternas[a].append(LPinterceptosRecta.last().x ());
                                                disVDirecta[a].append (distanciamax);
                                            }
                                            else
                                            {
                                                if( disInternas[a].size ()>1)
                                                {
                                                    if(LPinterceptosRecta.size ()%2 == 0)
                                                    {
                                                        disVDirecta[a].append (disInternas[a].last ());
                                                        disInternas[a].removeLast ();
                                                    }
                                                    else
                                                    {
                                                        disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                                    }

                                                }
                                                else
                                                {
                                                    disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                                    disInternas[a].clear ();
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if( disInternas[a].size ()>1)
                                            {
                                                if(LPinterceptosRecta.size ()%2 == 0)
                                                {
                                                    disVDirecta[a].append (disInternas[a].last ());
                                                    disInternas[a].removeLast ();
                                                }
                                                else
                                                {
                                                    disVDirecta[a].append (distanciamax);
                                                }
                                            }
                                            else
                                            {
                                                disVDirecta[a].append (distanciamax);
                                                disInternas[a].clear ();
                                            }
                                        }
                                    }
                                    else
                                    {}// qDebug()<<a<<"fuera";
                                }
                            }
                        }
                        if(disVDirecta[a].isEmpty ())
                        {
                            if(!disInternas[a].isEmpty ())
                            {
                                disVDirecta[a].append (disInternas[a].last ());
                                disInternas[a].removeLast ();
                            }
                            else
                            {
                                for (int i = 0; i < datosCurva.size (); ++i)
                                {
                                    if(i*90 > distanciamax)
                                    {
                                        if(datosCurva.at (i-1) > datosRecta.at (i-1) && datosCurva.at (i-1) < datosRectaASitio1.at (i-1) && datosCurva.at (i-1) > datosRectaASitio2.at (i-1))
                                        {
                                            disVDirecta[a].append ((i-1)*90);
                                            break;
                                        }
                                    }
                                }
                                if(disVDirecta[a].isEmpty ())
                                {
                                    listaASTodos.append (LPinterceptosAS2);
                                    listaASTodos.append (LPinterceptosAS1);
                                    if(!listaASTodos.isEmpty ())
                                        listaASTodos = listaPuntos (listaASTodos);
                                    if(!listaASTodos.isEmpty ())
                                    {
                                        for (auto listaASTodo : listaASTodos)
                                        {
                                            if(listaASTodo.y () > datosRecta.at (listaASTodo.x ()/90))
                                            {
                                                disVDirecta[a].clear ();
                                                disVDirecta[a].append (listaASTodo.x ());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if(disVDirecta[a].isEmpty ())
                        disVDirecta[a].append (0);
                    for (auto i : LPinterceptosAS2)
                    {
                        if(i.x () < disVDirecta[a].at (0) )
                            ZonaMuertaMuis2[a].append (i.x ());
                    }
                    for (auto i : LPinterceptosAS1)
                    {
                        if(i.x () < disVDirecta[a].at (0) )
                            ZonaMuertaMuis[a].append (i.x ());
                    }
                }
            }
            else
                /*m otros*/{
                double disaux = distanciamax;

                if(Vuelo==0)
                {
                    if (LPinterceptosRecta.isEmpty ())
                    {
                        disVDirecta[a].append (0);
                    }
                    else
                    {
                        for (int i = 0; i < LPinterceptosRecta.length(); ++i)
                        {
                            if(i < LPinterceptosRecta.length()-1)
                            {
                                if(LPinterceptosRecta.at(i).x () < disaux)
                                    disInternas[a].append(LPinterceptosRecta.at(i).x ());
                                else
                                {
                                    disVDirecta[a].append(disaux);
                                    break;
                                }
                            }
                            else
                            {
                                if(LPinterceptosRecta.last ().x ()< disaux)
                                {
                                    if(datosCurva.at (ciclo-1) > datosRecta.at (ciclo-1))
                                    {
                                        disInternas[a].append(LPinterceptosRecta.last().x ());
                                        disVDirecta[a].append (disaux);
                                    }
                                    else
                                    {
                                        if( disInternas[a].size ()>1)
                                        {
                                            if(LPinterceptosRecta.size ()%2 == 0)
                                            {
                                                disVDirecta[a].append (disInternas[a].last ());
                                                disInternas[a].removeLast ();
                                            }
                                            else
                                            {
                                                disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                            }
                                        }
                                        else
                                        {
                                            disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                            disInternas[a].clear ();
                                        }
                                    }
                                }
                                else
                                {
                                    QPoint paux;
                                    paux.setX (disaux);
                                    paux.setY (LPinterceptosRecta.last ().y ());
                                    LPinterceptosRecta.removeLast ();
                                    LPinterceptosRecta.append (paux);
                                    if(datosCurva.at (ciclo-1) > datosRecta.at (ciclo-1))
                                    {
                                        disVDirecta[a].append (disaux);
                                    }
                                    else
                                    {
                                        if( disInternas[a].size ()>1)
                                        {
                                            if(LPinterceptosRecta.size ()%2 == 0)
                                            {
                                                disVDirecta[a].append (disInternas[a].last ());
                                                disInternas[a].removeLast ();
                                            }
                                            else
                                            {
                                                disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                            }
                                        }
                                        else
                                        {
                                            disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                            disInternas[a].clear ();
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio==25 || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1)
                    {
                        if(!LPinterceptosAS1.isEmpty ())
                        {
                            for (auto i : LPinterceptosAS1)
                            {
                                if(i.x () < disVDirecta[a].at (0) )
                                    /*ZonaMuertaCCAA*/ZonaMuertaMuis2[a].append (i.x ());
                            }
                        }
                    }
                }
                else//vuelo >0
                {
                    if (LPinterceptosRecta.isEmpty ())
                    {
                        disVDirecta[a].append (disaux);
                    }
                    for (int i = 0; i < LPinterceptosRecta.length(); ++i)
                    {
                        if(i < LPinterceptosRecta.length()-1)
                        {
                            if(LPinterceptosRecta.at(i).x () < disaux)
                                disInternas[a].append(LPinterceptosRecta.at(i).x ());
                            else
                            {
                                if(disInternas[a].size ()%2 != 0)
                                {
                                    disVDirecta[a].append (disInternas[a].last ());
                                    disInternas[a].removeLast ();
                                }
                                else
                                {
                                    disVDirecta[a].append(disaux/*LPinterceptosRecta.at(i).x ()*/);
                                }


                                break;
                            }
                        }
                        else
                        {

                            if(LPinterceptosRecta.last ().x ()< disaux)
                            {
                                if(datosCurva.at (ciclo-1) > datosRecta.at (ciclo-1))
                                {
                                    disInternas[a].append(LPinterceptosRecta.last().x ());
                                    disVDirecta[a].append (disaux);
                                }
                                else
                                {
                                    if( disInternas[a].size ()>1)
                                    {
                                        if(LPinterceptosRecta.size ()%2 == 0)
                                        {
                                            disVDirecta[a].append (disInternas[a].last ());
                                            disInternas[a].removeLast ();
                                        }
                                        else
                                        {
                                            disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                        }
                                    }
                                    else
                                    {
                                        disVDirecta[a].append (LPinterceptosRecta.last().x ());
                                        disInternas[a].clear ();
                                    }
                                }
                            }
                            else
                            {
                                if(datosCurva.at (ciclo-1) > datosRecta.at (ciclo-1))
                                {
                                    disVDirecta[a].append (disaux);
                                }
                                else
                                {
                                    if( disInternas[a].size ()>1)
                                    {
                                        if(LPinterceptosRecta.size ()%2 == 0)
                                        {
                                            disVDirecta[a].append (disInternas[a].last ());
                                            disInternas[a].removeLast ();
                                        }
                                        else
                                        {
                                            disVDirecta[a].append (disaux);
                                        }
                                    }
                                    else
                                    {
                                        disVDirecta[a].append (disaux);
                                        disInternas[a].clear ();
                                    }
                                }
                            }
                        }
                    }
                    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio == 25 || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1)
                    {
                        if(!LPinterceptosAS1.isEmpty ())
                        {
                            for (auto i : LPinterceptosAS1)
                            {
                                if(i.x () < disVDirecta[a].at (0) )
                                    /*ZonaMuertaCCAA*/ZonaMuertaMuis2[a].append (i.x ());
                            }
                        }
                    }
                }
            }
            //arreglar lista disVDirecta
            corrige(a);
        }
        guardar ();
        guarda = true;

        // === BLOQUE SIGMA ===
        bool medioRequiereSigma = (calculos->medio == 36 || calculos->medio == 38 ||
                                   calculos->medio == 39 || calculos->medio == 40 ||
                                   calculos->medio == 42 || calculos->medio == 43 ||
                                   calculos->medio == 44);

        if (calculos->grupo == 1 && (Vuelo <= 500 || Vuelo >= 1000) &&
                medioRequiereSigma && sigmaSeleccionado != 1.0) {

            if (calculos->medio == 40 && sigmaSeleccionado != 1.0 && tipoOnda.isEmpty()) {
                qDebug() << "ADVERTENCIA: Medio 40 sin tipo de onda. Usando M por defecto.";
                tipoOnda = "M";
            }

            calcularYGuardarSigma(calculos->medio, calculos->grupo, Vuelo,
                                  disMax, nombreBaseLimpio);
        }
        // === FIN BLOQUE SIGMA ===

        emit progreso(0, "Mostrando gráfica para " + QString::number(Vuelo) +
                      "m. Esta operación puede tardar unos minutos ");
        emit findehilo(true);
    }
    else
    {
        //        nombreAux.clear();
        guarda = false;
        emit progreso(0,"Mostrando gráfica para "+ QString::number(Vuelo)+"m. Esta operación puede tardar unos minutos ");
        emit findehilo(true);
    }
}

QList<QPointF> distanThread::Pointinterceptos(bool recttierra,int a, QList<double> datosCurva, QList<double> datosRecta)
{
    QList<double> interceptos;
    QList<QPointF> LPinterceptos;
    QPointF p;

    double cu=0;
    double re=0;
    double mod=0;

    double auxre = 0;
    double auxcu = 0;
    double auxre1 = 0;
    double auxcu1 = 0;
    double mcu = 0;
    double mre = 0;
    double ycu = 0;
    double yre = 0;
    double dist = 0;
    char Signo = '+';
    char Signo2 = '+';

    bool tan = true;
    if(Vuelo > 0)
    {
        for (int j = 1; j < datosRecta.length(); j++)//busco intercepto
        {
            re=datosRecta.at(j);
            cu=datosCurva.at(j);
            mod=(cu-re);
            if (mod>0)
            {
                if (Signo=='-')
                {
                    auxre=re;
                    auxcu=cu;
                    auxre1=datosRecta[j-1];
                    auxcu1=datosCurva[j-1];

                    mre=(auxre-auxre1)/90;
                    mcu=(auxcu-auxcu1)/90;
                    for(double x=0;x<=90;x+=0.0001)
                    {
                        ycu=mcu*x+auxcu1;
                        yre=mre*x+auxre1;
                        if (yre<=ycu)
                        {
                            dist=(j-1)*90+(x);
                            interceptos.append(dist);
                            p.setX(dist);
                            p.setY((yre+ycu)/2);
                            LPinterceptos.append(p);
                            break;
                        }
                    }
                    Signo='+';
                }
            }
            if (mod<0)
            {
                if (Signo=='+')
                {
                    auxre=re;
                    auxcu=cu;
                    auxre1=datosRecta[j-1];
                    auxcu1=datosCurva[j-1];

                    mre=(auxre-auxre1)/90;
                    mcu=(auxcu-auxcu1)/90;
                    for(double x=0;x<=90;x+=0.0001)
                    {
                        ycu=mcu*x+auxcu1;
                        yre=mre*x+auxre1;
                        if (ycu<=yre)
                        {
                            dist=(j-1)*90+(x);
                            interceptos.append(dist);
                            p.setX(dist);
                            p.setY((yre+ycu)/2);
                            LPinterceptos.append(p);
                            break;
                        }
                    }
                    Signo='-';
                }
            }
            if (mod == 0)
            {
                if (Signo=='+')
                {
                    auxre=re;
                    auxcu=cu;
                    auxre1=datosRecta[j-1];
                    auxcu1=datosCurva[j-1];

                    mre=(auxre-auxre1)/90;
                    mcu=(auxcu-auxcu1)/90;
                    for(double x=0;x<=90;x+=0.0001)
                    {
                        ycu=mcu*x+auxcu1;
                        yre=mre*x+auxre1;
                        if (ycu<=yre)
                        {
                            dist=(j-1)*90+(x);
                            interceptos.append(dist);
                            p.setX(dist);
                            p.setY((yre+ycu)/2);
                            LPinterceptos.append(p);
                            break;
                        }
                    }
                }
                else
                {
                    auxre=re;
                    auxcu=cu;
                    auxre1=datosRecta[j-1];
                    auxcu1=datosCurva[j-1];

                    mre=(auxre-auxre1)/90;
                    mcu=(auxcu-auxcu1)/90;
                    for(double x=0;x<=90;x+=0.0001)
                    {
                        ycu=mcu*x+auxcu1;
                        yre=mre*x+auxre1;
                        if (yre<=ycu)
                        {
                            dist=(j-1)*90+(x);
                            interceptos.append(dist);
                            p.setX(dist);
                            p.setY((yre+ycu)/2);
                            LPinterceptos.append(p);
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {//revisar cuando altura es 0

        //      if(!muis)
        //        {
        if(recttierra)
        {
            for (auto i : calculos->DatosAzimut[a].datosAngulosCierre)
            {
                if(i->distancia < distanciamax)
                {
                    interceptos.append(i->distancia);
                    p.setX(i->distancia);
                    p.setY(i->altura);
                    LPinterceptos.append(p);
                }
                else
                {
                    //                  if(calculos->medio == 40)
                    //                    {
                    interceptos.append(distanciamax);
                    p.setX(distanciamax);
                    p.setY(datosCurva[distanciamax/90-1]);
                    LPinterceptos.append(p);
                    //                    }
                    break;
                }
            }
            //        }
        }
        for (int j = 1; j < datosRecta.length(); j++)//busco intercepto
        {
            if(interceptos.contains((j-1)*90))
                Signo2 = '+';
            tan = true;
            re=datosRecta.at(j);
            cu=datosCurva.at(j);
            mod=(re-cu);
            if (mod>0)
            {
                if (Signo2=='-')
                {
                    auxre=re;
                    auxcu=cu;
                    auxre1=datosRecta[j-1];
                    auxcu1=datosCurva[j-1];

                    mre=(auxre-auxre1)/90;
                    mcu=(auxcu-auxcu1)/90;
                    for(double x=0;x<=90;x+=0.0001)
                    {
                        ycu=mcu*x+auxcu1;
                        yre=mre*x+auxre1;
                        if (ycu<=yre)
                        {
                            dist=(j-1)*90+(x);
                            if(!interceptos.contains(dist))
                            {
                                interceptos.append(dist);
                                p.setX(dist);
                                p.setY((yre+ycu)/2);
                                LPinterceptos.append(p);
                                break;
                            }

                            tan = false;
                        }
                    }
                    if(tan)
                        Signo2='+';
                }
            }
            if (mod<0)
            {
                if (Signo2=='+')
                {
                    auxre=re;
                    auxcu=cu;
                    auxre1=datosRecta[j-1];
                    auxcu1=datosCurva[j-1];

                    mre=(auxre-auxre1)/90;
                    mcu=(auxcu-auxcu1)/90;
                    for(double x=0;x<=90;x+=0.0001)
                    {
                        ycu=mcu*x+auxcu1;
                        yre=mre*x+auxre1;
                        if (yre<=ycu)
                        {
                            dist=(j-1)*90+(x);

                            if(!interceptos.contains(dist))
                            {
                                interceptos.append(dist);
                                p.setX(dist);
                                p.setY((yre+ycu)/2);
                                LPinterceptos.append(p);
                                break;
                            }

                            tan = false;
                        }
                    }
                    if(tan)
                        Signo2='-';
                }
            }
        }
    }

    return LPinterceptos;
}

QList<QPointF> distanThread::listaPuntos(QList<QPointF> ListaPuntos)//ordena
{

    for (int i = 0; i < ListaPuntos.length (); ++i)
    {
        for (int j = 0; j < ListaPuntos.length (); ++j)
        {
            if(ListaPuntos.at (i).x ()< ListaPuntos.at (j).x ())
            {
                QPointF aux1 = ListaPuntos.at (i);
                QPointF aux2 = ListaPuntos.at (j);
                ListaPuntos.replace (i,aux2);
                ListaPuntos.replace (j,aux1);
            }
        }
    }
    return ListaPuntos;
}

QList<double> distanThread::listaNumeros(QList<double> ListaNumeros)//ordena
{
    qSort (ListaNumeros);
    return ListaNumeros;
}

void distanThread::guardar()
{
    parametro = 0;
    QString auxF = fichero;
    if(alcance)
    {
        auxF.insert (6,"a");
    }
    QFile f(QDir::currentPath() + "/Zonas/"+auxF);
    qDebug()<<"guardar hilo"<<fichero;

    //    creaTablaBaseDatos(auxF.replace(".","_"));
    QTextStream out(&f);

    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal("No puedo abrir el fichero para escritura.");

    double coox = 0;
    double cooy = 0;
    double disaux = 0;
    QList<double> listdisaux;
    QString aux;
    for (int a = 0; a < 360; a++)
    {
        emit progreso(a,"Guardando datos. Esta operación puede tardar unos minutos");

        listdisaux.clear ();
        if(disVDirecta[a].at (0)/1000 <= 320)
        {
            cooy = calculos->DatosAzimut[a].datosterreno.at((int)disVDirecta[a].at (0)/90)->LatOR/*Ymap*/;
            coox = calculos->DatosAzimut[a].datosterreno.at((int)disVDirecta[a].at (0)/90)->LonOR*-1/*Xmap*/;
        }
        else
        {
            cooy = calculos->DatosAzimut[a].datosterreno.last()->LatOR/*Ymap*/;
            coox = calculos->DatosAzimut[a].datosterreno.last()->LonOR*-1/*Xmap*/;
        }
        out << coox << "," << cooy << ",*" << disVDirecta[a].at(0);
        qDebug() <<a<< coox << "," << cooy << ",*" << disVDirecta[a].at(0);


        for (double i : disInternas[a])
        {
            out << "," << i;
            qDebug() << "," << i;

        }
        out << "*";
        qDebug() << "*";

        /***********ZonaMuerta*************/
        if(calculos->medio != 0 && calculos->medio != 11 && calculos->medio != 12 && calculos->medio != 13 && calculos->medio != 40)
        {
            if(ZonaMuertaMuis[a].length()>=1)
            {
                out<< ZonaMuertaMuis[a].at(0);
                qDebug()<< ZonaMuertaMuis[a].at(0);

                if(ZonaMuertaMuis[a].length()>1)
                {
                    for (int i = 1; i < ZonaMuertaMuis[a].length(); ++i)
                    {
                        out << "," << ZonaMuertaMuis[a].at(i);
                        qDebug() << "," << ZonaMuertaMuis[a].at(i);
                    }
                }
            }
            else
            {
                ZonaMuertaMuis[a].append(0);
                out << 0;
                qDebug() << 0;
            }
            out << "*";
            qDebug() << "*";


            if(ZonaMuertaMuis2[a].length()>=1)
            {
                out <<ZonaMuertaMuis2[a].at(0);
                qDebug() <<ZonaMuertaMuis2[a].at(0);

                if(ZonaMuertaMuis2[a].length()>1)
                {
                    for (int i = 1; i < ZonaMuertaMuis2[a].length(); ++i)
                    {
                        out << "," << ZonaMuertaMuis2[a].at(i);
                        qDebug() << "," << ZonaMuertaMuis2[a].at(i);
                    }
                }
            }
            else
            {
                ZonaMuertaMuis2[a].append(0);
                out << 0;
                qDebug() << 0;
            }
        }
        /**************************/
        /*********************probabilidad*************************/
        if(calculos->fichero.contains ("ZVD") && (calculos->medio >= 21))
        {
            out << "*";
            qDebug() << "*";

            for (int i = 0; i < disInternas[a].length(); i++)
            {
                if((i)%2 == 0)
                {
                    if(i > disInternas[a].length()-1 )
                    {
                        disVDirecta[a].replace (0,disInternas[a].last ());
                        disInternas[a].removeLast ();
                        i--;
                    }
                }
            }

            disaux = disVDirecta[a].at (0);
            listdisaux.append (disaux);
            if(disaux > disMax.at (1))//06
            {
                out << disMax.at (1)<<"*";
                qDebug() << disMax.at (1)<<"*";
                listdisaux.append (disMax.at (1));
            }
            else
            {
                out << disaux<<"*";
                qDebug() << disaux<<"*";
                listdisaux.append (disaux);
            }
            if(disaux > disMax.at (2))//07
            {
                out << disMax.at (2)<<"*";
                qDebug() << disMax.at (2)<<"*";
                listdisaux.append (disMax.at (2));
            }
            else
            {
                out << disaux<<"*";
                qDebug() << disaux<<"*";
                listdisaux.append (disaux);
            }

            if(disaux > disMax.at (3))//08
            {
                out << disMax.at (3)<<"*";
                qDebug() << disMax.at (3)<<"*";
                listdisaux.append (disMax.at (3));
            }
            else
            {
                out << disaux<<"*";
                qDebug() << disaux<<"*";
                listdisaux.append (disaux);
            }
            if(disaux > disMax.at (4))//09
            {
                out << disMax.at (4)<<"*";
                qDebug() << disMax.at (4)<<"*";
                listdisaux.append (disMax.at (4));
            }
            else
            {
                out << disaux<<"*";
                qDebug() << disaux<<"*";
                listdisaux.append (disaux);
            }
            disVDirecta[a].replace (0,listdisaux.at (probabilidad));
            for (int j = 0; j < disInternas[a].length(); ++j)
            {
                if(disInternas[a].at(j) > disVDirecta[a].at (0))
                {
                    disInternas[a].removeAt (j);
                    j--;
                }
            }
            ////**********CCAA*******/////
            if(calculos->medio == 39 /*&& Vuelo >= 100*/)
            {
                if(alcance)
                {
                    emit progreso(a,QString::number(a)+"   Guardando datos. Medio 39 Alcance");
                    if(comprobarECC(tiempo,Vuelo,disVDirecta[a].at (0)))//comprobando la visibilidad directa minima para el tiro
                    {
                        estDistanMinMax();

                        double disLanza = 0.0;
                        double dzdr = 0.0;
                        QList<double> disZDR;

                        if(listdisaux.at (0) > DRCmin)//comprobar si la dvd es mayor q la destruccion min
                        {
                            if(listdisaux.at (0) > DRCmax)//comprobar si la dvd es mayor q la destruccion max
                            {
                                dzdr =  DRCmax;
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            else
                            {
                                dzdr = listdisaux.at (0);
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            disZDR.append (dzdr);
                            if(probabilidad  == 0 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                            qDebug() <<disLanza<<"*";
                        }
                        else
                        {
                            //
                            disLanza = 0;
                            dzdr = 0;
                            disZDR.append (dzdr);
                            if(probabilidad  == 0 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                        }

                        if(listdisaux.at (1) > DRCmin)//comprobar si la dvd es mayor q la destruccion min
                        {
                            if(listdisaux.at (1) > DRCmax)//comprobar si la dvd es mayor q la destruccion max
                            {
                                dzdr =  DRCmax;
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            else
                            {
                                dzdr = listdisaux.at (1);
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            disZDR.append (dzdr);
                            if(probabilidad  == 1 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                            qDebug() <<disLanza<<"*";
                        }
                        else
                        {
                            //
                            disLanza = 0;
                            dzdr = 0;
                            disZDR.append (dzdr);
                            if(probabilidad  == 1 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                        }

                        if(listdisaux.at (2) > DRCmin)//comprobar si la dvd es mayor q la destruccion min
                        {
                            if(listdisaux.at (2) > DRCmax)//comprobar si la dvd es mayor q la destruccion max
                            {
                                dzdr =  DRCmax;
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            else
                            {
                                dzdr = listdisaux.at (2);
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            disZDR.append (dzdr);
                            if(probabilidad  == 2 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                            qDebug() <<disLanza<<"*";
                        }
                        else
                        {
                            //
                            disLanza = 0;
                            dzdr = 0;
                            disZDR.append (dzdr);
                            if(probabilidad  == 2 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                        }

                        if(listdisaux.at (3) > DRCmin)//comprobar si la dvd es mayor q la destruccion min
                        {
                            if(listdisaux.at (3) > DRCmax)//comprobar si la dvd es mayor q la destruccion max
                            {
                                dzdr =  DRCmax;
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            else
                            {
                                dzdr = listdisaux.at (3);
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            disZDR.append (dzdr);
                            if(probabilidad  == 3 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                            qDebug() <<disLanza<<"*";
                        }
                        else
                        {
                            //
                            disLanza = 0;
                            dzdr = 0;
                            disZDR.append (dzdr);
                            if(probabilidad  == 3 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                        }

                        if(listdisaux.at (4) > DRCmin)//comprobar si la dvd es mayor q la destruccion min
                        {
                            if(listdisaux.at (4) > DRCmax)//comprobar si la dvd es mayor q la destruccion max
                            {
                                dzdr =  DRCmax;
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            else
                            {
                                dzdr = listdisaux.at (4);
                                comprobarDlanzaSegmentosNV(&dzdr,a);//dislanza fuera de segmentos
                                disLanza = DLCmax - (DRCmax - dzdr);
                                comprobarDlanzaSegmentosNV(&disLanza,a);//dislanza fuera de segmentos
                                if(disLanza >= DLCmin)
                                {
                                    //                                    //calcular distancia de destruccion
                                    velocidadCohete = velocidadCohetes (Vuelo,disLanza);
                                    dzdr = disLanza * velocidadCohete / (0.96 * velocidadCohete - velocidad);
                                    double aux = dzdr;
                                    comprobarDlanzaSegmentosNV(&aux,a);//disDestr fuera de segmentos

                                    if(aux <= disLanza)
                                    {
                                        disLanza = disLanza - (dzdr - aux);
                                        comprobarDlanzaSegmentosNV(&disLanza,a);
                                        if(disLanza <= DLCmin)
                                        {
                                            disLanza = 0;
                                            dzdr = 0;
                                        }
                                    }
                                    dzdr = aux ;

                                    if(dzdr <= DRCmin )
                                    {
                                        disLanza = 0;
                                        dzdr = 0;
                                    }
                                }
                                else
                                {
                                    //
                                    disLanza = 0;
                                    dzdr = 0;
                                }
                            }
                            disZDR.append (dzdr);
                            if(probabilidad  == 4 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                            qDebug() <<disLanza<<"*";
                        }
                        else
                        {
                            //
                            disLanza = 0;
                            dzdr = 0;
                            disZDR.append (dzdr);
                            if(probabilidad  == 4 )
                                disVDirecta[a].append (disLanza);
                            out <<disLanza<<"*";
                        }

                        disVDirecta[a].append(disZDR.at (probabilidad));

                        QGeoCoordinate geo;
                        LatOR = calculos->datoterrenoMiPos.LatOR;
                        LonOR = calculos->datoterrenoMiPos.LonOR;

                        geoPos.setLatitude (calculos->datoterrenoMiPos.LatOR);
                        geoPos.setLongitude (calculos->datoterrenoMiPos.LonOR);

                        geo =  geoPos.atDistanceAndAzimuth (disZDR[0],a,0.0);

                        LatOR = geo.latitude ();
                        LonOR = geo.longitude ();

                        out<< disZDR[0]<<"*";
                        out<< disZDR[1]<<"*";
                        out<< disZDR[2]<<"*";
                        out<< disZDR[3]<<"*";
                        out<< disZDR[4]<<"*";
                        out <<-LonOR<<','<<LatOR<<"*";

                    }
                    else
                    {
                        disVDirecta[a].append (0);
                        disVDirecta[a].append (0);
                        out <<0<<"*";//DLanz
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<0<<"*";//disZDR
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<-LonOR<<','<<LatOR<<"*";
                    }
                }
                else
                {
                    if(comprobarECC(tiempo,Vuelo,disVDirecta[a].at (0)))//Dlanz
                    {
                        double disLanza;
                        double dzdr;
                        QList<double> disZDR;
                        disLanza = listdisaux.at (0) - tiempo * velocidad;//0.5
                        calcularDlDzr(&disLanza, &dzdr,  a);

                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <100)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (1) - tiempo * velocidad;//0.6

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <100)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (2) - tiempo * velocidad; //0.7

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <100)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (3) - tiempo * velocidad;//0.8

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <100)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (4) - tiempo * velocidad;//0.9

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <100)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        if(probabilidad > 0)
                        {
                            disLanza = listdisaux.at (probabilidad) - tiempo * velocidad;
                            if(Vuelo <100)
                                disLanza = 0;
                            calcularDlDzr(&disLanza, &dzdr,  a);
                            if(dzdr == 0)
                                disLanza = 0;


                            disVDirecta[a].append (disLanza);
                        }
                        else
                        {
                            disLanza = listdisaux.at (0) - tiempo * velocidad;
                            if(Vuelo <100)
                                disLanza = 0;
                            calcularDlDzr(&disLanza, &dzdr,  a);
                            if(dzdr == 0)
                                disLanza = 0;
                            disVDirecta[a].append (disLanza);
                        }
                        disVDirecta[a].append(disZDR.at (probabilidad));

                        QGeoCoordinate geo;
                        LatOR = calculos->datoterrenoMiPos.LatOR;
                        LonOR = calculos->datoterrenoMiPos.LonOR;

                        geoPos.setLatitude (calculos->datoterrenoMiPos.LatOR);
                        geoPos.setLongitude (calculos->datoterrenoMiPos.LonOR);

                        geo =  geoPos.atDistanceAndAzimuth (disZDR[0],a,0.0);

                        LatOR = geo.latitude ();
                        LonOR = geo.longitude ();

                        if(Vuelo <100)
                        {
                            disZDR[0] = 0;
                            disZDR[1] = 0;
                            disZDR[2] = 0;
                            disZDR[3] = 0;
                            disZDR[4] = 0;
                        }

                        out<< disZDR[0]<<"*";
                        out<< disZDR[1]<<"*";
                        out<< disZDR[2]<<"*";
                        out<< disZDR[3]<<"*";
                        out<< disZDR[4]<<"*";
                        out <<-LonOR<<','<<LatOR<<"*";

                    }
                    else
                    {
                        disVDirecta[a].append (0);
                        disVDirecta[a].append (0);
                        out <<0<<"*";//DLanz
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<0<<"*";//disZDR
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<-LonOR<<','<<LatOR<<"*";
                    }
                }
            }
            if(calculos->medio == 38 )
            {
                if(alcance )
                {
                    emit progreso(a,QString::number(a)+"   Guardando datos. Medio 38 Alcance");
                    if(comprobarECC(tiempo,Vuelo,disVDirecta[a].at (0)))//Dlanz
                    {
                        estDistanMinMax();

                        double disLanza;
                        double dzdr = 0.0;
                        QList<double> disZDR;

                        if(listdisaux.at (0) > DRCmin)//0.5
                        {
                            if(listdisaux.at (0) > DRCmax)
                            {
                                dzdr = DRCmax;
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }
                            else
                            {
                                switch (velocidad) {
                                case 150:
                                    dzdr = listdisaux.at (0) - 300;
                                    break;
                                case 300:
                                    dzdr = listdisaux.at (0) - 600;
                                    break;

                                default:
                                    break;
                                }
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }

                        }
                        else
                        {
                            dzdr = 0;
                        }
                        disZDR.append (dzdr);

                        if(Vuelo <25)
                            disLanza = 0;
                        if(dzdr == 0)
                            disLanza = 0;
                        else
                        {
                            velocidadCohete = velocidadCohetes (Vuelo, dzdr);
                            disLanza = dzdr * ( velocidadCohete + velocidad) / velocidadCohete;
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNV(&disLanza,a);
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNVzm(&disLanza,a);

                        }

                        if(probabilidad  == 0 )
                            disVDirecta[a].append (disLanza);
                        out <<disLanza<<"*";

                        //0.6
                        if(listdisaux.at (1) > DRCmin)//0.5
                        {
                            if(listdisaux.at (1) > DRCmax)
                            {
                                dzdr = DRCmax;
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }
                            else
                            {
                                switch (velocidad) {
                                case 150:
                                    dzdr = listdisaux.at (1) - 300;
                                    break;
                                case 300:
                                    dzdr = listdisaux.at (1) - 600;
                                    break;

                                default:
                                    break;
                                }
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }

                        }
                        else
                        {
                            dzdr = 0;
                        }
                        disZDR.append (dzdr);
                        if(Vuelo <25)
                            disLanza = 0;
                        if(dzdr == 0)
                            disLanza = 0;
                        else
                        {
                            velocidadCohete = velocidadCohetes (Vuelo, dzdr);
                            disLanza = dzdr * ( velocidadCohete + velocidad) - velocidadCohete;
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNV(&disLanza,a);
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNVzm(&disLanza,a);
                        }
                        if(probabilidad  == 1 )
                            disVDirecta[a].append (disLanza);
                        out <<disLanza<<"*";
                        //0.7
                        if(listdisaux.at (2) > DRCmin)//0.5
                        {
                            if(listdisaux.at (2) > DRCmax)
                            {
                                dzdr = DRCmax;
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }
                            else
                            {
                                switch (velocidad) {
                                case 150:
                                    dzdr = listdisaux.at (2) - 300;
                                    break;
                                case 300:
                                    dzdr = listdisaux.at (2) - 600;
                                    break;

                                default:
                                    break;
                                }
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }

                        }
                        else
                        {
                            dzdr = 0;
                        }
                        disZDR.append (dzdr);
                        if(Vuelo <25)
                            disLanza = 0;
                        if(dzdr == 0)
                            disLanza = 0;
                        else
                        {
                            velocidadCohete = velocidadCohetes (Vuelo, dzdr);
                            disLanza = dzdr * ( velocidadCohete + velocidad) - velocidadCohete;
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNV(&disLanza,a);
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNVzm(&disLanza,a);
                        }
                        if(probabilidad  == 2 )
                            disVDirecta[a].append (disLanza);
                        out <<disLanza<<"*";
                        //                        0.8
                        if(listdisaux.at (3) > DRCmin)//0.5
                        {
                            if(listdisaux.at (3) > DRCmax)
                            {
                                dzdr = DRCmax;
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }
                            else
                            {
                                switch (velocidad) {
                                case 150:
                                    dzdr = listdisaux.at (3) - 300;
                                    break;
                                case 300:
                                    dzdr = listdisaux.at (3) - 600;
                                    break;

                                default:
                                    break;
                                }
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }

                        }
                        else
                        {
                            dzdr = 0;
                        }
                        disZDR.append (dzdr);
                        if(Vuelo <25)
                            disLanza = 0;
                        if(dzdr == 0)
                            disLanza = 0;
                        else
                        {
                            velocidadCohete = velocidadCohetes (Vuelo, dzdr);
                            disLanza = dzdr * ( velocidadCohete + velocidad) - velocidadCohete;
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNV(&disLanza,a);
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNVzm(&disLanza,a);
                        }
                        if(probabilidad  == 3 )
                            disVDirecta[a].append (disLanza);
                        out <<disLanza<<"*";
                        //0.9
                        if(listdisaux.at (4) > DRCmin)//0.5
                        {
                            if(listdisaux.at (4) > DRCmax)
                            {
                                dzdr = DRCmax;
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }
                            else
                            {
                                switch (velocidad) {
                                case 150:
                                    dzdr = listdisaux.at (4) - 300;
                                    break;
                                case 300:
                                    dzdr = listdisaux.at (4) - 600;
                                    break;

                                default:
                                    break;
                                }
                                comprobarDZDR (&dzdr);
                                comprobarDlanzaSegmentosNV(&dzdr,a);
                                comprobarDZDR(&dzdr);
                                comprobarDlanzaSegmentosNVzm(&dzdr,a);
                            }

                        }
                        else
                        {
                            dzdr = 0;
                        }
                        disZDR.append (dzdr);
                        if(Vuelo <25)
                            disLanza = 0;
                        if(dzdr == 0)
                            disLanza = 0;
                        else
                        {
                            velocidadCohete = velocidadCohetes (Vuelo, dzdr);
                            disLanza = dzdr * ( velocidadCohete + velocidad) - velocidadCohete;
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNV(&disLanza,a);
                            comprobarDlanza (&disLanza,disVDirecta[a].at (0));
                            comprobarDlanzaSegmentosNVzm(&disLanza,a);
                        }
                        if(probabilidad  == 4 )
                            disVDirecta[a].append (disLanza);
                        out <<disLanza<<"*";

                        disVDirecta[a].append(disZDR.at (probabilidad));

                        QGeoCoordinate geo;
                        LatOR = calculos->datoterrenoMiPos.LatOR;
                        LonOR = calculos->datoterrenoMiPos.LonOR;

                        geoPos.setLatitude (calculos->datoterrenoMiPos.LatOR);
                        geoPos.setLongitude (calculos->datoterrenoMiPos.LonOR);

                        geo =  geoPos.atDistanceAndAzimuth (disZDR[0],a,0.0);

                        LatOR = geo.latitude ();
                        LonOR = geo.longitude ();

                        if(Vuelo <25)
                        {
                            disZDR[0] = 0;
                            disZDR[1] = 0;
                            disZDR[2] = 0;
                            disZDR[3] = 0;
                            disZDR[4] = 0;
                        }
                        out<< disZDR[0]<<"*";
                        out<< disZDR[1]<<"*";
                        out<< disZDR[2]<<"*";
                        out<< disZDR[3]<<"*";
                        out<< disZDR[4]<<"*";
                        out <<-LonOR<<','<<LatOR<<"*";
                    }
                    else
                    {
                        disVDirecta[a].append (0);
                        disVDirecta[a].append (0);
                        out <<0<<"*";//DLanz
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<0<<"*";//disZDR
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<-LonOR<<','<<LatOR<<"*";
                    }
                }
                else
                {
                    if(comprobarECC(tiempo,Vuelo,disVDirecta[a].at (0)))//Dlanz
                    {
                        double disLanza;
                        double dzdr;
                        QList<double> disZDR;
                        disLanza = listdisaux.at (0) - tiempo * velocidad;//0.5
                        calcularDlDzr(&disLanza, &dzdr,  a);

                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <25)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (1) - tiempo * velocidad;//0.6

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <25)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (2) - tiempo * velocidad; //0.7

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <25)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (3) - tiempo * velocidad;//0.8

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <25)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        disLanza = listdisaux.at (4) - tiempo * velocidad;//0.9

                        calcularDlDzr(&disLanza, &dzdr,  a);
                        disZDR.append (dzdr);
                        if(dzdr == 0)
                            disLanza = 0;
                        if(Vuelo <25)
                            disLanza = 0;
                        out <<disLanza<<"*";

                        if(probabilidad > 0)
                        {
                            disLanza = listdisaux.at (probabilidad) - tiempo * velocidad;
                            if(dzdr == 0)
                                disLanza = 0;
                            calcularDlDzr(&disLanza, &dzdr,  a);

                            if(Vuelo <25)
                                disLanza = 0;
                            disVDirecta[a].append (disLanza);
                        }
                        else
                        {
                            disLanza = listdisaux.at (0) - tiempo * velocidad;
                            if(Vuelo <25)
                                disLanza = 0;
                            calcularDlDzr(&disLanza, &dzdr,  a);
                            if(dzdr == 0)
                                disLanza = 0;

                            disVDirecta[a].append (disLanza);
                        }
                        disVDirecta[a].append(disZDR.at (probabilidad));

                        QGeoCoordinate geo;
                        LatOR = calculos->datoterrenoMiPos.LatOR;
                        LonOR = calculos->datoterrenoMiPos.LonOR;

                        geoPos.setLatitude (calculos->datoterrenoMiPos.LatOR);
                        geoPos.setLongitude (calculos->datoterrenoMiPos.LonOR);

                        geo =  geoPos.atDistanceAndAzimuth (disZDR[0],a,0.0);

                        LatOR = geo.latitude ();
                        LonOR = geo.longitude ();
                        if(Vuelo <25)
                        {
                            disZDR[0] = 0;
                            disZDR[1] = 0;
                            disZDR[2] = 0;
                            disZDR[3] = 0;
                            disZDR[4] = 0;
                        }
                        out<< disZDR[0]<<"*";
                        out<< disZDR[1]<<"*";
                        out<< disZDR[2]<<"*";
                        out<< disZDR[3]<<"*";
                        out<< disZDR[4]<<"*";
                        out <<-LonOR<<','<<LatOR<<"*";
                    }
                    else
                    {
                        disVDirecta[a].append (0);
                        disVDirecta[a].append (0);
                        out <<0<<"*";//DLanz
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<0<<"*";//disZDR
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";
                        out <<0<<"*";

                        out <<-LonOR<<','<<LatOR<<"*";
                    }
                }
            }

            if((calculos->medio == 37 ||
                calculos->medio == 36 ||
                calculos->medio == 35 ||
                calculos->medio == 34 ||
                calculos->medio == 33 ||
                calculos->medio == 32 ||
                calculos->medio == 31 ||
                calculos->medio == 21 ||
                calculos->medio == 22 ||
                calculos->medio == 23 ||
                calculos->medio == 24 ||
                calculos->medio == 25 ||
                calculos->medio == 27 ||
                calculos->medio == 26 ||
                calculos->medio == 42 ||
                calculos->medio == 43 ||
                calculos->medio == 44) && calculos->grupo == 1)
            {
                if(comprobarECC(tiempo,Vuelo,disVDirecta[a].at (0)))//Dlanz
                {
                    double disLanza;
                    double dzdr;
                    QList<double> disZDR;
                    disLanza = listdisaux.at (0) - tiempo * velocidad;//0.5
                    calcularDlDzr(&disLanza, &dzdr,  a);

                    disZDR.append (dzdr);
                    if(dzdr == 0)
                        disLanza = 0;
                    if(Vuelo <25)
                        disLanza = 0;
                    out <<disLanza<<"*";

                    disLanza = listdisaux.at (1) - tiempo * velocidad;//0.6

                    calcularDlDzr(&disLanza, &dzdr,  a);
                    disZDR.append (dzdr);
                    if(dzdr == 0)
                        disLanza = 0;
                    if(Vuelo <25)
                        disLanza = 0;
                    out <<disLanza<<"*";

                    disLanza = listdisaux.at (2) - tiempo * velocidad; //0.7

                    calcularDlDzr(&disLanza, &dzdr,  a);
                    disZDR.append (dzdr);
                    if(dzdr == 0)
                        disLanza = 0;
                    if(Vuelo <25)
                        disLanza = 0;
                    out <<disLanza<<"*";

                    disLanza = listdisaux.at (3) - tiempo * velocidad;//0.8

                    calcularDlDzr(&disLanza, &dzdr,  a);
                    disZDR.append (dzdr);
                    if(dzdr == 0)
                        disLanza = 0;
                    if(Vuelo <25)
                        disLanza = 0;
                    out <<disLanza<<"*";

                    disLanza = listdisaux.at (4) - tiempo * velocidad;//0.9

                    calcularDlDzr(&disLanza, &dzdr,  a);
                    disZDR.append (dzdr);
                    if(dzdr == 0)
                        disLanza = 0;
                    if(Vuelo <25)
                        disLanza = 0;
                    out <<disLanza<<"*";

                    if(probabilidad > 0)
                    {
                        disLanza = listdisaux.at (probabilidad) - tiempo * velocidad;
                        if(Vuelo <25)
                            disLanza = 0;
                        calcularDlDzr(&disLanza, &dzdr,  a);
                        if(dzdr == 0)
                            disLanza = 0;

                        disVDirecta[a].append (disLanza);
                    }
                    else
                    {
                        disLanza = listdisaux.at (0) - tiempo * velocidad;

                        if(Vuelo <25)
                            disLanza = 0;
                        calcularDlDzr(&disLanza, &dzdr,  a);
                        if(dzdr == 0)
                            disLanza = 0;
                        disVDirecta[a].append (disLanza);
                    }
                    disVDirecta[a].append(disZDR.at (probabilidad));

                    QGeoCoordinate geo;
                    LatOR = calculos->datoterrenoMiPos.LatOR;
                    LonOR = calculos->datoterrenoMiPos.LonOR;

                    geoPos.setLatitude (calculos->datoterrenoMiPos.LatOR);
                    geoPos.setLongitude (calculos->datoterrenoMiPos.LonOR);

                    geo =  geoPos.atDistanceAndAzimuth (disZDR[0],a,0.0);

                    LatOR = geo.latitude ();
                    LonOR = geo.longitude ();

                    if(Vuelo <25)
                    {
                        disZDR[0] = 0;
                        disZDR[1] = 0;
                        disZDR[2] = 0;
                        disZDR[3] = 0;
                        disZDR[4] = 0;
                    }
                    out<< disZDR[0]<<"*";
                    out<< disZDR[1]<<"*";
                    out<< disZDR[2]<<"*";
                    out<< disZDR[3]<<"*";
                    out<< disZDR[4]<<"*";
                    out <<-LonOR<<','<<LatOR<<"*";
                }
                else
                {
                    disVDirecta[a].append (0);
                    disVDirecta[a].append (0);
                    out <<0<<"*";//DLanz
                    out <<0<<"*";
                    out <<0<<"*";
                    out <<0<<"*";
                    out <<0<<"*";

                    out <<0<<"*";//disZDR
                    out <<0<<"*";
                    out <<0<<"*";
                    out <<0<<"*";
                    out <<0<<"*";

                    out <<-LonOR<<','<<LatOR<<"*";
                }
            }
        }////////////////
        out << endl;

        qDebug() << endl;
    }
    f.close();//cierro el fichero
    emit progreso(0,"cierro fichero");
}

bool distanThread::leer()
{
    QString auxF = fichero;
    if(alcance)
    {
        auxF.insert (6,"a");
    }
    qDebug()<<"fichero leer"<<fichero;
    bool entra = false;
    bool listo = false;
    QDir temp = QDir(QDir::currentPath() + "/Zonas");//direccion de los ficheros de coordenadas geograficas
    QFileInfoList Fich = temp.entryInfoList();

    foreach (const QFileInfo info, Fich)
    {
        QString nombfich = info.fileName();
        if (auxF == nombfich)
        {
            entra = true;
            break;
        }
    }
    if(entra)
    {
        QStringList todo;
        QFile file(QDir::currentPath() + "/Zonas/"+auxF);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            todo.append(line);
        }
        if( todo.length() == 360)
        {
            listo = true;
            for (int i = 0;i < 360; ++i)
            {
                emit progreso(i,"Calculando datos para "+ QString::number(Vuelo)+"m. Esta operación puede tardar unos minutos");
                QStringList grande=todo.at(i).split("*");
                QStringList visibilidad;
                double visib = 0;
                double disLanzamiento;
                double disZDR;
                QString dislan;
                //              probabilidad = 4;
                if(probabilidad == 0)
                {
                    if(((calculos->medio == 39 && Vuelo >= 100) || ((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio == 25 || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44) && Vuelo >= 25)) && calculos->grupo == 1)
                    {
                        visibilidad = grande.at (8).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (13).split(",");
                        disZDR = visibilidad.at(0).toDouble();
                    }
                    visibilidad = grande.at (1).split(",");//0.5
                    disVDirecta[i].append(visibilidad.at(0).toDouble());

                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio == 25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else if(probabilidad == 1)
                {
                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio ==25) && Vuelo >= 25)|| calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        visibilidad = grande.at (9).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (14).split(",");
                        disZDR = visibilidad.at(0).toDouble();
                    }
                    visibilidad = grande.at(4).split(",");//0.6
                    disVDirecta[i].append(visibilidad.at(0).toDouble());

                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio == 25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else if(probabilidad == 2)
                {
                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio==25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        visibilidad = grande.at (10).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (15).split(",");
                        disZDR = visibilidad.at(0).toDouble();
                    }
                    visibilidad = grande.at(5).split(",");//0.7
                    disVDirecta[i].append(visibilidad.at(0).toDouble());

                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio ==25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else if(probabilidad == 3)
                {
                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio ==25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        visibilidad = grande.at (11).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (16).split(",");
                        disZDR = visibilidad.at(0).toDouble();
                    }
                    visibilidad = grande.at(6).split(",");//0.8
                    disVDirecta[i].append(visibilidad.at(0).toDouble());

                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio ==25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else
                {
                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio ==25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        visibilidad = grande.at (12).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (17).split(",");
                        disZDR = visibilidad.at(0).toDouble();
                    }
                    visibilidad = grande.at(7).split(",");//0.9
                    disVDirecta[i].append(visibilidad.at(0).toDouble());

                    if(((calculos->medio == 39 && Vuelo >= 100) || (((calculos->medio == 38  ||  calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 || calculos->medio == 24 || calculos->medio == 25) && Vuelo >= 25) || calculos->medio==26 || calculos->medio==27 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)) && calculos->grupo == 1)
                    {
                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                visib = visibilidad.at(0).toDouble();
                visibilidad = grande.at(1).split(",");

                for (int j = 1; j < visibilidad.length(); ++j)
                {
                    if(visibilidad.at(j).toDouble() < visib)
                        disInternas[i].append(visibilidad.at(j).toDouble());
                }
                if(!fichero.contains(".ObjL") && !fichero.contains(".m00") && calculos->medio> 2 )
                {
                    QStringList muis1 =grande.at(2).split(",");
                    for (int j = 0; j < muis1.length(); ++j)
                    {
                        ZonaMuertaMuis[i].append(muis1.at(j).toDouble());
                    }
                    QStringList muis2 =grande.at(3).split(",");
                    for (int j = 0; j < muis2.length(); ++j)
                    {
                        ZonaMuertaMuis2[i].append(muis2.at(j).toDouble());
                    }
                }
            }
        }
        else
        {
            listo = false;
            file.remove();
        }
    }
    return listo;
}

//bool distanThread::leerDB()
//{
//    QStringList todo;
//    QString auxF = fichero;
//    auxF = auxF.replace(".","_");
//    m_basedatos->abrirBD();
//    todo = m_basedatos->tomar(auxF,listaCamposPOV);
//    qDebug()<<todo;
//}

void distanThread::run()
{
    detener = true;
    if(((calculos->medio >=21 && calculos->medio <=39) || (calculos->medio >=42 && calculos->medio <=44)) && calculos->grupo == 1)
        leerVelocidad ();

    calcDistancia();
}

void distanThread::estDistanMinMax()
{
    if(calculos->medio == 39)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 1000)
            {
                DRCmin = 7618;
                DRCmax = 23684;
                DLCmin = 6000;
                DLCmax = 18000;
                return;
            }
            if(Vuelo <= 2000)
            {
                DRCmin = 7618;
                DRCmax = 23523;
                DLCmin = 6000;
                DLCmax = 18000;
                return;
            }
            if(Vuelo == 4000)
            {
                DRCmin = 7618;
                DRCmax = 27268;
                DLCmin = 6000;
                DLCmax = 21000;
                return;
            }
            if(Vuelo == 6000)
            {
                DRCmin = 7678;
                DRCmax = 30038;
                DLCmin = 6000;
                DLCmax = 23000;
                return;
            }
            if(Vuelo == 10000)
            {
                DRCmin = 9148;
                DRCmax = 34069;
                DLCmin = 7000;
                DLCmax = 26000;
                return;
            }
            if(Vuelo == 17000)
            {
                DRCmin = 11762;
                DRCmax = 39205;
                DLCmin = 9000;
                DLCmax = 30000;
                return;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 1000)
            {
                DRCmin = 9753;
                DRCmax = 24127;
                DLCmin = 6000;
                DLCmax = 14000;
                return;
            }
            if(Vuelo <= 2000)
            {
                DRCmin = 9753;
                DRCmax = 25641;
                DLCmin = 6000;
                DLCmax = 15000;
                return;
            }
            if(Vuelo == 4000)
            {
                DRCmin = 9753;
                DRCmax = 26829;
                DLCmin = 6000;
                DLCmax = 16000;
                return;
            }
            if(Vuelo == 6000)
            {
                DRCmin = 9953;
                DRCmax = 30970;
                DLCmin = 6000;
                DLCmax = 18000;
                return;
            }
            if(Vuelo == 10000)
            {
                DRCmin = 12272;
                DRCmax = 34641;
                DLCmin = 7000;
                DLCmax = 20000;
                return;
            }
            if(Vuelo == 17000)
            {
                DRCmin = 15779;
                DRCmax = 40096;
                DLCmin = 9000;
                DLCmax = 24000;
                return;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 1000)
            {
                DRCmin = 12572;
                DRCmax = 24564;
                DLCmin = 6000;
                DLCmax = 11000;
                return;
            }
            if(Vuelo <= 2000)
            {
                DRCmin = 12572;
                DRCmax = 26797;
                DLCmin = 6000;
                DLCmax = 12000;
                return;
            }
            if(Vuelo == 4000)
            {
                DRCmin = 12572;
                DRCmax = 28261;
                DLCmin = 6000;
                DLCmax = 13000;
                return;
            }
            if(Vuelo == 6000)
            {
                DRCmin = 13043;
                DRCmax = 31804;
                DLCmin = 6000;
                DLCmax = 14000;
                return;
            }
            if(Vuelo == 10000)
            {
                DRCmin = 16886;
                DRCmax = 34483;
                DLCmin = 7000;
                DLCmax = 15000;
                return;
            }
            if(Vuelo == 17000)
            {
                DRCmin = 21710;
                DRCmax = 40783;
                DLCmin = 9000;
                DLCmax = 19000;
                return;
            }
        }
    }
    if(calculos->medio == 38)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 1000)
            {
                DRCmin = 6000;
                DRCmax = 16000;
                DLCmin = 4400;
                DLCmax = 12000;
                return;
            }
            if(Vuelo <= 2000)
            {
                DRCmin = 6000;
                DRCmax = 18000;
                DLCmin = 4400;
                DLCmax = 14000;
                return;
            }
            if(Vuelo == 4000)
            {
                DRCmin = 6000;
                DRCmax = 19000;
                DLCmin = 4400;
                DLCmax = 15500;
                return;
            }
            if(Vuelo <= 10000)
            {
                DRCmin = 6000;
                DRCmax = 22000;
                DLCmin = 4400;
                DLCmax = 18000;
                return;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 1000)
            {
                DRCmin = 6000;
                DRCmax = 17000;
                DLCmin = 3000;
                DLCmax = 11000;
                return;
            }
            if(Vuelo <= 2000)
            {
                DRCmin = 6000;
                DRCmax = 18000;
                DLCmin = 3000;
                DLCmax = 12000;
                return;
            }
            if(Vuelo == 4000)
            {
                DRCmin = 6000;
                DRCmax = 18000;
                DLCmin = 3000;
                DLCmax = 13000;
                return;
            }
            if(Vuelo <= 10000)
            {
                DRCmin = 6000;
                DRCmax = 22000;
                DLCmin = 3000;
                DLCmax = 17000;
                return;
            }
        }
    }
}

void distanThread::determDistanCiclo(/*int g, int a, int m*/)
{
    switch (calculos->grupo)
    {
    case 1://grupo
        switch (calculos->medio)
        {
        case 0://pop
            distancia = 12060;
            switch (Vuelo) {
            case 1000:
                distanciamax = 11970;
                distanciamaxOficial = 11970;
                break;
            case 2000:
                distanciamax = 11880;
                distanciamaxOficial = 11880;
                break;
            case 4000:
                distanciamax = 11340;
                distanciamaxOficial = 11340;
                break;
            case 6000:
                distanciamax = 10440;
                distanciamaxOficial = 10440;
                break;
            case 10000:
                distanciamax = 6660;
                distanciamaxOficial = 6660;
                break;
            default:
                distanciamax = 12060;
                distanciamaxOficial = 12060;
                break;
            }
            break;
        case 11://pov
            distancia = 20070;
            switch (Vuelo) {
            case 1000:
                distanciamax = distanciamaxOficial = 19975;break;
            case 2000: distanciamax = distanciamaxOficial = 19900;break;
            case 4000: distanciamax = distanciamaxOficial = 19596;break;
            case 6000: distanciamax = distanciamaxOficial = 19079;break;
            case 10000: distanciamax = distanciamaxOficial = 17320;break;
            case 17000: distanciamax = distanciamaxOficial = 10535;break;
            default:
                distanciamax = distanciamaxOficial = 20000;
                break;
            }
            break;
        case 12:
            distancia = 30060;
            switch (Vuelo) {
            case 1000: distanciamax = distanciamaxOficial = 29983;break;
            case 2000: distanciamax = distanciamaxOficial = 29933;break;
            case 4000: distanciamax = distanciamaxOficial = 29732;break;
            case 6000: distanciamax = distanciamaxOficial = 29394;break;
            case 10000: distanciamax = distanciamaxOficial = 28284;break;
            case 17000: distanciamax = distanciamaxOficial = 24718;break;
            default:
                distanciamax = distanciamaxOficial = 30000;
                break;
            }
            break;

        case 13:
            distancia = 40050;
            switch (Vuelo) {
            case 1000: distanciamax = distanciamaxOficial  = 39987;break;
            case 2000: distanciamax = distanciamaxOficial  = 39950;break;
            case 4000: distanciamax = distanciamaxOficial  = 39799;break;
            case 6000: distanciamax = distanciamaxOficial  = 39547;break;
            case 10000: distanciamax = distanciamaxOficial  = 38730;break;
            case 17000: distanciamax = distanciamaxOficial  = 36208;break;
            case 30000: distanciamax = distanciamaxOficial  = 26457;break;
            default:
                distanciamax  = distanciamaxOficial  = 40000;
                break;
            }
            break;
            /****////////////////////////////////////////////////////////////////
        case 21:
            distanciamax  = distanciamaxOficial  = 8010;
            distancia = 8010;
            zonamuerta = 5;
            break;
        case 22:
            distanciamax = distanciamaxOficial  = 8010;
            distancia = 8010;
            zonamuerta = 5;
            break;
        case 23:
            distanciamax = distanciamaxOficial  = 8010;
            distancia = 8010;
            zonamuerta = 5;
            break;
        case 24:
            distanciamax  = distanciamaxOficial = 8010;
            distancia = 8010;
            zonamuerta = 5;
            break;
        case 25:
            distanciamax  = distanciamaxOficial  = 8010;
            distancia = 8010;
            zonamuerta = 5;
            break;
        case 26:
            distanciamax = distanciamaxOficial  = 8010;
            distancia = 8010;
            zonamuerta = 5;
            break;
        case 27:
            distanciamax = distanciamaxOficial = 8010;
            distancia = 8010;
            zonamuerta = 5;
            break;
        case 31:
            distancia = 8010;
            distanciamax  = distanciamaxOficial = 8010;
            zonamuerta = 500;
            break;
        case 32:
            distancia = 8010;
            distanciamax  = distanciamaxOficial = 8010;
            zonamuerta = 500;
            break;
        case 33:
            distancia = 8010;
            distanciamax = distanciamaxOficial  = 8010;
            zonamuerta = 500;
            break;
        case 34:
            distancia = 8010;
            distanciamax = distanciamaxOficial  = 8010;
            switch (Vuelo) {
            case 500: zonamuerta = 10;break;
            case 1000: zonamuerta = 20;break;
            case 2000: zonamuerta = 40; break;
            default:zonamuerta = 10;break;
            }
            break;
        case 35:
            distancia = 8010;
            distanciamax = distanciamaxOficial  = 8010;
            zonamuerta = 800;
            break;
            /****////////////////////////////////////////////////////////////////
        case 36:
            distancia = 45000;
            distanciamax = distanciamaxOficial  = 45000;
            zonamuerta = 1500;
            break;
        case 37:
            distancia = 65070;
            distanciamax  = distanciamaxOficial = 65070;
            zonamuerta = 3500;
            break;
        case 38:
            distancia = 80010;
            distanciamax = distanciamaxOficial  = 80010;
            zonamuerta = 3500;
            break;
        case 39:
            if(ObjLoc)
            {
                distancia = 100000;
                distanciamax = distanciamaxOficial  = 100000;
            }
            else
            {
                distancia = 150030;
                distanciamax = distanciamaxOficial  = 150030;
            }
            zonamuerta = 7000;
            break;
        case 42:
            if(ObjLoc)
            {
                distancia = 100000;
            }
            else
                distancia = 160020;
            distanciamax = distanciamaxOficial  = 60000;
            switch (Vuelo) {
            case 100: zonamuerta = 10;break;
            case 200: zonamuerta = 20;break;
            case 300: zonamuerta = 30;break;
            case 400: zonamuerta = 40;break;
            case 500: zonamuerta = 50;break;
            case 1000: zonamuerta = 100;break;
            case 2000: zonamuerta = 200;break;
            case 6000: zonamuerta = 680;break;
            case 10000:zonamuerta = 1300;break;
            default:zonamuerta = 10;break;
            }
            break;
        case 43:
            if(ObjLoc)
            {
                distancia = 100000;
            }
            else
                distancia = 160020;
            distanciamax  = distanciamaxOficial = 25000;
            switch (Vuelo) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100; break;
            case 6000: zonamuerta = 340; break;
            case 10000:zonamuerta = 1300;break;
            default:zonamuerta = 5;break;
            }
            break;
        case 44:
            if(ObjLoc)
            {
                distancia = 100000;
            }
            else
                distancia = 160020;
            distanciamax = distanciamaxOficial  = 18000;
            switch (Vuelo) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100; break;
            default:zonamuerta = 5;break;
            }
            break;
        case 41:
            if(ObjLoc)
            {
                distancia = 100000;
                distanciamax = distanciamaxOficial  = 100000;
            }
            else
            {
                distancia = 185040;
                distanciamax  = distanciamaxOficial = 180000;
            }
            break;
        case 48:
            if(ObjLoc)
            {
                distancia = 100000;
                distanciamax = distanciamaxOficial  = 100000;
            }
            else
            {
                distancia = 185040;
                distanciamax = distanciamaxOficial  = 180000;
            }
            break;
        case 49:
            if(ObjLoc)
            {
                distancia = 100000;
                distanciamax  = distanciamaxOficial = 100000;
            }
            else
            {
                distancia = 185040;
                distanciamax = distanciamaxOficial  = 180000;
            }
            break;
        case 40:
            if(ObjLoc)
            {
                distancia = 100000;
                distanciamax = distanciamaxOficial  = 100000;
            }
            else
            {
                distancia = 320040;
                distanciamax = distanciamaxOficial  = 360000;
            }
            zonamuerta = 2.25*Vuelo;
            break;
        case 50:
            if(ObjLoc)
            {
                distancia = 100000;
                distanciamax = distanciamaxOficial  = 100000;
            }
            else
            {
                distancia = 320040;
                distanciamax  = distanciamaxOficial = 700000;
            }
            zonamuerta = 5*Vuelo;
            break;

        case 45:
            zonamuerta = 2.25*Vuelo;
            if(calculos->tipoBiesta == "Rx")
            {
                if(ObjLoc)
                {
                    distancia = 100000;
                    distanciamax = distanciamaxOficial  = 100000;
                }
                else
                {
                    distancia = 320040;
                    distanciamax  = distanciamaxOficial = 320000;
                }
            }
            else
            {
                if(ObjLoc)
                {
                    distancia = 100000;
                    distanciamax  = distanciamaxOficial = 100000;
                }
                else
                {
                    distancia = 125010;
                    distanciamax = distanciamaxOficial  = 120000;
                }
            }
            break;

        case 46:
            zonamuerta = 3500;
            if(calculos->tipoBiesta == "Rx")
            {
                distancia = 45000;
                distanciamax  = distanciamaxOficial = 40000;
            }
            else
            {
                distancia = 35010;
                distanciamax  = distanciamaxOficial = 30000;
            }
            break;

        case 47:
            if(calculos->tipoBiesta == "Rx")
            {
                distancia = 45000;
                distanciamax = distanciamaxOficial  = 40000;
            }
            else
            {
                distancia = 65070;
                distanciamax  = distanciamaxOficial = 60000;
            }
            break;
        }
        break;
    case 2://grupo
        switch (calculos->medio)
        {
        case 21:
            distanciamax  = distanciamaxOficial = 1500;
            distancia = 2070;
            switch (Vuelo) {
            case 100: zonamuerta = 5;distanciamax  = distanciamaxOficial = 1500;break;
            case 200: zonamuerta = 10;distanciamax = distanciamaxOficial  = 1500;break;
            case 300: zonamuerta = 15;distanciamax = distanciamaxOficial  = 1500;break;
            case 400: zonamuerta = 20;distanciamax = distanciamaxOficial  = 1500;break;
            case 500: zonamuerta = 25;distanciamax = distanciamaxOficial  = 1414;break;
            case 1000: zonamuerta = 50;distanciamax  = distanciamaxOficial = 1118;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 22:
            distanciamax  = distanciamaxOficial = 2500;
            distancia = 3060;
            switch (Vuelo) {
            case 100: zonamuerta = 5;distanciamax = distanciamaxOficial  = 2500;break;
            case 200: zonamuerta = 10;distanciamax  = distanciamaxOficial = 2500;break;
            case 300: zonamuerta = 15;distanciamax  = distanciamaxOficial = 2500;break;
            case 400: zonamuerta = 20;distanciamax  = distanciamaxOficial = 2500;break;
            case 500: zonamuerta = 25;distanciamax  = distanciamaxOficial = 2449;break;
            case 1000: zonamuerta = 50;distanciamax  = distanciamaxOficial = 2291;break;
            case 2000: zonamuerta = 100;distanciamax  = distanciamaxOficial = 2000;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 23:
            distanciamax = 2500;
            distancia = 3060;
            switch (Vuelo) {
            case 100: zonamuerta = 5;distanciamax = distanciamaxOficial  = 2500;break;
            case 200: zonamuerta = 10;distanciamax  = distanciamaxOficial = 2500;break;
            case 300: zonamuerta = 15;distanciamax  = distanciamaxOficial = 2500;break;
            case 400: zonamuerta = 20;distanciamax  = distanciamaxOficial = 2500;break;
            case 500: zonamuerta = 25;distanciamax  = distanciamaxOficial = 2449;break;
            case 1000: zonamuerta = 50;distanciamax = distanciamaxOficial  = 2291;break;
            case 2000: zonamuerta = 100;distanciamax  = distanciamaxOficial = 1500;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 24:
            distanciamax = 3000;
            distancia = 3510;
            switch (Vuelo) {
            case 100: zonamuerta = 5;distanciamax  = distanciamaxOficial = 3000;break;
            case 200: zonamuerta = 10;distanciamax = distanciamaxOficial  = 3000;break;
            case 300: zonamuerta = 15;distanciamax = distanciamaxOficial  = 3000;break;
            case 400: zonamuerta = 20;distanciamax = distanciamaxOficial  = 3000;break;
            case 500: zonamuerta = 25;distanciamax = distanciamaxOficial  = 2958;break;
            case 1000: zonamuerta = 50;distanciamax  = distanciamaxOficial = 2828;break;
            case 2000: zonamuerta = 100;distanciamax = distanciamaxOficial  = 2236;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 25:
            distanciamax = 4000;
            distancia = 4500;
            switch (Vuelo) {
            case 100: zonamuerta = 5;distanciamax  = distanciamaxOficial = 4000;break;
            case 200: zonamuerta = 10;distanciamax = distanciamaxOficial  = 4000;break;
            case 300: zonamuerta = 15;distanciamax = distanciamaxOficial  = 4000;break;
            case 400: zonamuerta = 20;distanciamax = distanciamaxOficial  = 4000;break;
            case 500: zonamuerta = 25;distanciamax = distanciamaxOficial  = 3937;break;
            case 1000: zonamuerta = 50;distanciamax  = distanciamaxOficial = 3872;break;
            case 2000: zonamuerta = 100;distanciamax = distanciamaxOficial  = 3464;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 26:
            distanciamax = 6000;
            distancia = 6570;
            switch (Vuelo) {
            case 100: zonamuerta = 5;distanciamax  = distanciamaxOficial = 6000;break;
            case 200: zonamuerta = 10;distanciamax = distanciamaxOficial  = 6000;break;
            case 300: zonamuerta = 15;distanciamax = distanciamaxOficial  = 6000;break;
            case 400: zonamuerta = 20;distanciamax = distanciamaxOficial  = 6000;break;
            case 500: zonamuerta = 25;distanciamax = distanciamaxOficial  = 5979;break;
            case 1000: zonamuerta = 50;distanciamax  = distanciamaxOficial = 5956;break;
            case 2000: zonamuerta = 100;distanciamax = distanciamaxOficial  = 5916;break;
            case 4000: zonamuerta = 340;distanciamax = distanciamaxOficial  = 2600;break;
            default:zonamuerta = 5;break;
            }
            break;
        case 27:
            distancia = 12510;
            distanciamax = 12000;
            switch (Vuelo) {
            case 100: zonamuerta = 10;distanciamax  = distanciamaxOficial = 12000;break;
            case 200: zonamuerta = 20;distanciamax  = distanciamaxOficial = 12000;break;
            case 300: zonamuerta = 30;distanciamax  = distanciamaxOficial = 12000;break;
            case 400: zonamuerta = 40;distanciamax  = distanciamaxOficial = 12000;break;
            case 500: zonamuerta = 50;distanciamax  = distanciamaxOficial = 12000;break;
            case 1000: zonamuerta = 100;distanciamax  = distanciamaxOficial = 12000;break;
            case 2000: zonamuerta = 200;distanciamax  = distanciamaxOficial = 11800;break;
            case 4000: zonamuerta = 340;distanciamax  = distanciamaxOficial = 11000;break;
            case 6000: zonamuerta = 680;distanciamax  = distanciamaxOficial = 10400;break;
            case 10000: zonamuerta = 1300;distanciamax  = distanciamaxOficial = 6600;break;
            default:zonamuerta = 10;break;
            }
            break;
        case 42:
            distancia = 50040;
            distanciamax = 12000;
            switch (Vuelo) {
            case 100: zonamuerta = 10;break;
            case 200: zonamuerta = 20;break;
            case 300: zonamuerta = 30;break;
            case 400: zonamuerta = 40;break;
            case 500: zonamuerta = 50;break;
            case 1000: zonamuerta = 100;break;
            case 2000:
                zonamuerta = 200;
                distanciamax  = distanciamaxOficial = 11800;
                break;
            case 6000:
                zonamuerta = 680;
                distanciamax = distanciamaxOficial  = 10400;
                break;
            case 10000:
                zonamuerta = 1300;
                distanciamax = distanciamaxOficial  = 6600;
                break;
            default:zonamuerta = 10;break;
            }
            break;
        case 43:
            distanciamax  = distanciamaxOficial = 6000;
            distancia = 6570;
            switch (Vuelo) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100; break;
            case 6000:
                zonamuerta = 340;
                distanciamax  = distanciamaxOficial = 2600;
                break;
            case 10000:
                zonamuerta = 1300;
                distanciamax  = distanciamaxOficial = 6600;
                break;
            default:zonamuerta = 5;break;
            }
            break;
        case 44:
            distanciamax  = distanciamaxOficial = 2500;
            distancia = 3060;
            switch (Vuelo) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100; break;
            default:zonamuerta = 5;break;
            }
            break;
        }
        break;
    case 3://grupo
        switch (calculos->medio)
        {
        case 31:
            distancia = 4500;
            distanciamax  = distanciamaxOficial = 4000;
            zonamuerta = 500;
            break;
        case 32:
            distancia = 2520;
            distanciamax  = distanciamaxOficial = 2000;
            zonamuerta = 500;
            break;
        case 33:
            distancia = 3060;
            distanciamax  = distanciamaxOficial = 2500;
            zonamuerta = 500;
            break;
        case 34:
            distancia = 3510;
            distanciamax  = distanciamaxOficial = 3000;
            switch (Vuelo) {
            case 500: zonamuerta = 10;break;
            case 1000: zonamuerta = 20;break;
            case 2000: zonamuerta = 40; break;
            default:zonamuerta = 10;break;
            }
            break;
        case 35:
            distancia = 5580;
            distanciamax = distanciamaxOficial = 5000;
            zonamuerta = 800;
            break;
        case 36:
            distancia = 10530;
            distanciamax  = distanciamaxOficial = 10000;
            zonamuerta = 1500;
            break;
        case 37:
            distancia = 19080;
            zonamuerta = 3500;
            switch (Vuelo) {
            case 100: distanciamax  = distanciamaxOficial = 14000;break;
            case 200: distanciamax  = distanciamaxOficial = 14000;break;
            case 300: distanciamax  = distanciamaxOficial = 14000;break;
            case 400: distanciamax  = distanciamaxOficial = 14000;break;
            case 500: distanciamax  = distanciamaxOficial = 16100;break;
            case 1000: distanciamax = distanciamaxOficial  = 17200;break;
            case 2000: distanciamax = distanciamaxOficial  = 18860; break;
            case 4000: distanciamax = distanciamaxOficial  = 20000; break;
            case 10000: distanciamax  = distanciamaxOficial = 15900; break;
            default:distanciamax  = distanciamaxOficial = 14000;break;
            }
            break;
        case 38:
            distancia = 19440;
            zonamuerta = 3500;
            if(Vuelo <= 500)
                distanciamax = distanciamaxOficial  = 11000;
            else
                distanciamax  = distanciamaxOficial = 17000;break;
        case 39:
            distancia = 43380;
            zonamuerta = 7000;
            if(Vuelo <= 1000)
                distanciamax  = distanciamaxOficial = 24000;
            else
            {
                switch (Vuelo)
                {
                case 2000: distanciamax  = distanciamaxOficial = 26000; break;
                case 4000: distanciamax  = distanciamaxOficial = 38500; break;
                case 6000: distanciamax  = distanciamaxOficial = 41500; break;
                case 10000: distanciamax = distanciamaxOficial  = 48500; break;
                case 17000: distanciamax = distanciamaxOficial  = 52000; break;
                case 30000: distanciamax = distanciamaxOficial  = 47000; break;
                }
                break;
            }
        }
        break;
    }
}

void distanThread::tangente(int ang)
{
    switch (ang)
    {
    case -1: tang1 = 0;          tang2 = -0.034920769; break;
    case 0: tang1 = 0.017455065; tang2 = -0.017455065; break;
    case 1: tang1 = 0.034920769; tang2 = 0; break;
    case 2: tang1 = 0.052407779; tang2 = 0.017455065; break;
    case 3: tang1 = 0.069926812; tang2 = 0.034920769; break;
    case 4: tang1 = 0.087488664; tang2 = 0.052407779; break;
    }

}

bool distanThread::pintarDistancias()//para guldris
{

    QVector<double> x1; QVector<double> y1;
    QString datostabla;
    for (int a = 0; a <= 360; a++)
    {
        if(a < 360)
        {
            if(disInternas[a].length() >= 1)//al menos un punto interno
            {
                for (int i = 0; i < disInternas[a].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(i < disInternas[a].length()-1 )
                        {

                        }
                        else
                        {
                            disVDirecta[a].replace (0,disInternas[a].last ());
                        }
                    }
                }
            }
        }
        else//360
        {
            if(disInternas[0].length() >= 1)
            {
                for (int i = 0; i < disInternas[0].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(i < disInternas[0].length()-1 )
                        {
                        }
                        else
                        {
                            disVDirecta[0].replace (0,disInternas[0].last ());

                        }
                    }
                }
            }
        }
    }
    return true;
}

bool distanThread::comprobarECC(int t, int alturaVuelo,double dvdecc)
{
    if(calculos->medio == 39)
    {
        if(!alcance)
        {
            if(velocidad == 150)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 1000)
                    {DECCM = 10300;return (dvdecc >= 10300);}

                    if(alturaVuelo == 2000)
                    {DECCM = 11300;return (dvdecc >= 11300);}

                    if(alturaVuelo == 4000)
                    {DECCM = 12300;return (dvdecc >= 12300);}

                    if(alturaVuelo == 6000)
                    {DECCM = 14300; return (dvdecc >= 14300);}

                    if(alturaVuelo == 10000)
                    {DECCM = 16300;return (dvdecc >= 16300);}

                    if(alturaVuelo == 17000)
                    {DECCM = 22300;return (dvdecc >= 22300);}

                    if(alturaVuelo == 30000)
                    {DECCM = 29300;return (dvdecc >= 29300);}
                    break;

                case 14:
                    if(alturaVuelo <= 1000)
                    {DECCM = 12100;return (dvdecc >= 12100);}

                    if(alturaVuelo == 2000)
                    {DECCM = 13100;return (dvdecc >= 13100);}

                    if(alturaVuelo == 4000)
                    {DECCM = 14100;return (dvdecc >= 14100);}

                    if(alturaVuelo == 6000)
                    {DECCM = 16100;return (dvdecc >= 16100);}

                    if(alturaVuelo == 10000)
                    {DECCM = 18100;return (dvdecc >= 18100);}

                    if(alturaVuelo == 17000)
                    {DECCM = 24100;return (dvdecc >= 24100);}

                    if(alturaVuelo == 30000)
                    {DECCM = 31100;return (dvdecc >= 31100);}
                    break;

                case 25:
                    if(alturaVuelo <= 1000)
                    {DECCM = 13800;return (dvdecc >= 13800);}
                    if(alturaVuelo <= 2000)
                    {DECCM = 14800;return (dvdecc >= 14800);}
                    if(alturaVuelo == 4000)
                    {DECCM = 15800;return (dvdecc >= 15800);}
                    if(alturaVuelo == 6000)
                    {DECCM = 17800;return (dvdecc >= 17800);}
                    if(alturaVuelo == 10000)
                    {DECCM = 19800;return (dvdecc >= 19800);}
                    if(alturaVuelo == 17000)
                    {DECCM = 25800;return (dvdecc >= 25800);}
                    if(alturaVuelo == 30000)
                    {DECCM = 32800;return (dvdecc >= 32800);}
                    break;
                case 35:
                    if(alturaVuelo <= 1000)
                    {DECCM = 15300;return (dvdecc >= 15300);}
                    if(alturaVuelo == 2000)
                    {DECCM = 16300;return (dvdecc >= 16300);}
                    if(alturaVuelo == 4000)
                    {DECCM = 17300;return (dvdecc >= 17300);}
                    if(alturaVuelo == 6000)
                    {DECCM = 19300;return (dvdecc >= 19300);}
                    if(alturaVuelo == 10000)
                    {DECCM = 21300;return (dvdecc >= 21300);}
                    if(alturaVuelo == 17000)
                    {DECCM = 27300;return (dvdecc >= 27300);}
                    if(alturaVuelo == 30000)
                    {DECCM = 34300;return (dvdecc >= 34300);}
                    break;
                default:
                    break;

                }
            }
            if(velocidad == 300)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 1000)
                    {DECCM = 12600;return (dvdecc >= 12600);}

                    if(alturaVuelo == 2000)
                    {DECCM = 12600;return (dvdecc >= 12600);}

                    if(alturaVuelo == 4000)
                    {DECCM = 13600;return (dvdecc >= 13600);}

                    if(alturaVuelo == 6000)
                    {DECCM = 16600;return (dvdecc >= 16600);}

                    if(alturaVuelo == 10000)
                    {DECCM = 19600;return (dvdecc >= 19600);}

                    if(alturaVuelo == 17000)
                    {DECCM = 26600;return (dvdecc >= 26600);}

                    if(alturaVuelo == 30000)
                    {DECCM = 34600; return (dvdecc >= 34600);}
                    break;

                case 14:
                    if(alturaVuelo <= 1000)
                    {DECCM = 16000;return (dvdecc >= 16000);}

                    if(alturaVuelo == 2000)
                    {DECCM = 16000;return (dvdecc >= 16000);}

                    if(alturaVuelo == 4000)
                    {DECCM = 17000;return (dvdecc >= 17000);}

                    if(alturaVuelo == 6000)
                    {DECCM = 20000;return (dvdecc >= 20000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 23000;return (dvdecc >= 23000);}

                    if(alturaVuelo == 17000)
                    {DECCM = 30000;return (dvdecc >= 30000);}

                    if(alturaVuelo == 30000)
                    {DECCM = 37000;return (dvdecc >= 37000);}
                    break;

                case 25:
                    if(alturaVuelo <= 2000)
                    {DECCM = 19500;return (dvdecc >= 19500);}
                    if(alturaVuelo == 4000)
                    {DECCM = 20500;return (dvdecc >= 20500);}
                    if(alturaVuelo == 6000)
                    {DECCM = 23500; return (dvdecc >= 23500);}
                    if(alturaVuelo == 10000)
                    {DECCM = 26500;return (dvdecc >= 26500);}
                    if(alturaVuelo == 17000)
                    {DECCM = 33500;return (dvdecc >= 33500);}
                    if(alturaVuelo == 30000)
                    {DECCM = 41500;return (dvdecc >= 41500);}
                    break;
                case 35:
                    if(alturaVuelo <= 1000)
                    {DECCM = 22500;return (dvdecc >= 22500);}
                    if(alturaVuelo == 2000)
                    {DECCM = 22500;return (dvdecc >= 22500);}
                    if(alturaVuelo == 4000)
                    {DECCM = 23500;return (dvdecc >= 23500);}
                    if(alturaVuelo == 6000)
                    {DECCM = 26500;return (dvdecc >= 26500);}
                    if(alturaVuelo == 10000)
                    {DECCM = 29500;return (dvdecc >= 29500);}
                    if(alturaVuelo == 17000)
                    {DECCM = 36500;return (dvdecc >= 36500);}
                    if(alturaVuelo == 30000)
                    {DECCM = 44500;return (dvdecc >= 44500);}
                    break;
                default:
                    break;

                }
            }
            if(velocidad == 420)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 1000)
                    {DECCM = 15900;return (dvdecc >= 15900);}

                    if(alturaVuelo == 2000)
                    {DECCM = 16900;return (dvdecc >= 16900);}

                    if(alturaVuelo == 4000)
                    {DECCM = 18900; return (dvdecc >= 18900);}

                    if(alturaVuelo == 6000)
                    {DECCM = 19900;return (dvdecc >= 19900);}

                    if(alturaVuelo == 10000)
                    {DECCM = 19900;return (dvdecc >= 19900);}

                    if(alturaVuelo == 17000)
                    {DECCM = 26900;return (dvdecc >= 26900);}

                    if(alturaVuelo == 30000)
                    {DECCM = 34900;return (dvdecc >= 34900);}
                    break;

                case 14:
                    if(alturaVuelo <= 1000)
                    {DECCM = 20880;return (dvdecc >= 20880);}

                    if(alturaVuelo == 2000)
                    {DECCM = 21880;return (dvdecc >= 21880);}

                    if(alturaVuelo == 4000)
                    {DECCM = 23880;return (dvdecc >= 23880);}

                    if(alturaVuelo == 6000)
                    {DECCM = 24880;return (dvdecc >= 24880);}

                    if(alturaVuelo == 10000)
                    {DECCM = 24880;return (dvdecc >= 24880);}

                    if(alturaVuelo == 17000)
                    {DECCM = 31880;return (dvdecc >= 31880);}

                    if(alturaVuelo == 30000)
                    {DECCM = 39880;return (dvdecc >= 39880);}
                    break;

                case 25:
                    if(alturaVuelo <= 2000)
                    {DECCM = 25500;return (dvdecc >= 25500);}
                    if(alturaVuelo == 4000)
                    {DECCM = 26500;return (dvdecc >= 26500);}
                    if(alturaVuelo == 6000)
                    {DECCM = 29500; return (dvdecc >= 29500);}
                    if(alturaVuelo == 10000)
                    {DECCM = 29500; return (dvdecc >= 29500);}
                    if(alturaVuelo == 17000)
                    {DECCM = 36500; return (dvdecc >= 36500);}
                    if(alturaVuelo == 30000)
                    {DECCM = 44500; return (dvdecc >= 44500);}
                    break;
                case 35:
                    if(alturaVuelo <= 1000)
                    {DECCM = 29700;return (dvdecc >= 29700);}
                    if(alturaVuelo == 2000)
                    {DECCM = 30700;return (dvdecc >= 30700);}
                    if(alturaVuelo == 4000)
                    {DECCM = 32700; return (dvdecc >= 32700);}
                    if(alturaVuelo == 6000)
                    {DECCM = 33700;return (dvdecc >= 33700);}
                    if(alturaVuelo == 10000)
                    {DECCM = 33700;return (dvdecc >= 33700);}
                    if(alturaVuelo == 17000)
                    {DECCM = 40700;return (dvdecc >= 40700);}
                    if(alturaVuelo == 30000)
                    {DECCM = 48700;return (dvdecc >= 48700);}
                    break;
                default:
                    break;
                }
            }
            if(velocidad == 640)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 1000)
                    {DECCM = 18300;return (dvdecc >= 18300);}

                    if(alturaVuelo == 2000)
                    {DECCM = 18300;return (dvdecc >= 18300);}

                    if(alturaVuelo == 4000)
                    {DECCM = 19300;return (dvdecc >= 19300);}

                    if(alturaVuelo == 6000)
                    {DECCM = 22300;return (dvdecc >= 22300);}

                    if(alturaVuelo == 10000)
                    {DECCM = 28300;return (dvdecc >= 28300);}

                    if(alturaVuelo == 17000)
                    {DECCM = 37300;return (dvdecc >= 37300);}

                    if(alturaVuelo == 30000)
                    {DECCM = 69300;return (dvdecc >= 69300);}
                    break;

                case 14:
                    if(alturaVuelo <= 1000)
                    {DECCM = 26000;return (dvdecc >= 26000);}

                    if(alturaVuelo == 2000)
                    {DECCM = 26000;return (dvdecc >= 26000);}

                    if(alturaVuelo == 4000)
                    {DECCM = 27000; return (dvdecc >= 27000);}

                    if(alturaVuelo == 6000)
                    {DECCM = 30000;return (dvdecc >= 30000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 36000;return (dvdecc >= 36000);}

                    if(alturaVuelo == 17000)
                    {DECCM = 45000;return (dvdecc >= 45000);}

                    if(alturaVuelo == 30000)
                    {DECCM = 77000;return (dvdecc >= 77000);}
                    break;

                case 25:
                    if(alturaVuelo <= 2000)
                    {DECCM = 33000; return (dvdecc >= 33000);}
                    if(alturaVuelo == 4000)
                    {DECCM = 34000;return (dvdecc >= 34000);}
                    if(alturaVuelo == 6000)
                    {DECCM = 37000;return (dvdecc >= 37000);}
                    if(alturaVuelo == 10000)
                    {DECCM = 43000;return (dvdecc >= 43000);}
                    if(alturaVuelo == 17000)
                    {DECCM = 52000;return (dvdecc >= 52000);}
                    if(alturaVuelo == 30000)
                    {DECCM = 82000;return (dvdecc >= 82000);}
                    break;
                case 35:
                    if(alturaVuelo <= 1000)
                    {DECCM = 39400;return (dvdecc >= 39400);}
                    if(alturaVuelo == 2000)
                    {DECCM = 39400;return (dvdecc >= 39400);}
                    if(alturaVuelo == 4000)
                    {DECCM = 40400;return (dvdecc >= 40400);}
                    if(alturaVuelo == 6000)
                    {DECCM = 43400;return (dvdecc >= 43400);}
                    if(alturaVuelo == 10000)
                    {DECCM = 49400;return (dvdecc >= 49400);}
                    if(alturaVuelo == 17000)
                    {DECCM = 58400;return (dvdecc >= 58400);}
                    if(alturaVuelo == 30000)
                    {DECCM = 90400;return (dvdecc >= 90400);}
                    break;
                default:
                    break;

                }
            }
            if(velocidad == 950)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 1000)
                    {DECCM = 39000;return (dvdecc >= 39000);}

                    if(alturaVuelo == 2000)
                    {DECCM = 39000;return (dvdecc >= 39000);}

                    if(alturaVuelo == 4000)
                    {DECCM = 40000;return (dvdecc >= 40000);}

                    if(alturaVuelo == 6000)
                    {DECCM = 44000;return (dvdecc >= 44000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 45000;return (dvdecc >= 45000);}

                    if(alturaVuelo == 17000)
                    {DECCM = 52000;return (dvdecc >= 52000);}

                    if(alturaVuelo == 30000)
                    {DECCM = 70000;return (dvdecc >= 70000);}
                    break;

                case 14:
                    if(alturaVuelo <= 1000)
                    {DECCM = 50300;return (dvdecc >= 50300);}

                    if(alturaVuelo == 2000)
                    {DECCM = 50300;return (dvdecc >= 50300);}

                    if(alturaVuelo == 4000)
                    {DECCM = 51300;return (dvdecc >= 51300);}

                    if(alturaVuelo == 6000)
                    {DECCM = 55300;return (dvdecc >= 55300);}

                    if(alturaVuelo == 10000)
                    {DECCM = 56300;return (dvdecc >= 56300);}

                    if(alturaVuelo == 17000)
                    {DECCM = 63300;return (dvdecc >= 63300);}

                    if(alturaVuelo == 30000)
                    {DECCM = 81300;return (dvdecc >= 81300);}
                    break;

                case 25:
                    if(alturaVuelo <= 2000)
                    {DECCM = 60800;return (dvdecc >= 60800);}
                    if(alturaVuelo == 4000)
                    {DECCM = 61800;return (dvdecc >= 61800);}
                    if(alturaVuelo == 6000)
                    {DECCM = 65800;return (dvdecc >= 65800);}
                    if(alturaVuelo == 10000)
                    {DECCM = 66800;return (dvdecc >= 66800);}
                    if(alturaVuelo == 17000)
                    {DECCM = 73800;return (dvdecc >= 73800);}
                    if(alturaVuelo == 30000)
                    {DECCM = 91800;return (dvdecc >= 91800);}
                    break;
                case 35:
                    if(alturaVuelo <= 1000)
                    {DECCM = 70300;return (dvdecc >= 70300);}
                    if(alturaVuelo == 2000)
                    {DECCM = 70300;return (dvdecc >= 70300);}
                    if(alturaVuelo == 4000)
                    {DECCM = 71300;return (dvdecc >= 71300);}
                    if(alturaVuelo == 6000)
                    {DECCM = 75300;return (dvdecc >= 75300);}
                    if(alturaVuelo == 10000)
                    {DECCM = 76300;return (dvdecc >= 76300);}
                    if(alturaVuelo == 17000)
                    {DECCM = 83300;return (dvdecc >= 83300);}
                    if(alturaVuelo == 30000)
                    {DECCM = 101250;return (dvdecc >= 101250);}
                    break;
                default:
                    break;
                }
            }
        }
        else
        {

            if(velocidad == 150)
            {
                switch (t)
                {
                case 0:
                case 14:
                    if(alturaVuelo <= 6000)
                    {DECCM = 8000;return (dvdecc >= 8000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 9500;return (dvdecc >= 9500);}

                    if(alturaVuelo == 17000)
                    {DECCM = 12000;return (dvdecc >= 12000);}
                    break;

                case 25:
                    if(alturaVuelo <= 6000)
                    {DECCM = 9500;return (dvdecc >= 9500);}

                    if(alturaVuelo == 10000)
                    {DECCM = 11000;return (dvdecc >= 11000);}

                    if(alturaVuelo == 17000)
                    {DECCM = 13500;return (dvdecc >= 13500);}
                    break;

                case 35:
                    if(alturaVuelo <= 6000)
                    {DECCM = 11000;return (dvdecc >= 11000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 12500;return (dvdecc >= 12500);}

                    if(alturaVuelo == 17000)
                    {DECCM = 15000;return (dvdecc >= 15000);}
                    break;
                }
            }
            if(velocidad == 300)
            {
                switch (t)
                {
                case 0:
                case 14:
                    if(alturaVuelo <= 6000)
                    {DECCM = 10353;return (dvdecc >= 10353);}

                    if(alturaVuelo == 10000)
                    {DECCM = 12872;return (dvdecc >= 12872);}

                    if(alturaVuelo == 17000)
                    {DECCM = 16379;return (dvdecc >= 16379);}
                    break;

                case 25:
                    if(alturaVuelo <= 6000)
                    {DECCM = 14000;return (dvdecc >= 14000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 16000;return (dvdecc >= 16000);}

                    if(alturaVuelo == 17000)
                    {DECCM = 20000;return (dvdecc >= 20000);}
                    break;

                case 35:
                    if(alturaVuelo <= 6000)
                    {DECCM = 17000;return (dvdecc >= 17000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 19000;return (dvdecc >= 19000);}

                    if(alturaVuelo == 17000)
                    {DECCM = 23000;return (dvdecc >= 23000);}
                    break;
                }
            }
            if(velocidad == 420)
            {
                switch (t)
                {
                case 0:
                case 14:
                    if(alturaVuelo <= 4000)
                    {DECCM = 13500;return (dvdecc >= 13500);}

                    if(alturaVuelo == 6000)
                    {DECCM = 14000;return (dvdecc >= 14000);}

                    if(alturaVuelo == 10000)
                    {DECCM = 17800;return (dvdecc >= 17800);}

                    if(alturaVuelo == 17000)
                    {DECCM = 22600;return (dvdecc >= 22600);}
                    break;

                case 25:
                    if(alturaVuelo <= 4000)
                    {DECCM = 18000;return (dvdecc >= 18000);}

                    if(alturaVuelo == 6000)
                    {DECCM = 18500;return (dvdecc >= 18500);}

                    if(alturaVuelo == 10000)
                    {DECCM = 22000;return (dvdecc >= 22000);}

                    if(alturaVuelo == 17000)
                    {DECCM = 26800;return (dvdecc >= 26800);}
                    break;

                case 35:
                    if(alturaVuelo <= 4000)
                    {DECCM = 22000;return (dvdecc >= 22000);}

                    if(alturaVuelo == 6000)
                    {DECCM = 22700;return (dvdecc >= 22700);}

                    if(alturaVuelo == 10000)
                    {DECCM = 26200;return (dvdecc >= 26200);}

                    if(alturaVuelo == 17000)
                    {DECCM = 31000;return (dvdecc >= 31000);}
                    break;
                }
            }
        }
    }
    if(calculos->medio == 38)
    {
        if(!alcance)
        {
            if(velocidad == 150)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 5100;return (dvdecc >= 5100);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 5100;return (dvdecc >= 5100);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 5800;return (dvdecc >= 5800);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 5800;return (dvdecc >=5800);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 7800;return (dvdecc >= 7800);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 8800;return (dvdecc >= 8800);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 10800;return (dvdecc >= 10800);
                    }
                    break;

                case 15:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 7050;return (dvdecc >= 7050);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 7050;return (dvdecc >= 7050);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 7750;return (dvdecc >= 7750);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 7750;return (dvdecc >= 7750);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 9750;return (dvdecc >= 9750);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 10750;return (dvdecc >= 10750);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 12750;return (dvdecc >= 12750);
                    }
                    break;

                case 25:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 8550;return (dvdecc >= 8550);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 8550;return (dvdecc >= 8550);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 9250;return (dvdecc >= 9250);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 9250;return (dvdecc >= 9250);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 11250;return (dvdecc >= 11250);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 12250;return (dvdecc >= 12250);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 14250;return (dvdecc >= 14250);
                    }
                    break;
                }
            }
            if(velocidad == 300)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 7100;return (dvdecc >= 7100);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 7100;return (dvdecc >= 7100);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 8100;return (dvdecc >= 8100);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 9100;return (dvdecc >= 9100);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 10100;return (dvdecc >= 10100);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 11100;return (dvdecc >= 11100);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 13100;return (dvdecc >= 13100);
                    }
                    break;

                case 15:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 11000;return (dvdecc >= 11000);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 11400;return (dvdecc >= 11400);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 12400;return (dvdecc >= 12400);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 13400;return (dvdecc >= 13400);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 14400;return (dvdecc >= 14400);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 15400;return (dvdecc >= 15400);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 17400;return (dvdecc >= 17400);
                    }
                    break;

                case 25:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 14000;return (dvdecc >= 14000);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 14400;return (dvdecc >= 14400);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 15400;return (dvdecc >= 15400);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 16400;return (dvdecc >= 16400);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 17400;return (dvdecc >= 17400);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 18400;return (dvdecc >= 18400);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 20400;return (dvdecc >= 20400);
                    }
                    break;
                }
            }
            if(velocidad == 420)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 7640;return (dvdecc >= 7640);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 7640;return (dvdecc >= 7640);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 8340;return (dvdecc >= 8340);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 9340;return (dvdecc >=9340);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 10340;return (dvdecc >= 10340);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 11340;return (dvdecc >= 11340);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    break;

                case 15:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 13100;return (dvdecc >= 13100);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 13100;return (dvdecc >= 13100);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 13800;return (dvdecc >= 13800);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 14800;return (dvdecc >= 14800);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 15800;return (dvdecc >= 15800);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 16800;return (dvdecc >= 16800);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    break;

                case 25:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 17300;return (dvdecc >= 17300);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 17300;return (dvdecc >= 17300);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 18000;return (dvdecc >= 18000);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 19000;return (dvdecc >= 19000);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 20000;return (dvdecc >= 20000);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 21000;return (dvdecc >= 21000);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    break;
                }
            }
            if(velocidad == 640)
            {
                switch (t)
                {
                case 0:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 9780;return (dvdecc >= 9780);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 10780;return (dvdecc >= 10780);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 10780;return (dvdecc >=10780);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 10780;return (dvdecc >= 10780);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 11780;return (dvdecc >= 11780);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    break;

                case 15:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 18100;return (dvdecc >= 18100);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 19100;return (dvdecc >= 19100);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 19100;return (dvdecc >= 19100);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 19100;return (dvdecc >= 19100);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 20100;return (dvdecc >= 20100);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    break;

                case 25:
                    if(alturaVuelo <= 400)
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    else  if(alturaVuelo <= 1000)
                    {
                        DECCM = 24500;return (dvdecc >= 24500);
                    }
                    else if(alturaVuelo == 2000)
                    {
                        DECCM = 25500;return (dvdecc >= 25500);
                    }

                    else if(alturaVuelo == 4000)
                    {
                        DECCM = 25500;return (dvdecc >= 25500);
                    }

                    else if(alturaVuelo == 6000)
                    {
                        DECCM = 25500;return (dvdecc >= 25500);
                    }
                    else if(alturaVuelo == 10000)
                    {
                        DECCM = 26500;return (dvdecc >= 26500);
                    }
                    else /*(alturaVuelo == 17000)*/
                    {
                        DECCM = 0;return (dvdecc >= 0);
                    }
                    break;
                }
            }
        }
        else
        {
            if(velocidad == 150)
            {
                if(alturaVuelo == 500)
                {
                    DECCM = 16000;return (dvdecc >= 16000);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 16000;return (dvdecc >= 16000);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 18000;return (dvdecc >= 18000);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 19000;return (dvdecc >=19000);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 22000;return (dvdecc >= 22000);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 22000;return (dvdecc >= 22000);
                }

            }
            if(velocidad == 300)
            {
                if(alturaVuelo == 500)
                {
                    DECCM = 17000;return (dvdecc >= 17000);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 17000;return (dvdecc >= 17000);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 18000;return (dvdecc >= 18000);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 19000;return (dvdecc >= 19000);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 22000;return (dvdecc >= 22000);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 22000;return (dvdecc >= 22000);
                }
            }
        }
    }

    if(calculos->medio == 37)
    {
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0://5100		5100		6100		7100		8100		9100

                if(alturaVuelo <= 400)
                {
                    DECCM = 5100;return (dvdecc >= 5100);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 5100;return (dvdecc >= 5100);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 6100;return (dvdecc >= 6100);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 7100;return (dvdecc >=7100);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 8100;return (dvdecc >= 8100);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 9100;return (dvdecc >=9100);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 24://9000		9000		10000		11000		12000		13000

                if(alturaVuelo <= 400)
                {
                    DECCM = 9000;return (dvdecc >= 9000);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 9000;return (dvdecc >= 9000);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 10000;return (dvdecc >= 10000);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 11000;return (dvdecc >= 11000);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 12000;return (dvdecc >= 12000);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 13000;return (dvdecc >= 13000);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 400)
                {
                    DECCM = 5400;return (dvdecc >= 5400);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 5400;return (dvdecc >=5400);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 6400;return (dvdecc >= 6400);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 7400;return (dvdecc >= 7400);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 8400;return (dvdecc >= 8400);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 9400;return (dvdecc >= 9400);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 24:
                if(alturaVuelo <= 400)
                {
                    DECCM = 13200;return (dvdecc >= 13200);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 13200;return (dvdecc >= 13200);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 14200;return (dvdecc >= 14200);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 15200;return (dvdecc >= 15200);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 16200;return (dvdecc >= 16200);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 17200;return (dvdecc >= 17200);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0://6640		6640		7640		8640		9640		10640
                if(alturaVuelo <= 400)
                {
                    DECCM = 6640;return (dvdecc >= 6640);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 6640;return (dvdecc >= 6640);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 7640;return (dvdecc >= 7640);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 8640;return (dvdecc >=8640);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 9640;return (dvdecc >= 9640);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 10640;return (dvdecc >= 10640);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 24://17560		17560		18560		19560		20560		21560

                if(alturaVuelo <= 400)
                {
                    DECCM = 17560;return (dvdecc >= 17560);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 17560;return (dvdecc >= 17560);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 18560;return (dvdecc >= 18560);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 19560;return (dvdecc >= 19560);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 20560;return (dvdecc >= 20560);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 21560;return (dvdecc >= 21560);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 640)
        {
            switch (t)
            {
            case 0://9080		9080		9080		10080		11080		12080
                if(alturaVuelo <= 400)
                {
                    DECCM = 9080;return (dvdecc >= 9080);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 9080;return (dvdecc >= 9080);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 9080;return (dvdecc >= 9080);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 10080;return (dvdecc >=10080);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 11080;return (dvdecc >= 11080);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 12080;return (dvdecc >= 12080);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 24://25720		25720		25720		26720		27720		28720
                if(alturaVuelo <= 400)
                {
                    DECCM = 25720;return (dvdecc >= 25720);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 25720;return (dvdecc >= 25720);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 25720;return (dvdecc >= 25720);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 26720;return (dvdecc >= 26720);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 27720;return (dvdecc >= 27720);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 28720;return (dvdecc >= 28720);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 36)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0://3800		3800		4800		5800
                if(alturaVuelo <= 400)
                {
                    DECCM = 3800;return (dvdecc >= 3800);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 3800;return (dvdecc >= 3800);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 4800;return (dvdecc >= 4800);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 5800;return (dvdecc >=5800);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 16://8000		8000		9000		10000
                if(alturaVuelo <= 400)
                {
                    DECCM = 8000;return (dvdecc >= 8000);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 8000;return (dvdecc >= 8000);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 9000;return (dvdecc >= 9000);
                }

                else if(alturaVuelo == 4000)
                {
                    DECCM = 10000;return (dvdecc >= 10000);
                }

                else if(alturaVuelo == 6000)
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                else if(alturaVuelo == 10000)
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0://3500		3500		3500		3500
                if(alturaVuelo <= 4000)
                {
                    DECCM = 3500;return (dvdecc >= 3500);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 16://5600		5600		5600		5600
                if(alturaVuelo <= 4000)
                {
                    DECCM = 5600;return (dvdecc >= 5600);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0://5040		5040		5040		6040
                if(alturaVuelo <= 2000)
                {
                    DECCM = 5040;return (dvdecc >= 5040);
                }
                else if(alturaVuelo == 4000)
                {
                    DECCM = 6040;return (dvdecc >=6040);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 16://10920		10920		10920		11920
                if(alturaVuelo <= 2000)
                {
                    DECCM = 10920;return (dvdecc >= 10920);
                }
                else if(alturaVuelo == 4000)
                {
                    DECCM = 11920;return (dvdecc >= 11920);
                }
                else /*(alturaVuelo == 17000)*/
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 35)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 2300;return (dvdecc >= 2300);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 2800;return (dvdecc >= 2800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 3200;return (dvdecc >= 3200);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 3800;return (dvdecc >= 3800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 2300;return (dvdecc >= 2300);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 2800;return (dvdecc >= 2800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 3200;return (dvdecc >= 3200);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 3700;return (dvdecc >= 3700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 3020;return (dvdecc >= 3020);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 4300;return (dvdecc >= 4300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
                break;
            }
        }
    }
    if(calculos->medio == 34)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 850;return (dvdecc >= 850);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 990;return (dvdecc >= 990);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 1000)
                {
                    DECCM = 2150;return (dvdecc >= 2150);
                }
                else if(alturaVuelo == 2000)
                {
                    DECCM = 2250;return (dvdecc >= 2250);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 33)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1900;return (dvdecc >= 1900);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 2800;return (dvdecc >= 2800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1300;return (dvdecc >= 1300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1750;return (dvdecc >= 1750);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 2540;return (dvdecc >= 2540);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 3800;return (dvdecc >= 3800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 32)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1900;return (dvdecc >= 1900);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 2800;return (dvdecc >= 2800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1300;return (dvdecc >= 1300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1750;return (dvdecc >= 1750);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 2440;return (dvdecc >= 2440);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 3700;return (dvdecc >= 3700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 31)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1800;return (dvdecc >= 1800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 2700;return (dvdecc >= 2700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1300;return (dvdecc >= 1300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 1750;return (dvdecc >= 1750);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 2340;return (dvdecc >= 2340);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 2000)
                {
                    DECCM = 3600;return (dvdecc >= 3600);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }

    if(calculos->medio == 27)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >=800);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 950;return (dvdecc >= 950);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1100;return (dvdecc >= 1100);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 2500;return (dvdecc >= 2500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1525;return (dvdecc >= 1525);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 1580;return (dvdecc >= 1580);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 1650;return (dvdecc >= 1650);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1800;return (dvdecc >=1800);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1950;return (dvdecc >= 1950);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 2400;return (dvdecc >= 2400);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 3400;return (dvdecc >= 3400);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 380;return (dvdecc >= 380);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 450;return (dvdecc >= 450);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 600;return (dvdecc >= 600);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 750;return (dvdecc >= 750);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1200;return (dvdecc >= 1200);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 2500;return (dvdecc >= 2500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 775;return (dvdecc >= 775);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 830;return (dvdecc >= 830);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 900;return (dvdecc >= 900);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1050;return (dvdecc >=1050);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1200;return (dvdecc >= 1200);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1650;return (dvdecc >= 1650);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 2650;return (dvdecc >= 2650);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 920;return (dvdecc >= 920);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 990;return (dvdecc >= 990);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1140;return (dvdecc >= 1140);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1540;return (dvdecc >= 1540);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1940;return (dvdecc >= 1940);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 2940;return (dvdecc >= 2940);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 4225;return (dvdecc >= 4225);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 4280;return (dvdecc >= 4280);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 4350;return (dvdecc >= 4350);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 4550;return (dvdecc >= 4550);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 4900;return (dvdecc >= 4900);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 5300;return (dvdecc >= 5300);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 6300;return (dvdecc >= 6300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 26 )
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >=800);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 950;return (dvdecc >= 950);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1100;return (dvdecc >= 1100);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1525;return (dvdecc >= 1525);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1700;return (dvdecc >=1700);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1850;return (dvdecc >= 1850);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 2000;return (dvdecc >= 2000);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 500;return (dvdecc >= 500);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 775;return (dvdecc >= 775);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 850;return (dvdecc >= 850);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 950;return (dvdecc >=950);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1100;return (dvdecc >= 1100);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1250;return (dvdecc >= 1250);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1040;return (dvdecc >= 1040);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1190;return (dvdecc >= 1190);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1190;return (dvdecc >= 1190);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 2125;return (dvdecc >= 2125);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 2150;return (dvdecc >= 2150);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 2200;return (dvdecc >= 2200);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 2300;return (dvdecc >= 2300);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 2450;return (dvdecc >= 2450);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 2450;return (dvdecc >= 2450);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }

    if(calculos->medio == 25)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >=800);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 950;return (dvdecc >= 950);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1525;return (dvdecc >= 1525);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1700;return (dvdecc >=1700);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1850;return (dvdecc >= 1850);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 500;return (dvdecc >= 500);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 775;return (dvdecc >= 775);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 850;return (dvdecc >= 850);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 950;return (dvdecc >=950);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1100;return (dvdecc >= 1100);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1040;return (dvdecc >= 1040);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1190;return (dvdecc >= 1190);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 2125;return (dvdecc >= 2125);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 2150;return (dvdecc >= 2150);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 2200;return (dvdecc >= 2200);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 2300;return (dvdecc >= 2300);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 2450;return (dvdecc >= 2450);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 24)
    {
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 500;return (dvdecc >= 500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 775;return (dvdecc >= 775);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 850;return (dvdecc >= 850);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 950;return (dvdecc >= 950);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >=650);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1525;return (dvdecc >= 1525);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 1600;return (dvdecc >=1600);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1700;return (dvdecc >= 1700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1040;return (dvdecc >= 1040);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 2125;return (dvdecc >= 2125);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 2150;return (dvdecc >= 2150);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 2200;return (dvdecc >= 2200);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 2300;return (dvdecc >= 2300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 23 )
    {
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 500;return (dvdecc >= 500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 775;return (dvdecc >= 775);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 850;return (dvdecc >= 850);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 950;return (dvdecc >= 950);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >=700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1525;return (dvdecc >= 1525);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1700;return (dvdecc >= 1700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1040;return (dvdecc >= 1040);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 2125;return (dvdecc >= 2125);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 2150;return (dvdecc >= 2150);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 2200;return (dvdecc >= 2200);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 2300;return (dvdecc >= 2300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 22)
    {
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 500;return (dvdecc >= 500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 775;return (dvdecc >= 775);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 850;return (dvdecc >= 850);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 900;return (dvdecc >= 900);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1525;return (dvdecc >= 1525);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1700;return (dvdecc >= 1700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1040;return (dvdecc >= 1040);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 2125;return (dvdecc >= 2125);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 2150;return (dvdecc >= 2150);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 2200;return (dvdecc >= 2200);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 2300;return (dvdecc >= 2300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 21)
    {
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 775;return (dvdecc >= 775);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 850;return (dvdecc >= 850);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1525;return (dvdecc >= 1525);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 5:
                if(alturaVuelo <= 100)
                {
                    DECCM = 2125;return (dvdecc >=2125);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 2150;return (dvdecc >= 2150);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 2200;return (dvdecc >= 2200);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }

    if(calculos->medio == 44)
    {
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 500;return (dvdecc >= 500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM =1700;return (dvdecc >= 1700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1900;return (dvdecc >= 1900);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >=700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 3050;return (dvdecc >= 3050);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 3100;return (dvdecc >= 3100);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 3200;return (dvdecc >= 3200);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 3400;return (dvdecc >= 3400);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1040;return (dvdecc >= 1040);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 4250;return (dvdecc >= 4250);
                }
                else  if(alturaVuelo <= 500)
                {
                    DECCM = 4300;return (dvdecc >= 4300);
                }
                else  if(alturaVuelo <= 1000)
                {
                    DECCM = 4400;return (dvdecc >=4400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 4600;return (dvdecc >= 4600);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 43)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >=800);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 950;return (dvdecc >= 950);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1100;return (dvdecc >= 1100);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 3050;return (dvdecc >= 3050);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 3100;return (dvdecc >= 3100);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 3200;return (dvdecc >= 3200);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 3400;return (dvdecc >=3400);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 3700;return (dvdecc >= 3700);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 4000;return (dvdecc >= 4000);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 350;return (dvdecc >= 350);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 400;return (dvdecc >= 400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 500;return (dvdecc >= 500);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 800;return (dvdecc >= 800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 1600;return (dvdecc >= 1600);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 1700;return (dvdecc >= 1700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1900;return (dvdecc >=1900);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 2200;return (dvdecc >= 2200);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 2500;return (dvdecc >= 2500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 890;return (dvdecc >= 890);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 940;return (dvdecc >= 940);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1040;return (dvdecc >= 1040);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1190;return (dvdecc >= 1190);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1190;return (dvdecc >= 1190);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 4250;return (dvdecc >= 4250);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 4300;return (dvdecc >= 4300);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 4400;return (dvdecc >= 4400);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 4600;return (dvdecc >= 4600);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 4900;return (dvdecc >= 4900);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 4900;return (dvdecc >= 4900);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
    if(calculos->medio == 42)
    {
        if(velocidad == 300)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 625;return (dvdecc >= 625);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 650;return (dvdecc >= 650);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 700;return (dvdecc >= 700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 800;return (dvdecc >=800);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 950;return (dvdecc >= 950);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1100;return (dvdecc >= 1100);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 2500;return (dvdecc >= 2500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 3050;return (dvdecc >= 3050);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 3160;return (dvdecc >= 3160);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 3300;return (dvdecc >= 3300);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 3600;return (dvdecc >=3600);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 3900;return (dvdecc >= 3900);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 4800;return (dvdecc >= 4800);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 6800;return (dvdecc >= 6800);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
        if(velocidad == 150)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 325;return (dvdecc >= 325);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 380;return (dvdecc >= 380);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 450;return (dvdecc >= 450);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 600;return (dvdecc >= 600);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 750;return (dvdecc >= 750);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1200;return (dvdecc >= 1200);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 2500;return (dvdecc >= 2500);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 1550;return (dvdecc >= 1550);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 1660;return (dvdecc >= 1660);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 1800;return (dvdecc >= 1800);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 2100;return (dvdecc >=2100);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 2400;return (dvdecc >= 2400);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 3300;return (dvdecc >= 3300);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 5300;return (dvdecc >= 5300);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            }
        }
        if(velocidad == 420)
        {
            switch (t)
            {
            case 0:
                if(alturaVuelo <= 100)
                {
                    DECCM = 865;return (dvdecc >= 865);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 920;return (dvdecc >= 920);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 990;return (dvdecc >= 990);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 1140;return (dvdecc >= 1140);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 1540;return (dvdecc >= 1540);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 1940;return (dvdecc >= 1940);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 2940;return (dvdecc >= 2940);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;

            case 10:
                if(alturaVuelo <= 100)
                {
                    DECCM = 8450;return (dvdecc >= 8450);
                }
                else if(alturaVuelo <= 500)
                {
                    DECCM = 8560;return (dvdecc >= 8560);
                }
                else if(alturaVuelo <= 1000)
                {
                    DECCM = 8700;return (dvdecc >= 8700);
                }
                else if(alturaVuelo <= 2000)
                {
                    DECCM = 9100;return (dvdecc >= 9100);
                }
                else if(alturaVuelo <= 4000)
                {
                    DECCM = 9800;return (dvdecc >= 9800);
                }
                else if(alturaVuelo <= 6000)
                {
                    DECCM = 10600;return (dvdecc >= 10600);
                }
                else if(alturaVuelo <= 10000)
                {
                    DECCM = 12600;return (dvdecc >= 12600);
                }
                else
                {
                    DECCM = 0;return (dvdecc >= 0);
                }
                break;
            }
        }
    }
}

bool distanThread::comprobarDlanza(double *dl,double dmaxAzzi)
{
    if(calculos->medio == 39 )
    {
        if(!alcance)
        {
            if(velocidad == 150)
            {
                if(Vuelo <= 1000)
                {DLCmin = 10000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 10000)
                    {
                        if(*dl > 31000)
                        {
                            *dl = 31000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;


                }
                if(Vuelo == 2000)
                {DLCmin = 110000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 11000)
                    {
                        if(*dl > 34000)
                        {
                            *dl = 34000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 12000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 12000)
                    {
                        if(*dl > 45000)
                        {
                            *dl = 45000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 14000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 14000)
                    {
                        if(*dl > 49000)
                        {
                            *dl = 49000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 16000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 16000)
                    {
                        if(*dl > 54000)
                        {
                            *dl = 54000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 22000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 22000)
                    {
                        if(*dl > 54000)
                        {
                            *dl = 54000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DLCmin = 29000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 29000)
                    {
                        if(*dl > 54000)
                        {
                            *dl = 54000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
            }
            if(velocidad == 300)
            {
                if(Vuelo <= 1000)
                {DLCmin = 12000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 12000)
                    {
                        if(*dl > 37000)
                        {
                            *dl = 37000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 2000)
                {DLCmin = 12000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 12000)
                    {
                        if(*dl > 37000)
                        {
                            *dl = 37000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 13000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 13000)
                    {
                        if(*dl > 54000)
                        {
                            *dl = 54000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 16000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 16000)
                    {
                        if(*dl > 58000)
                        {
                            *dl = 58000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 19000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 19000)
                    {
                        if(*dl > 68000)
                        {
                            *dl = 68000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 26000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 26000)
                    {
                        if(*dl > 73000)
                        {
                            *dl = 73000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DLCmin = 34000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 34000)
                    {
                        if(*dl > 66000)
                        {
                            *dl = 66000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
            }
            if(velocidad == 420)
            {
                if(Vuelo <= 1000)
                {DLCmin = 15000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 15000)
                    {
                        if(*dl > 41000)
                        {
                            *dl = 41000;
                        }
                        if(*dl >= dmaxAzzi)
                            *dl = dmaxAzzi - 840;
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 2000)
                {DLCmin = 16000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 16000)
                    {
                        if(*dl > 51000)
                        {
                            *dl = 51000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 18000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 18000)
                    {
                        if(*dl > 54000)
                        {
                            *dl = 54000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 19000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 19000)
                    {
                        if(*dl > 58000)
                        {
                            *dl = 58000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 19000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 19000)
                    {
                        if(*dl > 69000)
                        {
                            *dl = 69000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 26000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 26000)
                    {
                        if(*dl > 69000)
                        {
                            *dl = 69000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DLCmin = 34000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 34000)
                    {
                        if(*dl > 69000)
                        {
                            *dl = 69000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
            }
            if(velocidad == 640)
            {
                if(Vuelo <= 1000)
                {DLCmin = 17000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 17000)
                    {
                        if(*dl > 49000)
                        {
                            *dl = 49000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 2000)
                {DLCmin = 17000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 17000)
                    {
                        if(*dl > 49000)
                        {
                            *dl = 49000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 18000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 18000)
                    {
                        if(*dl > 51000)
                        {
                            *dl = 51000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 21000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 21000)
                    {
                        if(*dl > 54000)
                        {
                            *dl = 54000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 27000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 27000)
                    {
                        if(*dl > 62000)
                        {
                            *dl = 62000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 36000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 36000)
                    {
                        if(*dl > 75000)
                        {
                            *dl = 75000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DLCmin = 68000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 68000)
                    {
                        if(*dl > 84000)
                        {
                            *dl = 84000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
            }
            if(velocidad == 950)
            {
                if(Vuelo <= 1000)
                {DLCmin = 37000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1900;
                    if(*dl >= 37000)
                    {
                        if(*dl > 51000)
                        {
                            *dl = 51000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 2000)
                {DLCmin = 37000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1900;
                    if(*dl >= 37000)
                    {
                        if(*dl > 56000)
                        {
                            *dl = 56000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 38000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1900;
                    if(*dl >= 38000)
                    {
                        if(*dl > 61000)
                        {
                            *dl = 61000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 42000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1900;
                    if(*dl >= 42000)
                    {
                        if(*dl > 63000)
                        {
                            *dl = 63000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 43000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1900;
                    if(*dl >= 43000)
                    {
                        if(*dl > 70000)
                        {
                            *dl = 70000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 50000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1900;
                    if(*dl >= 50000)
                    {
                        if(*dl > 78000)
                        {
                            *dl = 78000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DLCmin = 68000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1900;
                    if(*dl >= 68000)
                    {
                        if(*dl > 85000)
                        {
                            *dl = 85000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
            }
        }
        else
        {
            if(velocidad == 150)
            {
                if(Vuelo <= 1000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 6000)
                    {
                        if(*dl > 17000)
                        {
                            *dl = 17000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;


                }
                if(Vuelo == 2000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 6000)
                    {
                        if(*dl > 17000)
                        {
                            *dl = 17000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 6000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 6000)
                    {
                        if(*dl > 18000)
                        {
                            *dl = 18000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 6000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 6000)
                    {
                        if(*dl > 20000)
                        {
                            *dl = 20000;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 7000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 7000)
                    {
                        if(*dl > 25000)
                        {
                            *dl = 25000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 9000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 9000)
                    {
                        if(*dl > 30000)
                        {
                            *dl = 30000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                //                if(Vuelo == 30000)
                //                {DLCmin = 29000;
                //                    if(*dl >= dmaxAzzi)
                //                        *dl = dmaxAzzi - 300;
                //                    if(*dl >= 29000)
                //                    {
                //                        if(*dl > 54000)
                //                        {
                //                            *dl = 54000;
                //                        }
                //                        return true;
                //                    }


                //                    *dl = 0;
                //                    return false;

                //                }
            }
            if(velocidad == 300)
            {
                if(Vuelo <= 1000)
                {DLCmin = 6000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 6000)
                    {
                        if(*dl > 11000)
                        {
                            *dl = 11000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 2000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 6000)
                    {
                        if(*dl > 11000)
                        {
                            *dl = 11000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 6000)
                    {
                        if(*dl > 12000)
                        {
                            *dl = 12000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 6000)
                    {
                        if(*dl > 13500)
                        {
                            *dl = 13500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 7000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 7000)
                    {
                        if(*dl > 16000)
                        {
                            *dl = 16000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 9000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 9000)
                    {
                        if(*dl > 18000)
                        {
                            *dl = 18000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                //                if(Vuelo == 30000)
                //                {DLCmin = 34000;
                //                    if(*dl >= dmaxAzzi)
                //                        *dl = dmaxAzzi - 600;
                //                    if(*dl >= 34000)
                //                    {
                //                        if(*dl > 66000)
                //                        {
                //                            *dl = 66000;
                //                        }

                //                        return true;
                //                    }


                //                    *dl = 0;
                //                    return false;

                //                }
            }
            if(velocidad == 420)
            {
                if(Vuelo <= 1000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 6000)
                    {
                        if(*dl > 9000)
                        {
                            *dl = 9000;
                        }
                        if(*dl >= dmaxAzzi)
                            *dl = dmaxAzzi - 840;
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 2000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 6000)
                    {
                        if(*dl > 9000)
                        {
                            *dl = 9000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 6000)
                    {
                        if(*dl > 10000)
                        {
                            *dl = 10000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DLCmin = 6000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 6000)
                    {
                        if(*dl > 12000)
                        {
                            *dl = 12000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DLCmin = 7000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 7000)
                    {
                        if(*dl > 14000)
                        {
                            *dl = 14000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DLCmin = 11000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 11000)
                    {
                        if(*dl > 16000)
                        {
                            *dl = 16000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                //                if(Vuelo == 30000)
                //                {DLCmin = 34000;
                //                    if(*dl >= dmaxAzzi)
                //                        *dl = dmaxAzzi - 840;
                //                    if(*dl >= 34000)
                //                    {
                //                        if(*dl > 69000)
                //                        {
                //                            *dl = 69000;
                //                        }

                //                        return true;
                //                    }


                //                    *dl = 0;
                //                    return false;

                //                }
            }
        }
    }

    if(calculos->medio == 38)
    {
        if(!alcance)
        {
            if(velocidad == 150)
            {
                if(Vuelo <= 400)
                {
                    DLCmin = 4800;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 4800)
                    {
                        if(*dl > 14500)
                        {
                            *dl = 14500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;


                }
                else  if(Vuelo <= 1000)
                {DLCmin = 4800;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 4800)
                    {
                        if(*dl > 21500)
                        {
                            *dl = 21500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else if(Vuelo == 2000)
                {DLCmin = 5500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 5500)
                    {
                        if(*dl > 21500)
                        {
                            *dl = 21500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 4000)
                {DLCmin = 5500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 5500)
                    {
                        if(*dl > 22500)
                        {
                            *dl = 22500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 6000)
                {DLCmin = 7500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 7500)
                    {
                        if(*dl > 22500)
                        {
                            *dl = 22500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 10000)
                {DLCmin = 8500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 8500)
                    {
                        if(*dl > 23500)
                        {
                            *dl = 23500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  /*17000*/
                {DLCmin = 10500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 10500)
                    {
                        if(*dl > 28500)
                        {
                            *dl = 28500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
            }
            if(velocidad == 300)
            {
                if(Vuelo <= 400)
                {
                    DLCmin = 6100;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 6100)
                    {
                        if(*dl > 17500)
                        {
                            *dl = 17500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;


                }
                else  if(Vuelo <= 1000)
                {DLCmin = 6500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 6500)
                    {
                        if(*dl > 25500)
                        {
                            *dl = 25500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else if(Vuelo == 2000)
                {DLCmin = 7500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >=7500)
                    {
                        if(*dl > 25500)
                        {
                            *dl = 25500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 4000)
                {DLCmin = 8500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 8500)
                    {
                        if(*dl > 25500)
                        {
                            *dl = 25500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 6000)
                {DLCmin =9500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 9500)
                    {
                        if(*dl > 26500)
                        {
                            *dl = 26500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 10000)
                {DLCmin = 10500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >=10500)
                    {
                        if(*dl > 28500)
                        {
                            *dl = 28500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  /*17000*/
                {DLCmin = 12500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 12500)
                    {
                        if(*dl > 33500)
                        {
                            *dl = 33500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
            }
            if(velocidad == 420)
            {
                if(Vuelo <= 400)
                {
                    DLCmin = 6800;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 6800)
                    {
                        if(*dl > 19500)
                        {
                            *dl = 19500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;


                }
                else  if(Vuelo <= 1000)
                {DLCmin = 6800;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 6800)
                    {
                        if(*dl > 26500)
                        {
                            *dl = 26500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else if(Vuelo == 2000)
                {DLCmin = 7500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 7500)
                    {
                        if(*dl > 28500)
                        {
                            *dl = 28500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 4000)
                {DLCmin = 8500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 8500)
                    {
                        if(*dl > 29500)
                        {
                            *dl = 29500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 6000)
                {DLCmin = 9500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 9500)
                    {
                        if(*dl > 30500)
                        {
                            *dl = 30500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 10000)
                {DLCmin = 10500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 840;
                    if(*dl >= 10500)
                    {
                        if(*dl > 31500)
                        {
                            *dl = 31500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  /*17000*/
                {
                    DLCmin = 0;
                    *dl = 0;
                    return true;
                }
            }
            if(velocidad == 640)
            {
                if(Vuelo <= 400)
                {
                    DLCmin = 8500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 8500)
                    {
                        if(*dl > 21500)
                        {
                            *dl = 21500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;
                }
                if(Vuelo <= 1000)
                {DLCmin = 8500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 8500)
                    {
                        if(*dl > 33500)
                        {
                            *dl = 33500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else if(Vuelo == 2000)
                {DLCmin = 9500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 9500)
                    {
                        if(*dl > 33500)
                        {
                            *dl = 33500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 4000)
                {DLCmin = 9500;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 9500)
                    {
                        if(*dl > 34500)
                        {
                            *dl = 34500;
                        }
                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 6000)
                {DLCmin = 9500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 9500)
                    {
                        if(*dl > 35500)
                        {
                            *dl = 35500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else  if(Vuelo == 10000)
                {DLCmin = 10500;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 1280;
                    if(*dl >= 10500)
                    {
                        if(*dl > 37500)
                        {
                            *dl = 37500;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                else
                {
                    DLCmin = 0;
                    *dl = 0;
                    return false;
                }
            }
        }
        else
        {
            if(velocidad == 150)
            {
                if(Vuelo <= 1000)
                {
                    //                    DLCmin = 4400;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 4400)
                    {
                        if(*dl > 12000)
                        {
                            *dl = 12000;
                        }
                        return true;
                    }
                    *dl = 0;
                    return false;
                }
                else if(Vuelo == 2000)
                {
                    //                    DLCmin = 4400;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 4400)
                    {
                        if(*dl > 14000)
                        {
                            *dl = 14000;
                        }
                        return true;
                    }
                    *dl = 0;
                    return false;
                }
                else if(Vuelo <= 4000)
                {
                    //                    DLCmin = 4400;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 4400)
                    {
                        if(*dl > 15500)
                        {
                            *dl = 15500;
                        }
                        return true;
                    }
                    *dl = 0;
                    return false;
                }
                else if(Vuelo <= 10000)
                {
                    //                    DLCmin = 4400;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 300;
                    if(*dl >= 4400)
                    {
                        if(*dl > 18000)
                        {
                            *dl = 18000;
                        }
                        return true;
                    }
                    *dl = 0;
                    return false;
                }
            }
            if(velocidad == 300)
            {
                if(Vuelo <= 1000)
                {
                    //                    DLCmin = 3000;

                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 3000)
                    {
                        if(*dl > 11000)
                        {
                            *dl = 11000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 2000)
                {
                    //                    DLCmin = 3000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 3000)
                    {
                        if(*dl > 12000)
                        {
                            *dl = 12000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {
                    //                    DLCmin = 3000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 3000)
                    {
                        if(*dl > 13000)
                        {
                            *dl = 13000;
                        }

                        return true;
                    }


                    *dl = 0;
                    return false;

                }
                if(Vuelo <= 10000)
                {
                    //                    DLCmin = 3000;
                    if(*dl >= dmaxAzzi)
                        *dl = dmaxAzzi - 600;
                    if(*dl >= 3000)
                    {
                        if(*dl > 17000)
                        {
                            *dl = 17000;
                        }

                        return true;
                    }

                    *dl = 0;
                    return false;

                }
            }
        }
    }
    if(calculos->medio == 37)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 400)
            {
                DLCmin = 4800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 4800)
                {
                    if(*dl > 21800)
                    {
                        *dl = 21800;
                    }
                    return true;
                }


                *dl = 0;
                return false;


            }
            else  if(Vuelo <= 1000)
            {DLCmin = 4800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 4800)
                {
                    if(*dl > 22800)
                    {
                        *dl = 22800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 5800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 5800)
                {
                    if(*dl > 29800)
                    {
                        *dl = 29800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 4000)
            {DLCmin = 6800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 6800)
                {
                    if(*dl > 30800)
                    {
                        *dl = 30800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 6000)
            {DLCmin = 7800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 7800)
                {
                    if(*dl > 31800)
                    {
                        *dl = 31800;
                    }

                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 10000)
            {DLCmin = 8800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 8800)
                {
                    if(*dl > 31800)
                    {
                        *dl = 31800;
                    }

                    return true;
                }


                *dl = 0;
                return false;

            }
            else  /*17000*/
            {DLCmin = 0;
                //                if(*dl >= dmaxAzzi)
                //                    *dl = dmaxAzzi - 300;
                //                if(*dl >= 10500)
                //                {
                //                    if(*dl > 28500)
                //                    {
                *dl = 0;
                //                    }

                return true;
                //                }
                //                else
                //                {
                //                    *dl = 0;
                //                    return false;
                //                }
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 400)
            {
                DLCmin = 4800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 4800)
                {
                    if(*dl > 21800)
                    {
                        *dl = 21800;
                    }
                    return true;
                }


                *dl = 0;
                return false;


            }
            else  if(Vuelo <= 1000)
            {DLCmin = 4800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 4800)
                {
                    if(*dl > 22800)
                    {
                        *dl = 22800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 5800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >=5800)
                {
                    if(*dl > 30800)
                    {
                        *dl = 30800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 4000)
            {DLCmin = 6800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 6800)
                {
                    if(*dl > 30800)
                    {
                        *dl = 30800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 6000)
            {DLCmin =7800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 7800)
                {
                    if(*dl > 31800)
                    {
                        *dl = 31800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 10000)
            {DLCmin = 8800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >=8800)
                {
                    if(*dl > 31800)
                    {
                        *dl = 31800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 400)
            {
                DLCmin = 5800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 5800)
                {
                    if(*dl > 25800)
                    {
                        *dl = 25800;
                    }
                    return true;
                }


                *dl = 0;
                return false;


            }
            else  if(Vuelo <= 1000)
            {DLCmin = 5800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 5800)
                {
                    if(*dl > 28800)
                    {
                        *dl = 28800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 6800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 6800)
                {
                    if(*dl > 33800)
                    {
                        *dl = 33800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 4000)
            {DLCmin = 7800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 7800)
                {
                    if(*dl > 34800)
                    {
                        *dl = 34800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 6000)
            {DLCmin = 8800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 8800)
                {
                    if(*dl > 35800)
                    {
                        *dl = 35800;
                    }

                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 10000)
            {DLCmin = 9800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 9800)
                {
                    if(*dl > 36800)
                    {
                        *dl = 36800;
                    }

                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;

            }
        }
        if(velocidad == 640)
        {
            if(Vuelo <= 400)
            {
                DLCmin = 7800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 1280;
                if(*dl >= 7800)
                {
                    if(*dl > 30800)
                    {
                        *dl = 30800;
                    }
                    return true;
                }


                *dl = 0;
                return false;


            }
            else  if(Vuelo <= 1000)
            {DLCmin = 7800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 1280;
                if(*dl >= 7800)
                {
                    if(*dl > 32800)
                    {
                        *dl = 32800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 7800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 1280;
                if(*dl >= 7800)
                {
                    if(*dl > 35800)
                    {
                        *dl = 35800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 4000)
            {DLCmin = 8800;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 1280;
                if(*dl >= 8800)
                {
                    if(*dl > 37800)
                    {
                        *dl = 37800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 6000)
            {DLCmin = 9800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 1280;
                if(*dl >= 9800)
                {
                    if(*dl > 37800)
                    {
                        *dl = 37800;
                    }

                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 10000)
            {DLCmin = 10800;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 1280;
                if(*dl >= 10800)
                {
                    if(*dl > 37800)
                    {
                        *dl = 37800;
                    }

                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 36)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 400)
            {
                DLCmin = 3200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 3200)
                {
                    if(*dl > 11200)
                    {
                        *dl = 11200;
                    }
                    return true;
                }


                *dl = 0;
                return false;


            }
            else  if(Vuelo <= 1000)
            {DLCmin = 3200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 3200)
                {
                    if(*dl > 11200)
                    {
                        *dl = 11200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 3200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 3200)
                {
                    if(*dl > 13200)
                    {
                        *dl = 13200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 4000)
            {DLCmin = 3200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 3200)
                {
                    if(*dl > 14200)
                    {
                        *dl = 14200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            //            else  if(Vuelo == 6000)
            //            {DLCmin = 7500;
            //                if(*dl >= dmaxAzzi)
            //                    *dl = dmaxAzzi - 300;
            //                if(*dl >= 7500)
            //                {
            //                    if(*dl > 22500)
            //                    {
            //                        *dl = 22500;
            //                    }

            //                    return true;
            //                }
            //                else
            //                {
            //                    *dl = 0;
            //                    return false;
            //                }
            //            }
            //            else  if(Vuelo == 10000)
            //            {DLCmin = 8500;
            //                if(*dl >= dmaxAzzi)
            //                    *dl = dmaxAzzi - 300;
            //                if(*dl >= 8500)
            //                {
            //                    if(*dl > 23500)
            //                    {
            //                        *dl = 23500;
            //                    }

            //                    return true;
            //                }
            //                else
            //                {
            //                    *dl = 0;
            //                    return false;
            //                }
            //            }
            else  /*17000*/
            {DLCmin = 0;
                //                 if(*dl >= dmaxAzzi)
                //                     *dl = dmaxAzzi - 300;
                //                 if(*dl >= 10500)
                //                 {
                //                     if(*dl > 25500)
                //                     {
                *dl = 0;
                //                     }

                return true;
                //                 }
                //                 else
                //                 {
                //                     *dl = 0;
                //                     return false;
                //                 }
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 400)
            {
                DLCmin = 3200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 3200)
                {
                    if(*dl > 11200)
                    {
                        *dl = 11200;
                    }
                    return true;
                }


                *dl = 0;
                return false;


            }
            else  if(Vuelo <= 1000)
            {DLCmin = 3200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 3200)
                {
                    if(*dl > 11200)
                    {
                        *dl = 11200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 4200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >=4200)
                {
                    if(*dl > 15200)
                    {
                        *dl = 15200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 4000)
            {DLCmin = 5200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 5200)
                {
                    if(*dl > 16200)
                    {
                        *dl = 16200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 400)
            {
                DLCmin = 4200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 4200)
                {
                    if(*dl > 12200)
                    {
                        *dl = 12200;
                    }
                    return true;
                }


                *dl = 0;
                return false;


            }
            else  if(Vuelo <= 1000)
            {DLCmin = 4200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 4200)
                {
                    if(*dl > 12200)
                    {
                        *dl = 12200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 4200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 4200)
                {
                    if(*dl > 16200)
                    {
                        *dl = 16200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo == 4000)
            {DLCmin = 5200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 5200)
                {
                    if(*dl > 17200)
                    {
                        *dl = 17200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 35)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 1000)
            {
                DLCmin = 1300;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 1300)
                {
                    if(*dl > 6700)
                    {
                        *dl = 6700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 1500;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 1500)
                {
                    if(*dl > 6700)
                    {
                        *dl = 6700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 1000)
            {
                DLCmin = 1700;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 1700)
                {
                    if(*dl > 7700)
                    {
                        *dl = 7700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 2200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 2200)
                {
                    if(*dl > 8200)
                    {
                        *dl = 8200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 1000)
            {
                DLCmin = 2200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 2200)
                {
                    if(*dl > 8100)
                    {
                        *dl =8100;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DLCmin = 2200;

                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 2200)
                {
                    if(*dl > 8200)
                    {
                        *dl = 8200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 34)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 1000)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 50)
                {
                    if(*dl > 4700)
                    {
                        *dl = 4700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 100)
                {
                    if(*dl > 4700)
                    {
                        *dl = 4700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 300)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 5700)
                    {
                        *dl = 5700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 6200)
                    {
                        *dl = 6200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            if(Vuelo == 2000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 100)
                {
                    if(*dl > 6200)
                    {
                        *dl = 6200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 1000)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 50)
                {
                    if(*dl > 6700)
                    {
                        *dl = 6700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {
                DLCmin = 150;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 150)
                {
                    if(*dl > 7200)
                    {
                        *dl = 7200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 33)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1000;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 1000)
                {
                    if(*dl > 3800)
                    {
                        *dl = 3800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1300;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 1300)
                {
                    if(*dl > 5200)
                    {
                        *dl = 5200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1700;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 1700)
                {
                    if(*dl > 5700)
                    {
                        *dl = 5700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 32)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1000;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 1000)
                {
                    if(*dl > 3200)
                    {
                        *dl = 3200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1300;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 1300)
                {
                    if(*dl > 4200)
                    {
                        *dl = 4200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1600;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 1600)
                {
                    if(*dl > 4700)
                    {
                        *dl = 4700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 31)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1000;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 1000)
                {
                    if(*dl > 5700)
                    {
                        *dl = 5700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 1200)
                {
                    if(*dl > 6700)
                    {
                        *dl = 6700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 2000)
            {
                DLCmin = 1500;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 1500)
                {
                    if(*dl > 7700)
                    {
                        *dl = 7700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }

    if(calculos->medio == 27|| calculos->medio == 42)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 25)
                {
                    if(*dl > 14700)
                    {
                        *dl = 14700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 80;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 80)
                {
                    if(*dl > 14700)
                    {
                        *dl = 14700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 150;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 150)
                {
                    if(*dl > 14700)
                    {
                        *dl = 14700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin =300;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 300)
                {
                    if(*dl > 14700)
                    {
                        *dl = 14700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 450;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 450)
                {
                    if(*dl > 14000)
                    {
                        *dl = 14000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {
                DLCmin = 900;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 900)
                {
                    if(*dl > 13000)
                    {
                        *dl = 13000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 10000)
            {
                DLCmin = 1700;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 1700)
                {
                    if(*dl > 8100)
                    {
                        *dl = 8100;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 25)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 80;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 80)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 150;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 150)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 300;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 300)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 450;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 450)
                {
                    if(*dl > 17400)
                    {
                        *dl = 17400;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {
                DLCmin = 900;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 900)
                {
                    if(*dl > 16500)
                    {
                        *dl = 16500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 10000)
            {
                DLCmin = 1900;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 1900)
                {
                    if(*dl > 9700)
                    {
                        *dl = 9700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 25)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 80;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 80)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 150;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 150)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 350;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 350)
                {
                    if(*dl >17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 700;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 700)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {
                DLCmin = 1100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 1100)
                {
                    if(*dl > 17500)
                    {
                        *dl = 17500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 10000)
            {
                DLCmin = 2100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 2100)
                {
                    if(*dl > 11000)
                    {
                        *dl = 11000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 26|| calculos->medio == 43)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 25)
                {
                    if(*dl >7500)
                    {
                        *dl = 7500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 50)
                {
                    if(*dl > 7000)
                    {
                        *dl = 7000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 100)
                {
                    if(*dl > 7200)
                    {
                        *dl = 7200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 200)
                {
                    if(*dl > 7100)
                    {
                        *dl = 7100;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 350;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 350)
                {
                    if(*dl > 5500)
                    {
                        *dl = 5500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {
                DLCmin = 500;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 500)
                {
                    if(*dl > 2700)
                    {
                        *dl = 2700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 25)
                {
                    if(*dl > 8700)
                    {
                        *dl = 8700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 8700)
                    {
                        *dl = 8700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 100)
                {
                    if(*dl > 8500)
                    {
                        *dl = 8500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi -600;
                if(*dl >= 200)
                {
                    if(*dl > 8200)
                    {
                        *dl = 8200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 350;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi -600;
                if(*dl >= 350)
                {
                    if(*dl > 6700)
                    {
                        *dl = 6700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {
                DLCmin = 500;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi -600;
                if(*dl >= 500)
                {
                    if(*dl > 3100)
                    {
                        *dl = 3100;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 25)
                {
                    if(*dl > 9500)
                    {
                        *dl = 9500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 50)
                {
                    if(*dl > 9500)
                    {
                        *dl = 9500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 100)
                {
                    if(*dl > 9500)
                    {
                        *dl = 9500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 200)
                {
                    if(*dl > 9500)
                    {
                        *dl = 9500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 350;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 350)
                {
                    if(*dl > 7600)
                    {
                        *dl = 7600;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {
                DLCmin =500;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 500)
                {
                    if(*dl > 3700)
                    {
                        *dl = 3700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }

    if(calculos->medio == 25)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 25)
                {
                    if(*dl > 5000)
                    {
                        *dl = 5000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 50)
                {
                    if(*dl > 4500)
                    {
                        *dl = 4500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 100)
                {
                    if(*dl > 4600)
                    {
                        *dl = 4600;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 200)
                {
                    if(*dl > 4200)
                    {
                        *dl = 4200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 350;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 350)
                {
                    if(*dl > 1800)
                    {
                        *dl = 1800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 25)
                {
                    if(*dl > 5600)
                    {
                        *dl = 5600;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 5600)
                    {
                        *dl = 5600;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 100)
                {
                    if(*dl > 5500)
                    {
                        *dl = 5500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 200)
                {
                    if(*dl > 5000)
                    {
                        *dl = 5000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 350;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 350)
                {
                    if(*dl > 2000)
                    {
                        *dl = 2000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 25)
                {
                    if(*dl > 6500)
                    {
                        *dl = 6500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 50)
                {
                    if(*dl > 6500)
                    {
                        *dl = 6500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 100)
                {
                    if(*dl > 6500)
                    {
                        *dl = 6500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 200)
                {
                    if(*dl > 5500)
                    {
                        *dl = 5500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {
                DLCmin = 350;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 350)
                {
                    if(*dl > 2300)
                    {
                        *dl = 2300;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 24)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 25)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 50)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 100)
                {
                    if(*dl > 3200)
                    {
                        *dl = 3200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 200)
                {
                    if(*dl > 2700)
                    {
                        *dl = 2700;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 25)
                {
                    if(*dl > 4200)
                    {
                        *dl = 4200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 4000)
                    {
                        *dl = 4000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 100)
                {
                    if(*dl > 4000)
                    {
                        *dl = 4000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi -600;
                if(*dl >= 200)
                {
                    if(*dl > 3200)
                    {
                        *dl = 3200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 25)
                {
                    if(*dl > 4500)
                    {
                        *dl = 4500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 50)
                {
                    if(*dl > 4500)
                    {
                        *dl = 4500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 100)
                {
                    if(*dl > 4500)
                    {
                        *dl = 4500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 200)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 23|| calculos->medio == 44)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 25)
                {
                    if(*dl > 3000)
                    {
                        *dl = 3000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 50)
                {
                    if(*dl > 3000)
                    {
                        *dl = 3000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 100)
                {
                    if(*dl > 2500)
                    {
                        *dl = 2500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 200)
                {
                    if(*dl > 1800)
                    {
                        *dl = 1800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 25)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 100)
                {
                    if(*dl > 3000)
                    {
                        *dl = 3000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 200)
                {
                    if(*dl > 2200)
                    {
                        *dl = 2200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 25)
                {
                    if(*dl > 4000)
                    {
                        *dl = 4000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 50)
                {
                    if(*dl > 4000)
                    {
                        *dl = 4000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 100)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 200)
                {
                    if(*dl > 2500)
                    {
                        *dl = 2500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 22)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 25)
                {
                    if(*dl > 3000)
                    {
                        *dl = 3000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 50)
                {
                    if(*dl > 3000)
                    {
                        *dl = 3000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 100)
                {
                    if(*dl > 2500)
                    {
                        *dl = 2500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 200)
                {
                    if(*dl > 1800)
                    {
                        *dl = 1800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 25)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 100)
                {
                    if(*dl > 3000)
                    {
                        *dl = 3000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 200)
                {
                    if(*dl > 2200)
                    {
                        *dl = 2200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 25)
                {
                    if(*dl > 4000)
                    {
                        *dl = 4000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 50)
                {
                    if(*dl > 4000)
                    {
                        *dl = 4000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 100)
                {
                    if(*dl > 3500)
                    {
                        *dl = 3500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {
                DLCmin = 200;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 200)
                {
                    if(*dl > 2500)
                    {
                        *dl = 2500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
    if(calculos->medio == 21)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 25)
                {
                    if(*dl > 1800)
                    {
                        *dl = 1800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 50)
                {
                    if(*dl > 1500)
                    {
                        *dl = 1500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 300;
                if(*dl >= 100)
                {
                    if(*dl > 1200)
                    {
                        *dl = 1200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 25)
                {
                    if(*dl > 2200)
                    {
                        *dl = 2200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 50)
                {
                    if(*dl > 2000)
                    {
                        *dl = 2000;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 600;
                if(*dl >= 100)
                {
                    if(*dl > 1500)
                    {
                        *dl = 1500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {
                DLCmin = 25;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 25)
                {
                    if(*dl > 2500)
                    {
                        *dl = 2500;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {
                DLCmin = 50;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 50)
                {
                    if(*dl > 2200)
                    {
                        *dl = 2200;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DLCmin = 100;
                if(*dl >= dmaxAzzi)
                    *dl = dmaxAzzi - 840;
                if(*dl >= 100)
                {
                    if(*dl > 1800)
                    {
                        *dl = 1800;
                    }
                    return true;
                }


                *dl = 0;
                return false;

            }
            else
            {
                DLCmin = 0;
                *dl = 0;
                return true;
            }
        }
    }
}

bool distanThread::comprobarDZDR(double *dzdr)
{
    if(calculos->medio == 39 )
    {
        if(!alcance)
        {
            if(velocidad == 150)
            {
                if(Vuelo <= 1000)
                {DRCmin = 7000;
                    if(*dzdr >= 7000)
                    {
                        if(*dzdr > 24000)
                        {
                            *dzdr = 24000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {DRCmin = 8000;
                    if(*dzdr >= 8000)
                    {
                        if(*dzdr > 27735)
                        {
                            *dzdr = 27735;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DRCmin = 9000;
                    if(*dzdr >= 9000)
                    {
                        if(*dzdr > 38333)
                        {
                            *dzdr = 38333;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DRCmin = 10300;
                    if(*dzdr >= 10300)
                    {
                        if(*dzdr > 40723)
                        {
                            *dzdr = 40723;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DRCmin = 12046;
                    if(*dzdr >= 12046)
                    {
                        if(*dzdr > 45000)
                        {
                            *dzdr = 45000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DRCmin = 17326;
                    if(*dzdr >= 17326)
                    {
                        if(*dzdr > 45000)
                        {
                            *dzdr = 45000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DRCmin = 23682;
                    if(*dzdr >= 23682)
                    {
                        if(*dzdr > 38571)
                        {
                            *dzdr = 38571;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 300)
            {
                if(Vuelo <= 1000)
                {DRCmin = 7000;
                    if(*dzdr >= 7000)
                    {
                        if(*dzdr > 24000)
                        {
                            *dzdr = 24000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {DRCmin = 7000;
                    if(*dzdr >= 7000)
                    {
                        if(*dzdr > 26000)
                        {
                            *dzdr = 26000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DRCmin = 7500;
                    if(*dzdr >= 7500)
                    {
                        if(*dzdr > 38500)
                        {
                            *dzdr = 38500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DRCmin = 9500;
                    if(*dzdr >= 9500)
                    {
                        if(*dzdr > 41000)
                        {
                            *dzdr = 41000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DRCmin = 11500;
                    if(*dzdr >= 11500)
                    {
                        if(*dzdr > 48500)
                        {
                            *dzdr = 48500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DRCmin = 17000;
                    if(*dzdr >= 17000)
                    {
                        if(*dzdr > 52000)
                        {
                            *dzdr = 52000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DRCmin = 24000;
                    if(*dzdr >= 24000)
                    {
                        if(*dzdr > 47000)
                        {
                            *dzdr = 47000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 420)
            {
                if(Vuelo <= 1000)
                {DRCmin = 7759;
                    if(*dzdr >= 7759)
                    {
                        if(*dzdr > 24216)
                        {
                            *dzdr = 24216;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {DRCmin = 8300;
                    if(*dzdr >= 8300)
                    {
                        if(*dzdr > 33000)
                        {
                            *dzdr = 33000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DRCmin = 8783;
                    if(*dzdr >= 8783)
                    {
                        if(*dzdr > 35000)
                        {
                            *dzdr = 35000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DRCmin = 9500;
                    if(*dzdr >= 9500)
                    {
                        if(*dzdr > 38000)
                        {
                            *dzdr = 38000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DRCmin = 9500;
                    if(*dzdr >= 9500)
                    {
                        if(*dzdr > 45000)
                        {
                            *dzdr = 45000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DRCmin = 15000;
                    if(*dzdr >= 15000)
                    {
                        if(*dzdr > 44500)
                        {
                            *dzdr = 44500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DRCmin = 20000;
                    if(*dzdr >= 20000)
                    {
                        if(*dzdr > 47000)
                        {
                            *dzdr = 47000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 640)
            {
                if(Vuelo <= 1000)
                {DRCmin = 7330;
                    if(*dzdr >= 7330)
                    {
                        if(*dzdr > 24539)
                        {
                            *dzdr = 24539;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {DRCmin = 7330;
                    if(*dzdr >= 7330)
                    {
                        if(*dzdr > 26825)
                        {
                            *dzdr = 26825;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DRCmin = 8000;
                    if(*dzdr >= 8000)
                    {
                        if(*dzdr > 27920)
                        {
                            *dzdr = 27920;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DRCmin = 9200;
                    if(*dzdr >= 9200)
                    {
                        if(*dzdr > 29500)
                        {
                            *dzdr = 29500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DRCmin = 12000;
                    if(*dzdr >= 12000)
                    {
                        if(*dzdr > 34000)
                        {
                            *dzdr = 34000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DRCmin = 17000;
                    if(*dzdr >= 17000)
                    {
                        if(*dzdr > 41000)
                        {
                            *dzdr = 41000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DRCmin = 35000;
                    if(*dzdr >= 35000)
                    {
                        if(*dzdr > 44500)
                        {
                            *dzdr = 45000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 950)
            {
                if(Vuelo <= 1000)
                {DRCmin = 15000;
                    if(*dzdr >= 15000)
                    {
                        if(*dzdr > 21500)
                        {
                            *dzdr = 21500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {DRCmin = 16000;
                    if(*dzdr >= 16000)
                    {
                        if(*dzdr > 25000)
                        {
                            *dzdr = 25000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {DRCmin = 16000;
                    if(*dzdr >= 16000)
                    {
                        if(*dzdr > 27000)
                        {
                            *dzdr = 27000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {DRCmin = 17000;
                    if(*dzdr >= 17000)
                    {
                        if(*dzdr > 28000)
                        {
                            *dzdr = 28000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {DRCmin = 18000;
                    if(*dzdr >= 18000)
                    {
                        if(*dzdr > 31000)
                        {
                            *dzdr = 31000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {DRCmin = 22000;
                    if(*dzdr >= 22000)
                    {
                        if(*dzdr > 34600)
                        {
                            *dzdr = 34600;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 30000)
                {DRCmin = 31500;
                    if(*dzdr >= 31500)
                    {
                        if(*dzdr > 41000)
                        {
                            *dzdr = 41000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
        }
        else
        {
            if(velocidad == 150)
            {
                if(Vuelo <= 1000)
                {
                    if(*dzdr >= 8700)
                    {
                        if(*dzdr > 24000)
                        {
                            *dzdr = 24000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {
                    if(*dzdr >= 8600)
                    {
                        if(*dzdr > 26000)
                        {
                            *dzdr = 26000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {
                    if(*dzdr >= 8600)
                    {
                        if(*dzdr > 27000)
                        {
                            *dzdr = 27000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {
                    if(*dzdr >= 8600)
                    {
                        if(*dzdr > 29000)
                        {
                            *dzdr = 29000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {
                    if(*dzdr >= 10000)
                    {
                        if(*dzdr > 34000)
                        {
                            *dzdr = 34000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {
                    if(*dzdr >= 13000)
                    {
                        if(*dzdr > 40000)
                        {
                            *dzdr = 40000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                //                if(Vuelo == 30000)
                //                {DRCmin = 23682;
                //                    if(*dzdr >= 23682)
                //                    {
                //                        if(*dzdr > 38571)
                //                        {
                //                            *dzdr = 38571;
                //                        }
                //                        return true;
                //                    }


                //                    *dzdr = 0;
                //                    return false;

                //                }
            }
            if(velocidad == 300)
            {
                if(Vuelo <= 1000)
                {
                    if(*dzdr >= 10000)
                    {
                        if(*dzdr > 24000)
                        {
                            *dzdr = 24000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {
                    if(*dzdr >= 10500)
                    {
                        if(*dzdr > 26000)
                        {
                            *dzdr = 26000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {
                    if(*dzdr >= 11500)
                    {
                        if(*dzdr > 27000)
                        {
                            *dzdr = 27000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {
                    if(*dzdr >= 12000)
                    {
                        if(*dzdr > 29000)
                        {
                            *dzdr = 29000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {
                    if(*dzdr >= 16000)
                    {
                        if(*dzdr > 34000)
                        {
                            *dzdr = 34000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {
                    if(*dzdr >= 20000)
                    {
                        if(*dzdr > 40000)
                        {
                            *dzdr = 40000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 420)
            {
                if(Vuelo <= 1000)
                {
                    if(*dzdr >= 12000)
                    {
                        if(*dzdr > 24000)
                        {
                            *dzdr = 24000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                if(Vuelo <= 2000)
                {
                    if(*dzdr >= 12000)
                    {
                        if(*dzdr > 26000)
                        {
                            *dzdr = 26000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 4000)
                {
                    if(*dzdr >= 12500)
                    {
                        if(*dzdr > 27000)
                        {
                            *dzdr = 27000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 6000)
                {
                    if(*dzdr >= 13000)
                    {
                        if(*dzdr > 29000)
                        {
                            *dzdr = 29000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 10000)
                {
                    if(*dzdr >= 17000)
                    {
                        if(*dzdr > 34000)
                        {
                            *dzdr = 34000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                if(Vuelo == 17000)
                {
                    if(*dzdr >= 24000)
                    {
                        if(*dzdr > 40000)
                        {
                            *dzdr = 40000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
        }
    }
    if(calculos->medio == 38 )
    {
        if(!alcance)
        {
            if(velocidad == 150)
            {
                if(Vuelo <= 400)
                {DRCmin = 3500;
                    if(*dzdr >= 3500)
                    {
                        if(*dzdr > 11400)
                        {
                            *dzdr = 11400;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo <= 1000)
                {DRCmin = 3500;
                    if(*dzdr >= 3500)
                    {
                        if(*dzdr > 17300)
                        {
                            *dzdr = 17300;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else  if(Vuelo == 2000)
                {DRCmin = 4000;
                    if(*dzdr >= 4000)
                    {
                        if(*dzdr > 17225)
                        {
                            *dzdr = 17225;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 4000)
                {DRCmin = 4000;
                    if(*dzdr >= 4000)
                    {
                        if(*dzdr > 18040)
                        {
                            *dzdr = 18040;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 6000)
                {DRCmin = 5500;
                    if(*dzdr >= 5500)
                    {
                        if(*dzdr > 17900)
                        {
                            *dzdr = 17900;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 10000)
                {DRCmin = 6300;
                    if(*dzdr >= 6300)
                    {
                        if(*dzdr > 18720)
                        {
                            *dzdr = 18720;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else/*17000*/
                {DRCmin = 7900;
                    if(*dzdr >= 7900)
                    {
                        if(*dzdr > 23900)
                        {
                            *dzdr = 23900;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 300)
            {
                if(Vuelo <= 400)
                {DRCmin = 3500;
                    if(*dzdr >= 3500)
                    {
                        if(*dzdr > 11500)
                        {
                            *dzdr = 11500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo <= 1000)
                {DRCmin = 3900;
                    if(*dzdr >= 3900)
                    {
                        if(*dzdr > 17200)
                        {
                            *dzdr = 17200;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else  if(Vuelo == 2000)
                {DRCmin = 4500;
                    if(*dzdr >= 4500)
                    {
                        if(*dzdr > 17310)
                        {
                            *dzdr = 17310;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 4000)
                {DRCmin = 5000;
                    if(*dzdr >= 5000)
                    {
                        if(*dzdr > 17340)
                        {
                            *dzdr = 17340;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 6000)
                {DRCmin = 5800;
                    if(*dzdr >= 5800)
                    {
                        if(*dzdr > 18035)
                        {
                            *dzdr = 18035;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 10000)
                {DRCmin = 6500;
                    if(*dzdr >= 6500)
                    {
                        if(*dzdr > 19500)
                        {
                            *dzdr = 19500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else/*17000*/
                {DRCmin = 7900;
                    if(*dzdr >= 7900)
                    {
                        if(*dzdr > 24800)
                        {
                            *dzdr = 24800;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 420)
            {
                if(Vuelo <= 400)
                {DRCmin = 3500;
                    if(*dzdr >= 3500)
                    {
                        if(*dzdr > 11500)
                        {
                            *dzdr = 11500;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo <= 1000)
                {DRCmin = 3500;
                    if(*dzdr >= 3500)
                    {
                        if(*dzdr > 16300)
                        {
                            *dzdr = 16300;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else  if(Vuelo == 2000)
                {DRCmin = 3900;
                    if(*dzdr >= 3900)
                    {
                        if(*dzdr > 17200)
                        {
                            *dzdr = 17200;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 4000)
                {DRCmin = 4400;
                    if(*dzdr >= 4400)
                    {
                        if(*dzdr > 18000)
                        {
                            *dzdr = 18000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 6000)
                {DRCmin = 4900;
                    if(*dzdr >= 4900)
                    {
                        if(*dzdr > 18200)
                        {
                            *dzdr = 18200;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 10000)
                {DRCmin = 5500;
                    if(*dzdr >= 5500)
                    {
                        if(*dzdr > 18800)
                        {
                            *dzdr = 18800;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else/*17000*/
                {DRCmin = 0;
                    //                if(*dzdr >= 17326)
                    //                {
                    //                    if(*dzdr > 45000)
                    //                    {
                    //                        *dzdr = 45000;
                    //                    }
                    //                    return true;
                    //                }
                    //                else
                    //                {
                    *dzdr = 0;
                    return false;
                    //                }
                }
            }
            if(velocidad == 640)
            {
                if(Vuelo <= 400)
                { DRCmin = 3500;
                    if(*dzdr >= 3500)
                    {
                        if(*dzdr > 10483)
                        {
                            *dzdr = 10483;
                        }
                        return true;
                    }
                    else
                    {
                        *dzdr = 0;
                        return false;
                    }

                }
                if(Vuelo <= 1000)
                {DRCmin = 3600;
                    if(*dzdr >= 3600)
                    {
                        if(*dzdr > 17600)
                        {
                            *dzdr = 17600;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else  if(Vuelo == 2000)
                {DRCmin = 4000;
                    if(*dzdr >= 4000)
                    {
                        if(*dzdr > 16800)
                        {
                            *dzdr = 16800;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 4000)
                {DRCmin = 3900;
                    if(*dzdr >= 3900)
                    {
                        if(*dzdr > 17700)
                        {
                            *dzdr = 17700;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 6000)
                {DRCmin = 3900;
                    if(*dzdr >= 3900)
                    {
                        if(*dzdr > 17800)
                        {
                            *dzdr = 17800;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 10000)
                {DRCmin = 4300;
                    if(*dzdr >= 4300)
                    {
                        if(*dzdr > 18600)
                        {
                            *dzdr = 18600;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else/*17000*/
                {
                    *dzdr = 0;
                    return false;
                }
            }
        }
        else
        {
            if(velocidad == 150)
            {
                if(Vuelo == 500)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 16000)
                        {
                            *dzdr = 16000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo <= 1000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 16000)
                        {
                            *dzdr = 16000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else  if(Vuelo == 2000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 18000)
                        {
                            *dzdr = 18000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 4000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 19000)
                        {
                            *dzdr = 19000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 6000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 22000)
                        {
                            *dzdr = 22000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 10000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 22000)
                        {
                            *dzdr = 22000;
                        }
                        return true;
                    }


                    *dzdr = 0;
                    return false;

                }
            }
            if(velocidad == 300)
            {
                if(Vuelo == 500)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 17000)
                        {
                            *dzdr = 17000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo <= 1000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 17000)
                        {
                            *dzdr = 17000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else  if(Vuelo == 2000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 18000)
                        {
                            *dzdr = 18000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 4000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 19000)
                        {
                            *dzdr = 19000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 6000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 22000)
                        {
                            *dzdr = 22000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
                else if(Vuelo == 10000)
                {
                    if(*dzdr >= 6000)
                    {
                        if(*dzdr > 22000)
                        {
                            *dzdr = 22000;
                        }
                        return true;
                    }

                    *dzdr = 0;
                    return false;

                }
            }
        }
    }

    if(calculos->medio == 37 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 400)
            {DRCmin = 3650;
                if(*dzdr >= 3650)
                {
                    if(*dzdr > 17050)
                    {
                        *dzdr = 17050;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 3650;
                if(*dzdr >= 3650)
                {
                    if(*dzdr > 17900)
                    {
                        *dzdr = 17900;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 4400;
                if(*dzdr >= 4400)
                {
                    if(*dzdr > 23700)
                    {
                        *dzdr = 23700;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 4000)
            {DRCmin = 5100;
                if(*dzdr >= 5100)
                {
                    if(*dzdr > 24550)
                    {
                        *dzdr = 24550;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 6000)
            {DRCmin = 5800;
                if(*dzdr >= 5800)
                {
                    if(*dzdr > 25600)
                    {
                        *dzdr = 25600;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 10000)
            {DRCmin = 6600;
                if(*dzdr >= 6600)
                {
                    if(*dzdr > 25600)
                    {
                        *dzdr = 25600;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else/*17000*/
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 400)
            {DRCmin = 3000;
                if(*dzdr >= 3000)
                {
                    if(*dzdr > 14000)
                    {
                        *dzdr = 14000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 3000;
                if(*dzdr >= 3000)
                {
                    if(*dzdr > 15000)
                    {
                        *dzdr = 15000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 3500;
                if(*dzdr >= 3500)
                {
                    if(*dzdr > 20400)
                    {
                        *dzdr = 20400;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 4000)
            {DRCmin = 4000;
                if(*dzdr >= 4000)
                {
                    if(*dzdr > 20400)
                    {
                        *dzdr = 20400;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 6000)
            {DRCmin = 4600;
                if(*dzdr >= 4600)
                {
                    if(*dzdr > 21350)
                    {
                        *dzdr = 21350;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 10000)
            {DRCmin = 5300;
                if(*dzdr >= 5300)
                {
                    if(*dzdr > 21450)
                    {
                        *dzdr = 21450;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else/*17000*/
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 400)
            {DRCmin = 3100;
                if(*dzdr >= 3100)
                {
                    if(*dzdr > 14650)
                    {
                        *dzdr = 14650;
                    }
                    return true;
                }

                *dzdr = 0;
                return false;
            }
            else if(Vuelo <= 1000)
            {DRCmin = 3100;
                if(*dzdr >= 3100)
                {
                    if(*dzdr > 16550)
                    {
                        *dzdr = 16550;
                    }
                    return true;
                }

                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 3600;
                if(*dzdr >= 3600)
                {
                    if(*dzdr > 20000)
                    {
                        *dzdr = 20000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 4000)
            {DRCmin = 4100;
                if(*dzdr >= 4100)
                {
                    if(*dzdr > 20800)
                    {
                        *dzdr = 20800;
                    }
                    return true;
                }

                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 6000)
            {DRCmin = 4500;
                if(*dzdr >= 4500)
                {
                    if(*dzdr > 21600)
                    {
                        *dzdr = 21600;
                    }
                    return true;
                }

                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 10000)
            {DRCmin = 5000;
                if(*dzdr >= 5000)
                {
                    if(*dzdr > 22300)
                    {
                        *dzdr = 22300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else/*17000*/
            {
                DRCmin = 0;
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 640)
        {
            if(Vuelo <= 400)
            { DRCmin = 3400;
                if(*dzdr >= 3400)
                {
                    if(*dzdr > 14450)
                    {
                        *dzdr = 14450;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;


            }
            else if(Vuelo <= 1000)
            {DRCmin = 3400;
                if(*dzdr >= 3400)
                {
                    if(*dzdr > 15600)
                    {
                        *dzdr = 15600;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 4100;
                if(*dzdr >= 4100)
                {
                    if(*dzdr > 17500)
                    {
                        *dzdr = 17500;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 4000)
            {DRCmin = 4600;
                if(*dzdr >= 4600)
                {
                    if(*dzdr > 18600)
                    {
                        *dzdr = 18600;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 6000)
            {
                DRCmin = 4050;
                if(*dzdr >= 4050)
                {
                    if(*dzdr > 18950)
                    {
                        *dzdr = 18950;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 10000)
            {
                DRCmin = 4500;
                if(*dzdr >= 4500)
                {
                    if(*dzdr > 18950)
                    {
                        *dzdr = 18950;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else/*17000*/
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 36 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 400)
            {DRCmin = 2100;
                if(*dzdr >= 2100)
                {
                    if(*dzdr > 8700)
                    {
                        *dzdr = 8700;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;
            }
            else if(Vuelo <= 1000)
            {
                DRCmin = 2100;
                if(*dzdr >= 2100)
                {
                    if(*dzdr > 8700)
                    {
                        *dzdr = 8700;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {
                DRCmin = 2000;
                if(*dzdr >= 2000)
                {
                    if(*dzdr > 10200)
                    {
                        *dzdr = 10200;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 4000)
            {
                DRCmin = 2000;
                if(*dzdr >= 2000)
                {
                    if(*dzdr > 10800)
                    {
                        *dzdr = 10800;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else/*17000*/
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 400)
            {DRCmin = 1600;
                if(*dzdr >= 1600)
                {
                    if(*dzdr > 7100)
                    {
                        *dzdr = 7100;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 1600;
                if(*dzdr >= 1600)
                {
                    if(*dzdr > 7100)
                    {
                        *dzdr = 7100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 2000;
                if(*dzdr >= 2000)
                {
                    if(*dzdr > 10000)
                    {
                        *dzdr = 10000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 4000)
            {DRCmin = 2500;
                if(*dzdr >= 2500)
                {
                    if(*dzdr > 10300)
                    {
                        *dzdr = 10300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }

            else/*17000*/
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 400)
            {
                DRCmin = 1800;
                if(*dzdr >= 1800)
                {
                    if(*dzdr > 7800)
                    {
                        *dzdr = 7800;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {
                DRCmin = 1800;
                if(*dzdr >= 1800)
                {
                    if(*dzdr > 7800)
                    {
                        *dzdr = 7800;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {
                DRCmin = 1700;
                if(*dzdr >= 1700)
                {
                    if(*dzdr > 10700)
                    {
                        *dzdr = 10700;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 4000)
            {DRCmin = 2000;
                if(*dzdr >= 2000)
                {
                    if(*dzdr > 11100)
                    {
                        *dzdr = 11100;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;

            }

            else/*17000*/
            {
                *dzdr = 0;
                return false;
            }
        }
    }

    if(calculos->medio == 35 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 300)
            {
                DRCmin = 800;
                if(*dzdr >= 800)
                {
                    if(*dzdr > 5200)
                    {
                        *dzdr = 5200;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;
            }
            else if(Vuelo <= 1000)
            {
                DRCmin = 800;
                if(*dzdr >= 800)
                {
                    if(*dzdr > 5070)
                    {
                        *dzdr = 5070;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 800;
                if(*dzdr >= 800)
                {
                    if(*dzdr > 5000)
                    {
                        *dzdr = 5000;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;
            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 300)
            {
                DRCmin = 800;
                if(*dzdr >= 800)
                {
                    if(*dzdr > 5040)
                    {
                        *dzdr = 5040;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;
            }
            else if(Vuelo <= 2000)
            {
                DRCmin = 800;
                if(*dzdr >= 800)
                {
                    if(*dzdr > 5000)
                    {
                        *dzdr = 5000;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;
            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 1000)
            {
                DRCmin = 800;
                if(*dzdr >= 800)
                {
                    if(*dzdr > 5000)
                    {
                        *dzdr = 5000;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;
            }
            else  if(Vuelo == 2000)
            {
                DRCmin = 800;
                if(*dzdr >= 800)
                {
                    if(*dzdr > 5100)
                    {
                        *dzdr = 5100;
                    }
                    return true;
                }
                *dzdr = 0;
                return false;
            }

            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 34 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 300)
            {DRCmin = 10;
                if(*dzdr >= 10)
                {
                    if(*dzdr > 3100)
                    {
                        *dzdr = 3100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 20;
                if(*dzdr >= 20)
                {
                    if(*dzdr > 3100)
                    {
                        *dzdr = 3100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 40;
                if(*dzdr >= 40)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }

            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 300)
            {DRCmin = 10;
                if(*dzdr >= 10)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 18;
                if(*dzdr >= 18)
                {
                    if(*dzdr > 3120)
                    {
                        *dzdr = 3120;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 38;
                if(*dzdr >= 38)
                {
                    if(*dzdr > 3020)
                    {
                        *dzdr = 3020;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 300)
            {DRCmin = 10;
                if(*dzdr >= 10)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 15;
                if(*dzdr >= 15)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 40;
                if(*dzdr >= 40)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 33 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 2500)
                    {
                        *dzdr = 2500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 1000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 2600)
                    {
                        *dzdr = 2600;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo == 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 2510)
                    {
                        *dzdr = 2510;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 2500)
                    {
                        *dzdr = 2500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 32 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 2060)
                    {
                        *dzdr = 2060;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 2100)
                    {
                        *dzdr = 2100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 2000)
                    {
                        *dzdr = 2000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 31 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 1000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 4150)
                    {
                        *dzdr = 4150;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 4100)
                    {
                        *dzdr = 4100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 4000)
                    {
                        *dzdr = 4000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 300)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 4050)
                    {
                        *dzdr = 4050;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 4100)
                    {
                        *dzdr = 4100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo == 2000)
            {DRCmin = 500;
                if(*dzdr >= 500)
                {
                    if(*dzdr > 4200)
                    {
                        *dzdr = 4200;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }

    if(calculos->medio == 27 || calculos->medio == 42)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {DRCmin = 10;
                if(*dzdr >= 10)
                {
                    if(*dzdr > 12100)
                    {
                        *dzdr = 12100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 12000)
                    {
                        *dzdr = 12000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 11950)
                    {
                        *dzdr = 11950;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin =200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 11850)
                    {
                        *dzdr = 11850;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 4000)
            {DRCmin = 340;
                if(*dzdr >= 340)
                {
                    if(*dzdr > 11100)
                    {
                        *dzdr = 11100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {DRCmin = 680;
                if(*dzdr >= 680)
                {
                    if(*dzdr > 10400)
                    {
                        *dzdr = 10400;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 10000)
            {DRCmin = 1300;
                if(*dzdr >= 1300)
                {
                    if(*dzdr > 6650)
                    {
                        *dzdr = 6650;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {DRCmin = 10;
                if(*dzdr >= 10)
                {
                    if(*dzdr > 12100)
                    {
                        *dzdr = 12100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 12000)
                    {
                        *dzdr = 12000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 11950)
                    {
                        *dzdr = 11950;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 11850)
                    {
                        *dzdr = 11850;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {DRCmin = 340;
                if(*dzdr >= 340)
                {
                    if(*dzdr > 11000)
                    {
                        *dzdr = 11000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {DRCmin = 680;
                if(*dzdr >= 680)
                {
                    if(*dzdr > 10400)
                    {
                        *dzdr = 10400;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 10000)
            {DRCmin = 1300;
                if(*dzdr >= 1300)
                {
                    if(*dzdr > 6650)
                    {
                        *dzdr = 6650;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {DRCmin = 10;
                if(*dzdr >= 10)
                {
                    if(*dzdr > 12100)
                    {
                        *dzdr = 12100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 12000)
                    {
                        *dzdr = 12000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 11950)
                    {
                        *dzdr = 11950;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 11850)
                    {
                        *dzdr = 11850;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 4000)
            {DRCmin = 340;
                if(*dzdr >= 340)
                {
                    if(*dzdr > 11000)
                    {
                        *dzdr = 11000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {DRCmin = 680;
                if(*dzdr >= 680)
                {
                    if(*dzdr > 10400)
                    {
                        *dzdr = 10400;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 10000)
            {DRCmin = 1300;
                if(*dzdr >= 1300)
                {
                    if(*dzdr > 6650)
                    {
                        *dzdr = 6650;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 26 || calculos->medio == 43)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 6270)
                    {
                        *dzdr = 6270;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 6100)
                    {
                        *dzdr = 6100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 5990)
                    {
                        *dzdr = 5990;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 5870)
                    {
                        *dzdr = 5870;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 4000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 4740)
                    {
                        *dzdr = 4740;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 6000)
            {DRCmin = 340;
                if(*dzdr >= 340)
                {
                    if(*dzdr > 2250)
                    {
                        *dzdr = 2250;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <=100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 6200)
                    {
                        *dzdr = 6200;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 6100)
                    {
                        *dzdr = 6100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 5990)
                    {
                        *dzdr = 5990;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 5870)
                    {
                        *dzdr = 5870;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 4740)
                    {
                        *dzdr = 4740;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 6000)
            {DRCmin = 340;
                if(*dzdr >= 340)
                {
                    if(*dzdr > 2250)
                    {
                        *dzdr = 2250;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 6270)
                    {
                        *dzdr = 6270;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 6100)
                    {
                        *dzdr = 6100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 5990)
                    {
                        *dzdr = 5990;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 5890)
                    {
                        *dzdr = 5890;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 4000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 4740)
                    {
                        *dzdr = 4740;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 6000)
            {DRCmin = 340;
                if(*dzdr >= 340)
                {
                    if(*dzdr > 2250)
                    {
                        *dzdr = 2250;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }

    if(calculos->medio == 25 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 4100)
                    {
                        *dzdr = 4100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 3970)
                    {
                        *dzdr = 3970;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 3880)
                    {
                        *dzdr = 3880;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin =100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 3470)
                    {
                        *dzdr = 3470;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 4000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 1480)
                    {
                        *dzdr = 1480;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 4000)
                    {
                        *dzdr = 4000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 3970)
                    {
                        *dzdr = 3970;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 3880)
                    {
                        *dzdr = 3880;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 3470)
                    {
                        *dzdr = 3470;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 4000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 1400)
                    {
                        *dzdr = 1400;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 4100)
                    {
                        *dzdr = 4100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 3970)
                    {
                        *dzdr = 3970;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 3900)
                    {
                        *dzdr = 3900;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 3470)
                    {
                        *dzdr = 3470;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 4000)
            {DRCmin = 200;
                if(*dzdr >= 200)
                {
                    if(*dzdr > 1480)
                    {
                        *dzdr = 1480;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 24 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2960)
                    {
                        *dzdr = 2960;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2860)
                    {
                        *dzdr = 2860;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 2250)
                    {
                        *dzdr = 2250;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <=100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2960)
                    {
                        *dzdr = 2960;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2860)
                    {
                        *dzdr = 2860;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 2250)
                    {
                        *dzdr = 2250;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 3000)
                    {
                        *dzdr = 3000;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2960)
                    {
                        *dzdr = 2960;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2880)
                    {
                        *dzdr = 2880;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 2250)
                    {
                        *dzdr = 2250;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 23 || calculos->medio == 44)
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 2500)
                    {
                        *dzdr = 2500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2480)
                    {
                        *dzdr = 2480;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2300)
                    {
                        *dzdr = 2300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 1520)
                    {
                        *dzdr = 1520;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;

            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 2500)
                    {
                        *dzdr = 2500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2465)
                    {
                        *dzdr = 2465;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2300)
                    {
                        *dzdr = 2300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 1550)
                    {
                        *dzdr = 1550;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 2530)
                    {
                        *dzdr = 2530;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2520)
                    {
                        *dzdr = 2520;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2300)
                    {
                        *dzdr = 2300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 1570)
                    {
                        *dzdr = 1570;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 22 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 2500)
                    {
                        *dzdr = 2500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2450)
                    {
                        *dzdr = 2450;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2300)
                    {
                        *dzdr = 2300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 1520)
                    {
                        *dzdr = 1520;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 2500)
                    {
                        *dzdr = 2500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2450)
                    {
                        *dzdr = 2450;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2300)
                    {
                        *dzdr = 2300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 1520)
                    {
                        *dzdr = 1520;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 2500)
                    {
                        *dzdr = 2500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 2450)
                    {
                        *dzdr = 2450;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 2300)
                    {
                        *dzdr = 2300;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 2000)
            {DRCmin = 100;
                if(*dzdr >= 100)
                {
                    if(*dzdr > 1525)
                    {
                        *dzdr = 1525;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
    if(calculos->medio == 21 )
    {
        if(velocidad == 150)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 1500)
                    {
                        *dzdr = 1500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 1350)
                    {
                        *dzdr = 1350;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 1100)
                    {
                        *dzdr = 1100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 300)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 1500)
                    {
                        *dzdr = 1500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 1350)
                    {
                        *dzdr = 1350;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 1100)
                    {
                        *dzdr = 1100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
        if(velocidad == 420)
        {
            if(Vuelo <= 100)
            {DRCmin = 5;
                if(*dzdr >= 5)
                {
                    if(*dzdr > 1500)
                    {
                        *dzdr = 1500;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 500)
            {DRCmin = 25;
                if(*dzdr >= 25)
                {
                    if(*dzdr > 1350)
                    {
                        *dzdr = 1350;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else  if(Vuelo <= 1000)
            {DRCmin = 50;
                if(*dzdr >= 50)
                {
                    if(*dzdr > 1100)
                    {
                        *dzdr = 1100;
                    }
                    return true;
                }


                *dzdr = 0;
                return false;

            }
            else
            {
                *dzdr = 0;
                return false;
            }
        }
    }
}

bool distanThread::comprobarDlanzaSegmentosNV(double *dl,int a)
{
    double ini = 0;
    double fin = 0;
    int disrestar=0;

    if(!alcance)
    {
        if(calculos->medio == 39)
            switch (velocidad)
            {
            case 150: disrestar = 300; break;
            case 300: disrestar = 600; break;
            case 420: disrestar = 840; break;
            case 640: disrestar = 1280; break;
            case 950: disrestar = 1900; break;
            }
        if(calculos->medio == 38 || calculos->medio == 37)
            switch (velocidad)
            {
            case 150: disrestar = 300; break;
            case 300: disrestar = 600; break;
            case 420: disrestar = 840; break;
            case 640: disrestar = 1280; break;
            }
        if(calculos->medio <= 36 || calculos->medio == 44 || calculos->medio == 43 || calculos->medio == 42)
            switch (velocidad)
            {
            case 150: disrestar = 300; break;
            case 300: disrestar = 600; break;
            case 420: disrestar = 840; break;
            }

        for (int i = 0; i < disInternas[a].length (); ++i)
        {
            if((i)%2 == 0)
            {
                ini = disInternas[a].at (i);
                fin = disInternas[a].at (i+1);
                if(ini <= *dl && *dl <=fin)
                {
                    *dl = ini-disrestar;
                    i-=1;
                }
                else
                {
                    if(*dl < ini )
                    {
                        if((ini - (*dl)) < disrestar)
                        {
                            *dl = *dl-(ini - (*dl));
                            i -= 1;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(calculos->medio == 39)
            switch (velocidad)
            {
            case 150: disrestar = 150; break;
            case 300: disrestar = 300; break;
            case 420: disrestar = 420; break;

            }
        if(calculos->medio == 38 || calculos->medio == 37)
            switch (velocidad)
            {
            case 150: disrestar = 300; break;
            case 300: disrestar = 600; break;
            case 420: disrestar = 840; break;
            case 640: disrestar = 1280; break;
            }
        if(calculos->medio <= 36 || calculos->medio == 44 || calculos->medio == 43 || calculos->medio == 42)
            switch (velocidad)
            {
            case 150: disrestar = 300; break;
            case 300: disrestar = 600; break;
            case 420: disrestar = 840; break;
            }

        for (int i = 0; i < disInternas[a].length (); ++i)
        {
            if((i)%2 == 0)
            {
                ini = disInternas[a].at (i);
                fin = disInternas[a].at (i+1);
                if(ini <= *dl && *dl <=fin)//esta en el segmento
                {
                    *dl = ini - disrestar;
                    i=-1;
                }
                else
                {
                    //me quedo asi
                }
            }
        }
    }
    return true;
}

bool distanThread::comprobarDlanzaSegmentosNVzm(double *dl, int a)
{
    double ini = 0;
    double fin = 0;
    if(ZonaMuertaMuis2[a].length ()>1)
    {
        for (int i = 0; i < ZonaMuertaMuis2[a].length (); ++i)
        {
            if((i)%2 == 0)
            {
                ini = ZonaMuertaMuis2[a].at (i);
                fin = ZonaMuertaMuis2[a].at (i+1);
                if(ini <= *dl && *dl <=fin)
                {
                    *dl = 0;
                    i=-1;
                }
            }
        }
    }
    return true;
}

bool distanThread::comprobarDlanzaDzr(double *dl, double dr, int a)
{
    bool cumple = false;
    double ini = 0;
    double fin = 0;
    double s = 0;
    int disrestar=0;
    switch (velocidad)
    {
    case 150: disrestar = 1500; break;
    case 300: disrestar = 3000; break;
    case 420: disrestar = 4200; break;
    case 640: disrestar = 6400; break;
    case 950: disrestar = 9500; break;
    }
    for (int i = 0; i < disInternas[a].length (); ++i)
    {
        if((i)%2 == 0)
        {
            ini = disInternas[a].at (i);
            fin = disInternas[a].at (i+1);
            s =  fin - ini;
            if(dr<=ini && fin<=*dl )
            {
                if(s >= disrestar)
                {
                    *dl = ini;
                    cumple = true;
                    break;
                }
            }
        }
    }
    return cumple;
}

void distanThread::calcularDlDzr(double *dl, double *dr, int a)
{

    comprobarDlanza (dl,disVDirecta[a].at (0));
    comprobarDlanzaSegmentosNV(dl,a);
    comprobarDlanza (dl,disVDirecta[a].at (0));
    comprobarDlanzaSegmentosNVzm(dl,a);

    velocidadCohete = velocidadCohetes (Vuelo,*dl);
    *dr = *dl * velocidadCohete/(velocidad  + velocidadCohete);
    //    qDebug()<< "calcularDlDzr   " << *dl<<"    "<< *dr;

    comprobarDZDR(dr);
    comprobarDlanzaSegmentosNV(dr,a);
    comprobarDZDR(dr);
    comprobarDlanzaSegmentosNVzm(dr,a);

    if( comprobarDlanzaDzr(dl,*dr,a))
    {
        calcularDlDzr(dl,dr,a);
    }
}

int distanThread::velocidadCohetes(int hb, double dlanz)
{
    int r = 0;
    int c = 0;
    if(calculos->medio <= 27 || (calculos->medio <= 44 && calculos->medio >= 42))
    {
        if(hb<=100)
            hb = 100;
        else if (hb<=500)
            hb = 500;
        else if (hb<=1000)
            hb = 1000;
    }
    else if(calculos->medio <= 35)
    {
        if(hb<=300)
            hb = 300;
        else if (hb<=1000)
            hb = 1000;
    }
    else if(calculos->medio <= 38)
    {
        if(hb<=400)
            hb = 400;
        else if (hb<=1000)
            hb = 1000;
    }
    else
    {
        if(hb<=1000)
            hb = 1000;
    }

    int ant = 0;
    int pos = 0;

    for (int i = 0; i <= 74; ++i)
    {
        if(tablaVelocidad[i][0] >= dlanz)
        {
            ant = tablaVelocidad[i-1][0];
            pos = tablaVelocidad[i][0];
            if( ( dlanz-ant) < (pos-dlanz) )
            {
                r = i-1;
                dlanz = ant;
            }
            else
            {
                r = i;
                dlanz = pos;
            }
            break;
        }
    }
    if(tablaVelocidad[r][0] == dlanz)
    {
        for (int j = 1; j < 8; ++j)
        {
            if(tablaVelocidad[0][j]  == hb)
            {
                c = j;
            }
        }
    }
    return tablaVelocidad[r][c];
}

void distanThread::leerVelocidad()
{
    QStringList todo;
    QString name;
    int ini = 0;
    int fin = 0;

    if(calculos->medio == 39 && calculos->grupo == 1)
    {
        if(!alcance)
        {
            name = "/m39g1.txt";
            switch (velocidad) {
            case 300: ini = 1;fin = 75; break;
            case 150: ini = 77;fin = 151; break;
            case 420: ini = 153;fin = 227; break;
            case 640: ini = 229;fin = 303; break;
            case 950: ini = 305;fin = 379; break;
                break;
            }
        }
        else
        {
            name = "/m39a.txt";
            switch (velocidad) {
            case 300: ini = 1;fin = 75; break;
            case 150: ini = 77;fin = 151; break;
            case 420: ini = 153;fin = 227; break;
                break;
            }
        }
    }
    if(calculos->medio == 38 && calculos->grupo == 1)
    {
        if(!alcance)
        {
            name = "/m38g1.txt";
            switch (velocidad) {
            case 300: ini = 1;fin = 75; break;
            case 150: ini = 77;fin = 151; break;
            case 420: ini = 153;fin = 227; break;
            case 640: ini = 229;fin = 303; break;
                break;
            }
        }
        else
        {
            name = "/m38a.txt";
            switch (velocidad) {
            case 300: ini = 1;fin = 23; break;
            case 150: ini = 26;fin = 48; break;

                break;
            }
        }
    }
    if(calculos->medio == 37 && calculos->grupo == 1)
    {
        name = "/m37g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 75; break;
        case 150: ini = 77;fin = 151; break;
        case 420: ini = 153;fin = 227; break;
        case 640: ini = 229;fin = 303; break;
            break;
        }
    }
    if(calculos->medio == 36 && calculos->grupo == 1)
    {
        name = "/m36g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 16; break;
        case 150: ini = 18;fin = 33; break;
        case 420: ini = 35;fin = 50; break;
            //        case 640: ini = 229;fin = 303; break;
            break;
        }
    }

    if(calculos->medio == 35 && calculos->grupo == 1)
    {
        name = "/m35g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 16; break;
        case 150: ini = 18;fin = 33; break;
        case 420: ini = 35;fin = 50; break;

            break;
        }
    }
    if(calculos->medio == 34 && calculos->grupo == 1)
    {
        name = "/m34g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 18; break;
        case 150: ini = 20;fin = 37; break;
        case 420: ini = 39;fin = 56; break;

            break;
        }
    }
    if(calculos->medio == 33 && calculos->grupo == 1)
    {
        name = "/m33g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 11; break;
        case 150: ini = 13;fin = 23; break;
        case 420: ini = 25;fin = 35; break;

            break;
        }
    }
    if(calculos->medio == 32 && calculos->grupo == 1)
    {
        name = "/m32g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 9; break;
        case 150: ini = 11;fin = 19; break;
        case 420: ini = 21;fin = 29; break;

            break;
        }
    }
    if(calculos->medio == 31 && calculos->grupo == 1)
    {
        name = "/m31g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 15; break;
        case 150: ini = 17;fin = 31; break;
        case 420: ini = 33;fin = 47; break;

            break;
        }
    }

    if((calculos->medio == 27 || calculos->medio == 42)&& calculos->grupo == 1)
    {
        name = "/m27g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 44; break;
        case 150: ini = 46;fin = 89; break;
        case 420: ini = 91;fin = 134; break;

            break;
        }
    }
    if((calculos->medio == 26 || calculos->medio == 43)&& calculos->grupo == 1)
    {
        name = "/m26g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 28; break;
        case 150: ini = 30;fin = 57; break;
        case 420: ini = 59;fin = 86; break;

            break;
        }
    }

    if(calculos->medio == 25 && calculos->grupo == 1)
    {
        name = "/m25g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 22; break;
        case 150: ini = 24;fin = 45; break;
        case 420: ini = 47;fin = 68; break;

            break;
        }
    }
    if(calculos->medio == 24 && calculos->grupo == 1)
    {
        name = "/m24g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 18; break;
        case 150: ini = 20;fin = 37; break;
        case 420: ini = 39;fin = 56; break;

            break;
        }
    }

    if((calculos->medio == 23 || calculos->medio == 44) && calculos->grupo == 1)
    {
        name = "/m23g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 17; break;
        case 150: ini = 19;fin = 35; break;
        case 420: ini = 37;fin = 53; break;

            break;
        }
    }
    if(calculos->medio == 22 && calculos->grupo == 1)
    {
        name = "/m22g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 17; break;
        case 150: ini = 19;fin = 35; break;
        case 420: ini = 37;fin = 53; break;

            break;
        }
    }
    if(calculos->medio == 21 && calculos->grupo == 1)
    {
        name = "/m21g1.txt";
        switch (velocidad) {
        case 300: ini = 1;fin = 14; break;
        case 150: ini = 16;fin = 29; break;
        case 420: ini = 31;fin = 44; break;

            break;
        }
    }
    QString fileName = QDir::currentPath() + name;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug()<<"no abre fichero velocidad";
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        todo.append(line);
    }
    file.close ();

    QStringList aux;

    int c = 0;
    for (int i = ini; i <= fin; ++i)
    {
        aux = todo.at (i).split(",");
        for (int j = 0; j < aux.length ()-1; ++j)
        {
            const QString& a = aux.at (j).trimmed();

            tablaVelocidad[c][j] = a.toInt();
        }
        c++;
    }
}

//bool distanThread::creaTablaBaseDatos(const QString &tablaName)
//{
//    m_basedatos->crearTablaEnBD(tablaName,listaCampos ,
//                                QStringList()<<llavePrimariaUnicaAutoincremental<<flotante<<flotante<<flotante<<Texto
//                                <<flotante <<Texto<<flotante<<Texto
//                                <<flotante<<flotante<<flotante<<flotante<<flotante<<flotante<<flotante
//                                <<flotante<<flotante<<flotante<<flotante<<flotante<<flotante<<flotante<<flotante<<flotante );
//    return true;
//}

void distanThread::corrige(int a)
{
    if(disInternas[a].length() >= 1)
    {
        //datostabla = QString::number(disVDirecta[0].at(dis)/1000)+ "----> DisInternas(km): ";

        for (int i = 0; i < disInternas[a].length(); i++)
        {
            if((i)%2 == 0)
            {
                if(i < disInternas[a].length()-1 )
                {

                }
                else
                {
                    disVDirecta[a].replace (0,disInternas[a].last ());
                    disInternas[a].removeLast ();
                    i--;
                }
            }
        }
    }
}

void distanThread::setSigmaSeleccionado(double sigma)
{
    sigmaSeleccionado = sigma;
}

void distanThread::setTipoOnda(const QString &tipo)
{
    tipoOnda = tipo;
}

double distanThread::getSigmaSeleccionado() const
{
    return sigmaSeleccionado;
}

QString distanThread::getTipoOnda() const
{
    return tipoOnda;
}


//////////////////**********************************************////////////////////////////
// Inicializar los coeficientes desde Sigma.txt
// Llamar una sola vez al inicio del programa o en el constructor
void distanThread::inicializarCoeficientesSigma()
{

    // ============================================================
    // MEDIOS ESTÁNDAR (sin sub-bandas)
    // ============================================================
    // Mantener los coeficientes que YA TENÍAS para estos medios
    // Aquí pongo valores de ejemplo - REEMPLAZA con tus valores reales

    // Medio 36
    QHash<double, double> coefs36;
    coefs36.insert(5.0,  1.81);   // REEMPLAZA con tu valor real
    coefs36.insert(0.5, -0.78);   // REEMPLAZA con tu valor real
    coefs36.insert(0.1, -2.6);   // REEMPLAZA con tu valor real
    coeficientesSigma.insert("36", coefs36);

    // Medio 37
    QHash<double, double> coefs37;
    coefs37.insert(5.0,  1.89);   // REEMPLAZA con tu valor real
    coefs37.insert(0.5, -0.81);   // REEMPLAZA con tu valor real
    coefs37.insert(0.1, -2.7);   // REEMPLAZA con tu valor real
    coeficientesSigma.insert("37", coefs37);

    // Medio 38
    QHash<double, double> coefs38;
    coefs38.insert(5.0,  1.89);   // REEMPLAZA con tu valor real
    coefs38.insert(0.5, -0.81);   // REEMPLAZA con tu valor real
    coefs38.insert(0.1, -2.7);   // REEMPLAZA con tu valor real
    coeficientesSigma.insert("38", coefs38);

    // Medio 39
    QHash<double, double> coefs39;
    coefs39.insert(5.0,  2.17);   // REEMPLAZA con tu valor real
    coefs39.insert(0.5, -0.93);   // REEMPLAZA con tu valor real
    coefs39.insert(0.1, -3.1);   // REEMPLAZA con tu valor real
    coeficientesSigma.insert("39", coefs39);

    // Medio 42
    QHash<double, double> coefs42;
    coefs42.insert(5.0,  2.66);   // REEMPLAZA con tu valor real
    coefs42.insert(0.5, -1.14);   // REEMPLAZA con tu valor real
    coefs42.insert(0.1, -3.8);   // REEMPLAZA con tu valor real
    coeficientesSigma.insert("42", coefs42);

    // Medio 43
    QHash<double, double> coefs43;
    coefs43.insert(5.0,  1.81);   // REEMPLAZA con tu valor real
    coefs43.insert(0.5, -0.78);   // REEMPLAZA con tu valor real
    coefs43.insert(0.1, -2.6);   // REEMPLAZA con tu valor real
    coeficientesSigma.insert("43", coefs43);

    // Medio 44
    QHash<double, double> coefs44;
    coefs44.insert(5.0,  1.81);   // REEMPLAZA con tu valor real
    coefs44.insert(0.5, -0.78);   // REEMPLAZA con tu valor real
    coefs44.insert(0.1, -2.6);   // REEMPLAZA con tu valor real
    coeficientesSigma.insert("44", coefs44);

    // ============================================================
    // MEDIO 40 - CON SUB-BANDAS (M, DM, CM)
    // ============================================================
    // Los coeficientes que me has pasado:

    // Métrica (M)
    QHash<double, double> coefs40_M;
    coefs40_M.insert(5.0,   6.98);
    coefs40_M.insert(0.5,  -2.99);
    coefs40_M.insert(0.1,  -9.98);
    coeficientesSigma.insert("40_M", coefs40_M);

    // Decimétrica (DM)
    QHash<double, double> coefs40_DM;
    coefs40_DM.insert(5.0,   3.98);
    coefs40_DM.insert(0.5,  -1.71);
    coefs40_DM.insert(0.1,  -5.70);
    coeficientesSigma.insert("40_DM", coefs40_DM);

    // Centimétrica (CM)
    QHash<double, double> coefs40_CM;
    coefs40_CM.insert(5.0,   2.66);
    coefs40_CM.insert(0.5,  -1.14);
    coefs40_CM.insert(0.1,  -3.80);
    coeficientesSigma.insert("40_CM", coefs40_CM);
}

//----------------------------------------------------------------------------
// Función: Calcular y Guardar Sigma (CORREGIDA CON distanciamaxOficial POR BANDA)
//----------------------------------------------------------------------------
void distanThread::calcularYGuardarSigma(int medio, int grupo, int vuelo,
                                         const QList<double>& disMax,
                                         const QString& nombreBase)
{
    // ============================================================
    // DETERMINAR CLAVE PARA BUSCAR COEFICIENTES
    // ============================================================
    QString claveCoef;

    if (medio == 40) {
        if (tipoOnda.isEmpty()) {
            qDebug() << "ERROR: Medio 40 requiere tipo de onda configurado";
            return;
        }
        claveCoef = QString::number(medio) + "_" + tipoOnda;
    } else {
        claveCoef = QString::number(medio);
    }

    // Verificar si este medio/grupo/vuelo requiere cálculo Sigma
    if (!coeficientesSigma.contains(claveCoef) || grupo != 1) {
        return;
    }

    // Solo alturas <= 500 o >= 1000
    if (vuelo > 500 && vuelo < 1000) {
        return;
    }

    // Obtener coeficientes para esta clave
    QHash<double, double> coefs = coeficientesSigma[claveCoef];

    // ============================================================
    // CAMBIO CLAVE: Calcular distanciamaxOficial según banda
    // ============================================================
    double distanciamaxOficialBanda = obtenerDistanciaMaxOficial(medio, tipoOnda);
    qDebug() << "distanciamaxOficial para medio" << medio << "banda" << tipoOnda
             << ":" << distanciamaxOficialBanda/1000 << "km";

    // Definir valores Sigma, factores y sufijos
    double valoresSigma[] = {5.0, 0.5, 0.1};
    double factoresSigma[] = {1.49, 0.84, 0.56};

    double factoresSigmaM[] = {0.82, 0.46, 0.31};
    double factoresSigmaD[] = {0.89, 0.5, 0.34};
    double factoresSigmaC[] = {1.2, 0.67, 0.45};

    QString sufijosSigma[] = {"_S5", "_S05", "_S01"};

    // ============================================================
    // LIMPIAR NOMBRE BASE DE SUFIJOS ANTERIORES
    // ============================================================
    QString nombreBaseLimpio = limpiarSufijosNombre(nombreBase);

    // Guardar estado original para restaurar al final de TODOS los sigma
    QList<double> disVDirectaOriginal[360];
    QList<double> disInternasOriginal[360];
    QList<double> ZonaMuertaMuisOriginal[360];
    QList<double> ZonaMuertaMuis2Original[360];

    for (int a = 0; a < 360; a++) {
        disVDirectaOriginal[a] = disVDirecta[a];
        disInternasOriginal[a] = disInternas[a];
        ZonaMuertaMuisOriginal[a] = ZonaMuertaMuis[a];
        ZonaMuertaMuis2Original[a] = ZonaMuertaMuis2[a];
    }

    // Calcular para cada Sigma (5.0, 0.5, 0.1)
    for (int i = 0; i < 3; i++) {
        double sigma = valoresSigma[i];
        double factor = 0;

        if (medio == 40) {
            if (tipoOnda.isEmpty()) {
                qDebug() << "ERROR: Medio 40 requiere tipo de onda configurado";
                return;
            }

            if (tipoOnda == "M") {
                factor = factoresSigmaM[i];
            } else if (tipoOnda == "DM") {
                factor = factoresSigmaD[i];
            } else /*(tipoOnda == "CM")*/ {
                factor = factoresSigmaC[i];
            }
        }
        else {
            factor = factoresSigma[i];
        }



        double coeficiente = coefs[sigma] * 1000;  // km → m

        // ============================================================
        // PASO 1: Calcular nuevo disVDirecta[0] con Sigma (POR AZIMUT)
        // ============================================================
        // CORRECCIÓN: Calcular un valor específico para CADA azimut,
        // usando su disVDirecta original como base

        // ============================================================
        // PASO 2: Recalcular disMax para probabilidades 0.6-0.9
        // ============================================================
        // La base para probabilidades es el máximo de los nuevos valores
        // Calculamos primero todos los disVDirecta para encontrar el máximo

        double maxDisVDirectaNuevo = 0;
        double nuevosDisVDirecta[360];

        for (int a = 0; a < 360; a++) {
            double disVDirectaOriginal_a = 0;
            if (!disVDirectaOriginal[a].isEmpty()) {
                disVDirectaOriginal_a = disVDirectaOriginal[a][0];
            }

            // Aplicar factor sigma al valor ORIGINAL de este azimut
            double nuevoDisVDirecta_a = disVDirectaOriginal_a * factor;

            // Limitar a distanciamaxOficialBanda (límite físico del radar para esta banda)
            if (nuevoDisVDirecta_a > distanciamaxOficialBanda) {
                nuevoDisVDirecta_a = distanciamaxOficialBanda;
            }

            // Para alturas <= 500m, sumar coeficiente
            if (vuelo <= 500) {
                nuevoDisVDirecta_a += coeficiente;
                if (nuevoDisVDirecta_a < 0) nuevoDisVDirecta_a = 0;
                if (nuevoDisVDirecta_a > distanciamaxOficialBanda) {
                    nuevoDisVDirecta_a = distanciamaxOficialBanda;
                }
            }

            nuevosDisVDirecta[a] = nuevoDisVDirecta_a;

            if (nuevoDisVDirecta_a > maxDisVDirectaNuevo) {
                maxDisVDirectaNuevo = nuevoDisVDirecta_a;
            }
        }

        // Recalcular disMax basado en el nuevo máximo
        QList<double> nuevoDisMax;
        nuevoDisMax.append(maxDisVDirectaNuevo);                       // 0.5
        nuevoDisMax.append(maxDisVDirectaNuevo * 0.68630039 * 1.35);   // 0.6
        nuevoDisMax.append(maxDisVDirectaNuevo * 0.62735625 * 1.35);   // 0.7
        nuevoDisMax.append(maxDisVDirectaNuevo * 0.5576305 * 1.35);    // 0.8
        nuevoDisMax.append(maxDisVDirectaNuevo * 0.46250405 * 1.35);   // 0.9

        // Limitar a distanciamaxOficialBanda
        for (int j = 0; j < 5; j++) {
            if (nuevoDisMax[j] > distanciamaxOficialBanda) {
                nuevoDisMax[j] = distanciamaxOficialBanda;
            }
        }

        // ============================================================
        // PASO 3: DETERMINAR SI ESTE SIGMA ES EL SELECCIONADO
        // ============================================================
        bool esSigmaSeleccionado = (sigma == sigmaSeleccionado);

        // ============================================================
        // PASO 4: PREPARAR DATOS (globales o temporales) - POR AZIMUT
        // ============================================================
        DatosSigma datosTemp;

        for (int a = 0; a < 360; a++) {
            double nuevoDisVDirecta_a = nuevosDisVDirecta[a];

            // disVDirecta: reemplazar [0] con valor Sigma específico del azimut
            datosTemp.disVDirecta[a] = disVDirectaOriginal[a];
            if (!datosTemp.disVDirecta[a].isEmpty()) {
                datosTemp.disVDirecta[a][0] = nuevoDisVDirecta_a;
            } else {
                datosTemp.disVDirecta[a].append(nuevoDisVDirecta_a);
            }

            // disInternas: solo las que están antes de nuevoDisVDirecta_a
            for (double interna : disInternasOriginal[a]) {
                if (interna < nuevoDisVDirecta_a) {
                    datosTemp.disInternas[a].append(interna);
                }
            }

            // ZonaMuertaMuis
            for (double zm : ZonaMuertaMuisOriginal[a]) {
                if (zm < nuevoDisVDirecta_a) {
                    datosTemp.ZonaMuertaMuis[a].append(zm);
                }
            }
            if (datosTemp.ZonaMuertaMuis[a].isEmpty()) {
                datosTemp.ZonaMuertaMuis[a].append(0);
            }

            // ZonaMuertaMuis2
            for (double zm : ZonaMuertaMuis2Original[a]) {
                if (zm < nuevoDisVDirecta_a) {
                    datosTemp.ZonaMuertaMuis2[a].append(zm);
                }
            }
            if (datosTemp.ZonaMuertaMuis2[a].isEmpty()) {
                datosTemp.ZonaMuertaMuis2[a].append(0);
            }
        }

        // ============================================================
        // PASO 5: SI ES SIGMA SELECCIONADO → COPIAR A GLOBALES
        // ============================================================
        if (esSigmaSeleccionado) {
            for (int a = 0; a < 360; a++) {
                disVDirecta[a] = datosTemp.disVDirecta[a];
                disInternas[a] = datosTemp.disInternas[a];
                ZonaMuertaMuis[a] = datosTemp.ZonaMuertaMuis[a];
                ZonaMuertaMuis2[a] = datosTemp.ZonaMuertaMuis2[a];
            }
            // Actualizar disMax global
            this->disMax = nuevoDisMax;
        }

        // ============================================================
        // PASO 6: Generar nombre de fichero
        // ============================================================
        QString extension = obtenerExtension(vuelo);
        QString sufijoTipo;

        if (medio == 40) {
            sufijoTipo = "_" + tipoOnda;  // "_M", "_DM", "_CM"
        }

        QString nombreSigma = nombreBaseLimpio + sufijosSigma[i] + sufijoTipo + extension;

        qDebug() << "Guardando Sigma:" << nombreSigma
                 << "| Medio:" << medio
                 << "| Sigma:" << sigma
                 << "| Seleccionado:" << esSigmaSeleccionado
                 << "| Tipo:" << tipoOnda
                 << "| Coef:" << coeficiente / 1000 << "km"
                 << "| Max disVDirecta nuevo:" << maxDisVDirectaNuevo;

        // ============================================================
        // PASO 7: Guardar fichero
        // ============================================================
        if (esSigmaSeleccionado) {
            guardarFicheroSigma(nombreSigma, nuevoDisMax, sigma, i,
                                disVDirecta, disInternas, ZonaMuertaMuis, ZonaMuertaMuis2);
        } else {
            guardarFicheroSigma(nombreSigma, nuevoDisMax, sigma, i, datosTemp);
        }
    }
}

//----------------------------------------------------------------------------
// Función auxiliar: Obtiene la extensión según la altura de vuelo
//----------------------------------------------------------------------------
QString distanThread::obtenerExtension(int vuelo)
{
    switch (vuelo) {
    case 0:    return ".m00";
    case 25:   return ".m25";
    case 50:   return ".m50";
    case 100:  return ".001";
    case 200:  return ".002";
    case 300:  return ".003";
    case 400:  return ".004";
    case 500:  return ".005";
    case 1000: return ".010";
    case 2000: return ".020";
    case 4000: return ".040";
    case 6000: return ".060";
    case 10000: return ".100";
    case 17000: return ".170";
    case 30000: return ".300";
    default:   return ".m00";
    }
}

//----------------------------------------------------------------------------
// Función para guardar el fichero con formato Sigma (CORREGIDA CON BANDA)
//----------------------------------------------------------------------------
void distanThread::guardarFicheroSigma(const QString& nombreFichero,
                                       const QList<double>& disMaxSigma,
                                       double sigma,
                                       int indiceSigma,
                                       QList<double> disVDirectaMod[360],
QList<double> disInternasMod[360],
QList<double> ZonaMuertaMuisMod[360],
QList<double> ZonaMuertaMuis2Mod[360])
{
    QString auxF = nombreFichero;
    if(alcance)
    {
        auxF.insert(6, "a");
    }

    QFile f(QDir::currentPath() + "/Zonas/" + auxF);
    qDebug() << "guardar Sigma" << nombreFichero << "Sigma=" << sigma;

    QTextStream out(&f);

    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal("No puedo abrir el fichero Sigma para escritura.");

    // ============================================================
    // NUEVO: Obtener distanciamaxOficial según banda para validaciones
    // ============================================================
    double distanciamaxOficialBanda = obtenerDistanciaMaxOficial(calculos->medio, tipoOnda);

    double coox = 0;
    double cooy = 0;
    double disaux = 0;
    QList<double> listdisaux;

    for (int a = 0; a < 360; a++)
    {
        emit progreso(a, "Guardando datos Sigma. Esta operacion puede tardar unos minutos");

        listdisaux.clear();

        // ============================================================
        // Coordenadas usando disVDirectaMod específico del azimut
        // ============================================================
        double disVDirectaActual = disVDirectaMod[a].isEmpty() ? 0 : disVDirectaMod[a].at(0);

        // Asegurar que no excede el alcance de la banda
        if (disVDirectaActual > distanciamaxOficialBanda) {
            disVDirectaActual = distanciamaxOficialBanda;
            disVDirectaMod[a][0] = distanciamaxOficialBanda;  // Actualizar para consistencia
        }

        if(disVDirectaActual/1000 <= 320)
        {
            int indiceTerreno = (int)disVDirectaActual/90;
            // Asegurar que no excedemos el tamaño del array
            int maxIdx = calculos->DatosAzimut[a].datosterreno.size() - 1;
            if (indiceTerreno > maxIdx) indiceTerreno = maxIdx;
            if (indiceTerreno < 0) indiceTerreno = 0;

            cooy = calculos->DatosAzimut[a].datosterreno.at(indiceTerreno)->LatOR;
            coox = calculos->DatosAzimut[a].datosterreno.at(indiceTerreno)->LonOR*-1;
        }
        else
        {
            cooy = calculos->DatosAzimut[a].datosterreno.last()->LatOR;
            coox = calculos->DatosAzimut[a].datosterreno.last()->LonOR*-1;
        }

        // Coordenadas y distancia directa
        out << coox << "," << cooy << ",*" << disVDirectaActual;

        // Distancias internas
        for (double i : disInternasMod[a])
        {
            out << "," << i;
        }
        out << "*";

        // Zonas muertas
        if(calculos->medio != 0 && calculos->medio != 11 && calculos->medio != 12 &&
                calculos->medio != 13 && calculos->medio != 40)
        {
            if(ZonaMuertaMuisMod[a].length()>=1)
            {
                out << ZonaMuertaMuisMod[a].at(0);
                if(ZonaMuertaMuisMod[a].length()>1)
                {
                    for (int i = 1; i < ZonaMuertaMuisMod[a].length(); ++i)
                    {
                        out << "," << ZonaMuertaMuisMod[a].at(i);
                    }
                }
            }
            else
            {
                out << 0;
            }
            out << "*";

            if(ZonaMuertaMuis2Mod[a].length()>=1)
            {
                out << ZonaMuertaMuis2Mod[a].at(0);
                if(ZonaMuertaMuis2Mod[a].length()>1)
                {
                    for (int i = 1; i < ZonaMuertaMuis2Mod[a].length(); ++i)
                    {
                        out << "," << ZonaMuertaMuis2Mod[a].at(i);
                    }
                }
            }
            else
            {
                out << 0;
            }
        }

        // Probabilidades con Sigma modificada
        if(calculos->fichero.contains("ZVD") && (calculos->medio >= 21))
        {
            out << "*";

            // Ajustar disInternas
            for (int i = 0; i < disInternasMod[a].length(); i++)
            {
                if((i)%2 == 0)
                {
                    if(i > disInternasMod[a].length()-1)
                    {
                        disVDirectaMod[a].replace(0, disInternasMod[a].last());
                        disInternasMod[a].removeLast();
                        i--;
                    }
                }
            }

            disaux = disVDirectaMod[a].at(0);
            // Asegurar que disaux no excede el alcance de la banda
            if (disaux > distanciamaxOficialBanda) {
                disaux = distanciamaxOficialBanda;
            }
            listdisaux.append(disaux);

            // Probabilidades 0.6-0.9 desde disMaxSigma
            // Asegurar que no exceden el alcance de la banda
            for (int prob = 1; prob <= 4; prob++) {
                double probVal = disMaxSigma.at(prob);
                if (probVal > distanciamaxOficialBanda) {
                    probVal = distanciamaxOficialBanda;
                }
                out << probVal << "*";
                listdisaux.append(probVal);
            }

            // Ajustar disVDirecta según probabilidad seleccionada
            double disVDProb = listdisaux.at(probabilidad);
            if (disVDProb > distanciamaxOficialBanda) {
                disVDProb = distanciamaxOficialBanda;
            }
            disVDirectaMod[a].replace(0, disVDProb);
            for (int j = 0; j < disInternasMod[a].length(); ++j)
            {
                if(disInternasMod[a].at(j) > disVDirectaMod[a].at(0))
                {
                    disInternasMod[a].removeAt(j);
                    j--;
                }
            }

            // Cálculo de distancias de lanzamiento y ZDR
            if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 ||
                calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 ||
                calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 ||
                calculos->medio == 21 || calculos->medio == 22 || calculos->medio == 23 ||
                calculos->medio == 24 || calculos->medio == 25 || calculos->medio == 26 ||
                calculos->medio == 27 || calculos->medio == 42 || calculos->medio == 43 ||
                calculos->medio == 44) && calculos->grupo == 1)
            {
                if(comprobarECC(tiempo, Vuelo, disVDirectaMod[a].at(0)))
                {
                    double disLanza;
                    double dzdr;
                    QList<double> disZDR;

                    for (int prob = 0; prob < 5; prob++) {
                        disLanza = listdisaux.at(prob) - tiempo * velocidad;
                        // Asegurar que disLanza no es negativo ni excede alcance
                        if (disLanza < 0) disLanza = 0;
                        if (disLanza > distanciamaxOficialBanda) disLanza = distanciamaxOficialBanda;

                        calcularDlDzr(&disLanza, &dzdr, a);

                        disZDR.append(dzdr);
                        if(dzdr == 0) disLanza = 0;
                        if(Vuelo < 25) disLanza = 0;

                        if ((calculos->medio == 39 || calculos->medio == 38) && alcance) {
                            if (prob == 0) {
                                estDistanMinMax();
                            }
                        }

                        out << disLanza << "*";
                    }

                    // DisLanza según probabilidad seleccionada
                    if(probabilidad > 0)
                    {
                        disLanza = listdisaux.at(probabilidad) - tiempo * velocidad;
                    }
                    else
                    {
                        disLanza = listdisaux.at(0) - tiempo * velocidad;
                    }
                    if (disLanza < 0) disLanza = 0;
                    if (disLanza > distanciamaxOficialBanda) disLanza = distanciamaxOficialBanda;
                    if(Vuelo < 25) disLanza = 0;
                    calcularDlDzr(&disLanza, &dzdr, a);
                    if(dzdr == 0) disLanza = 0;
                    disVDirectaMod[a].append(disLanza);

                    disVDirectaMod[a].append(disZDR.at(probabilidad));

                    // Coordenadas ZDR
                    QGeoCoordinate geo;
                    LatOR = calculos->datoterrenoMiPos.LatOR;
                    LonOR = calculos->datoterrenoMiPos.LonOR;

                    geoPos.setLatitude(calculos->datoterrenoMiPos.LatOR);
                    geoPos.setLongitude(calculos->datoterrenoMiPos.LonOR);

                    geo = geoPos.atDistanceAndAzimuth(disZDR[0], a, 0.0);

                    LatOR = geo.latitude();
                    LonOR = geo.longitude();

                    if(Vuelo < 25)
                    {
                        disZDR[0] = 0; disZDR[1] = 0; disZDR[2] = 0;
                        disZDR[3] = 0; disZDR[4] = 0;
                    }

                    out << disZDR[0] << "*";
                    out << disZDR[1] << "*";
                    out << disZDR[2] << "*";
                    out << disZDR[3] << "*";
                    out << disZDR[4] << "*";
                    out << -LonOR << ',' << LatOR << "*";
                }
                else
                {
                    disVDirectaMod[a].append(0);
                    disVDirectaMod[a].append(0);
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << 0 << "*";
                    out << -LonOR << ',' << LatOR << "*";
                }
            }
        }

        out << endl;
    }

    f.close();
    emit progreso(0, "Fichero Sigma guardado: " + nombreFichero);
    qDebug() << "Fichero Sigma cerrado:" << nombreFichero;
}


//============================================================================
// GUARDARFICHEROSIGMA() - Versión const con datos temporales
//============================================================================

void distanThread::guardarFicheroSigma(const QString& nombreFichero,
                                       const QList<double>& disMaxSigma,
                                       double sigma,
                                       int indiceSigma,
                                       const DatosSigma& datos)
{
    // Simplemente desempaqueta y llama a la versión con arrays
    guardarFicheroSigma(nombreFichero, disMaxSigma, sigma, indiceSigma,
                        const_cast<QList<double>*>(datos.disVDirecta),
                        const_cast<QList<double>*>(datos.disInternas),
                        const_cast<QList<double>*>(datos.ZonaMuertaMuis),
                        const_cast<QList<double>*>(datos.ZonaMuertaMuis2));
}
//----------------------------------------------------------------------------
// Función auxiliar: Limpia nombre de fichero de todos los sufijos
//----------------------------------------------------------------------------
QString distanThread::limpiarSufijosNombre(const QString& nombre)
{
    QString resultado = nombre;

    // === QUITAR EXTENSIÓN PRIMERO ===
    // (para no confundir puntos de extensión con otros caracteres)
    QStringList extensiones = {
        ".m00", ".m25", ".m50", ".001", ".002", ".003",
        ".004", ".005", ".010", ".020", ".040", ".060",
        ".100", ".170", ".300", ".ObjL"
    };
    for (const QString& ext : extensiones) {
        if (resultado.endsWith(ext)) {
            resultado.chop(ext.length());
            break;
        }
    }

    // === QUITAR SUFIJOS DE TIPO DE ONDA (orden importante: DM antes que M) ===
    if (resultado.endsWith("_DM")) {
        resultado.chop(3);  // "_DM"
    } else if (resultado.endsWith("_CM")) {
        resultado.chop(3);  // "_CM"
    } else if (resultado.endsWith("_M")) {
        resultado.chop(2);  // "_M"
    }

    // === QUITAR SUFIJOS SIGMA (orden importante: S05 antes que S5) ===
    if (resultado.endsWith("_S05")) {
        resultado.chop(4);  // "_S05"
    } else if (resultado.endsWith("_S01")) {
        resultado.chop(4);  // "_S01"
    } else if (resultado.endsWith("_S5")) {
        resultado.chop(3);  // "_S5"
    }

    return resultado;
}
//----------------------------------------------------------------------------
// Función auxiliar: Obtener distancia máxima oficial según medio y banda
//----------------------------------------------------------------------------
double distanThread::obtenerDistanciaMaxOficial(int medio, const QString& tipoOnda)
{
    if (medio == 40) {
        // Medio 40: Alcance según tipo de onda (banda)
        if (tipoOnda == "M") {
            return 360000.0;  // Métrica: 360 km
        } else if (tipoOnda == "DM") {
            return 200000.0;  // Decimétrica: 200 km
        } else if (tipoOnda == "CM") {
            return 360000.0;  // Centimétrica: 120 km (AJUSTAR SEGÚN ESPECIFICACIÓN)
        } else {
            // Por defecto, usar el máximo (métrica)
            qDebug() << "ADVERTENCIA: Tipo de onda desconocido '" << tipoOnda
                     << "' para medio 40. Usando métrico por defecto (360 km)";
            return 360000.0;
        }
    }
    //    else if (medio == 41 || medio == 48) {
    //        // Radar de vigilancia aérea (ejemplo)
    //        return 180000.0;  // 180 km
    //    }
    else {
        // Para otros medios, usar el valor original (debe estar definido en la clase)
        return distanciamaxOficial;  // Guardar el valor original en el constructor
    }
}
