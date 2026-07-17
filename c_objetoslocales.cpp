#include "c_objetoslocales.h"

c_ObjetosLocales::c_ObjetosLocales(QObject *parent) :
    QThread(parent)
{

}

void c_ObjetosLocales::calcular()
{
    int distancia = 100000;
    QList<double> datosCurva;
    QList<double> datosRecta;
    for (int a = 0; a < 360; a++)
    {
        QList<CDatoTerreno*> *datosterreno = &calculos->DatosAzimut[a].datosterreno;
    }


}

void c_ObjetosLocales::run()
{

}
