#include "cdatoterreno.h"
#include "utiles.h"
#include <QDir>
#include <QDebug>
CDatoTerreno::CDatoTerreno()
{
    Ymap = 0;
    Xmap = 0;
    LatOR = 0;
    LonOR = 0;
    altura = 0;
}
CDatoTerreno::CDatoTerreno(QFile *f)
{
    leer(f);
}

bool CDatoTerreno::determinarAltura(DanCalc * danca, double latRad, double lonRad, double Xkm, double Ykm, double DistMts, short* datoAux)
{
    bool bOk = false;
    const int SRTM_SIZE = 1201;
    LatOR = danca->Latitud_2(Ykm,latRad);//determinar la latitud del objetivo con respecto al radar en grados decimales
    LonOR = danca->Longitud_2(Xkm,lonRad,LatOR);//determinar la longitud del objetivo con respecto al radar en grados decimales

    GMS gmsla = QUtiles::GradosToGms(LatOR);
    GMS gmslo = QUtiles::GradosToGms(LonOR);

    Ymap = danca->Convierte_Lat_CoordMapa_y(LatOR);
    Xmap = danca->Convierte_Long_CoordMapa_x(LonOR);

    QString nombreFichero = QString("N%1W%2.hgt")
            .arg(gmsla.G, 2, 10, QLatin1Char('0'))
            .arg(gmslo.G + 1, 3, 10, QLatin1Char('0'));
    QString dirCubaAlt;

#ifdef Q_OS_WIN
    dirCubaAlt = QDir::rootPath ()+ "CubaAlt/"+nombreFichero;
#else
    dirCubaAlt = QDir::homePath ()+"/CubaAlt/"+nombreFichero;
//    qDebug()<<dirCubaAlt;
#endif
    QFile f(dirCubaAlt);
    if(f.open(QFile::ReadOnly))
    {
        int latArc=(gmsla.M)*60+gmsla.S;
        int lonArc=(gmslo.M)*60+gmslo.S;
        int row = 1200 - int(round(latArc / 3));
        int col = 1200 - int(round(lonArc / 3));
        int pos = (row * SRTM_SIZE + col) * 2;
        if(f.seek(pos))
        {
            short dato = 0;
            f.read((char*)&dato, 2);
            qSwap(((uchar*)&dato)[0], ((uchar*)&dato)[1]);
            if(dato >= 0 && dato <= 2000)
                *datoAux = dato;
            altura = *datoAux - DistMts * DistMts / 17000000;
alturaReal = *datoAux;
            bOk = true;
        }
        f.close();
    }
    else
    {alturaReal = *datoAux;
        altura = *datoAux - DistMts * DistMts / 17000000;
        bOk = true;
    }
    return bOk;
}

void CDatoTerreno::guardar(QFile *f)
{
    f->write((char*)&altura,sizeof(altura));
    f->write((char*)&LatOR/*Ymap*/,sizeof(LatOR/*Ymap*/));
    f->write((char*)&LonOR/*Xmap*/,sizeof(LonOR/*Xmap*/));
}

void CDatoTerreno::leer(QFile *f)
{
    f->read((char*)&altura,sizeof(altura));
    f->read((char*)&LatOR/*Ymap*/,sizeof(LatOR/*Ymap*/));
    f->read((char*)&LonOR/*Xmap*/,sizeof(LonOR/*Xmap*/));
}
