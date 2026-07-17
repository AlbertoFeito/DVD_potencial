#ifndef CDATOAZIMUT_H
#define CDATOAZIMUT_H

#include "cdatoangulocierre.h"
#include "cdatoterreno.h"

#include <QFile>
class CDatoAzimut
{
public:
    CDatoAzimut();
    CDatoAzimut(QFile *f);
    ~CDatoAzimut();

    QList<CDatoTerreno*> datosterreno;
    QList<CDatoAnguloCierre*> datosAngulosCierre;

    void guardar(QFile *f);
    void leer(QFile *f);
    void clearlistaDTerreno();
    void clearlistaDAcierre();
};

#endif // CDATOAZIMUT_H
