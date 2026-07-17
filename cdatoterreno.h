#ifndef CDATOTERRENO_H
#define CDATOTERRENO_H

#include <QFile>

#include "dancalc.h"
#include <QGeoCoordinate>
class CDatoTerreno
{
public:
    CDatoTerreno();
    CDatoTerreno(QFile *f);

    double LatOR{};
    double LonOR{};
    double altura{};
    int alturaReal{};


    bool determinarAlturaFeito(const QGeoCoordinate& geoCoor, qreal azimut, qreal distancia, short* datoAux);
    bool determinarAltura(DanCalc * danca, double latRad, double lonRad, double Xkm, double Ykm, double DistMts, short* datoAux);
    void guardar(QFile *f);
    void leer(QFile *f);
};

#endif // CDATOTERRENO_H
