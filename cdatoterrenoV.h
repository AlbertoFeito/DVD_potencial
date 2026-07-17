#ifndef CDATOTERRENO_H
#define CDATOTERRENO_H

#include <QFile>

#include "dancalc.h"

class CDatoTerreno
{
public:
    CDatoTerreno();
    CDatoTerreno(QFile *f);
    double Ymap;
    double Xmap;
    double LatOR;
    double LonOR;
    double altura;
    double alturaReal;


    bool determinarAltura(DanCalc * danca, double latRad, double lonRad, double Xkm, double Ykm, double DistMts, short* datoAux);
    void guardar(QFile *f);
    void leer(QFile *f);
};

#endif // CDATOTERRENO_H
