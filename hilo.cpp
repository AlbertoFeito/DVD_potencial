#include "hilo.h"

#include <QMutex>
#include <QFile>
#include<QTextStream>
#include <QDir>
#include <qmath.h>
#include <QDebug>

hilo::hilo(QObject *parent):
    QThread(parent)
{

    AlAntena = 0;
    gradlong = 0;
    gradlat = 0;
    minlong = 0;
    minlat = 0;
    seglong = 0;
    seglat = 0;
    file = "";
    cooperar = 0;
    angulositio = 0;

}
void hilo::run()
{
    calculos.calcularDatos();
}
void hilo::recibirDatos(int Rcooperar, int Rgradlong, int Rminlong, int Rseglong, int Rgradlat, int Rminlat, int Rseglat, float RAlAntena, int grupo)
{
    cooperar = Rcooperar;
    calculos.grupo = grupo;
    calculos.AlAnt = RAlAntena;
    calculos.latPos.G = Rgradlat;
    calculos.latPos.M = Rminlat;
    calculos.latPos.S = Rseglat;
    calculos.lonPos.G = Rgradlong;
    calculos.lonPos.M = Rminlong;
    calculos.lonPos.S = Rseglong;
    calculos.medio = medio;
    calculos.arma = arma;
    calculos.camara = camara;
    calculos.calibre = calibre;
    calculos.cohete = cohete;
    calculos.angulositio = angulositio;
}
