
#include <math.h>
#include "gmsgeocoord.h"


extern QString m_strPathCubaAlt;


gmsGeoCoord::gmsGeoCoord(void)
{

}

gmsGeoCoord::~gmsGeoCoord(void)
{

}

double gmsGeoCoord::GmsToGrados(GMSG gms)
{
    double coord;
    if(gms.G<0)
        coord = static_cast<double>(gms.G) - static_cast<double>(gms.M / 60.0) - static_cast<double>(gms.S / 3600.0);
    else
        coord = static_cast<double>(gms.G) + static_cast<double>(gms.M / 60.0) + static_cast<double>(gms.S / 3600.0);
    return(coord);
}
GMSG gmsGeoCoord::GradosToGms(double Valor)
{
    GMSG gms;
    double val;
    gms.G = static_cast<short>(Valor);
    if(Valor<0)
        val = Valor*-1;//es valido
    else
        val = Valor;

    gms.M = static_cast<uchar>(fmod(val * 60, 60));
    gms.S = static_cast<uchar>(fmod(val * 3600, 60));
    return gms;
}

//short gmsGeoCoord::determinarAltura(GMS latPos, GMS lonPos)
//{
//    const int SRTM_SIZE = 1201;
//    short datoAux;

//    QString nombreFichero = QString("N%1W%2.hgt")
//            .arg(latPos.G, 2, 10)
//            .arg((-1*lonPos.G) + 1, 3, 10, QLatin1Char('0'));
//    QString camino;
//    camino = m_strPathCubaAlt+nombreFichero;
//    QFile f(camino);
//    if(f.open(QFile::ReadOnly))
//    {
//        int latArc=(latPos.M)*60+latPos.S;
//        int lonArc=(lonPos.M)*60+lonPos.S;
//        int row = 1200 - int(round(latArc / 3));
//        int col = 1200 - int(round(lonArc / 3));
//        int pos = (row * SRTM_SIZE + col) * 2;
//        if(f.seek(pos))
//        {
//            short dato = 0;
//            f.read((char*)&dato, 2);
//            qSwap(((uchar*)&dato)[0], ((uchar*)&dato)[1]);
//            if(dato >= 0 && dato <= 2000)
//                datoAux = dato;
//        }
//        f.close();
//    }
//    else
//    {
//        datoAux=0;
//    }
//    return (datoAux);
//}
