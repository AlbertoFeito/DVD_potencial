#include "cdatoazimut.h"
#include <QDebug>
CDatoAzimut::CDatoAzimut()
= default;

CDatoAzimut::CDatoAzimut(QFile *f)
{
    leer(f);
}

CDatoAzimut::~CDatoAzimut()
{
    clearlistaDTerreno();
    clearlistaDAcierre();
}

void CDatoAzimut::guardar(QFile *f)
{
    CDatoTerreno *dt;
    qint32 length = datosterreno.length();
    f->write((char*)&length,sizeof(length));
    foreach (dt, datosterreno)
    {
        dt->guardar(f);
    }

    CDatoAnguloCierre *Ac;
    length = datosAngulosCierre.length();
    f->write((char*)&length,sizeof(length));
    foreach (Ac, datosAngulosCierre)
    {
        Ac->guardar(f);
    }
}

void CDatoAzimut::leer(QFile *f)
{
    CDatoTerreno *dt;
    qint32 length = 0;
    f->read((char*)&length,sizeof(length));
    clearlistaDTerreno();
    for(int i = 0; i < length; i++)
    {
        dt= new CDatoTerreno(f);
        datosterreno.append(dt);
    }

    CDatoAnguloCierre *Ac;
    length = 0;
    f->read((char*)&length,sizeof(length));
    clearlistaDAcierre();
    for(int i = 0; i < length; i++)
    {
        Ac= new CDatoAnguloCierre(f);
        datosAngulosCierre.append(Ac);
    }
}

void CDatoAzimut::clearlistaDTerreno()
{
//    qDebug()<<"clearlistaDTerreno";
    CDatoTerreno *dt;
    foreach (dt, datosterreno)
    {
        delete dt;
    }
    datosterreno.clear();
}

void CDatoAzimut::clearlistaDAcierre()
{
    CDatoAnguloCierre *Ac;
    foreach (Ac, datosAngulosCierre)
    {
        delete Ac;
    }
    datosAngulosCierre.clear();
}
