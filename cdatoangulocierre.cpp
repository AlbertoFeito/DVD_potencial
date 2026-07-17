#include "cdatoangulocierre.h"
#include <QDebug>
CDatoAnguloCierre::CDatoAnguloCierre()
{
    angulo = 0;
    distancia = 0;
    tangente = 0;
    altura = 0;
}

CDatoAnguloCierre::CDatoAnguloCierre(QFile *f)
{
    leer(f);
}

void CDatoAnguloCierre::guardar(QFile *f)
{

    f->write((char*)&angulo,sizeof(angulo));
    f->write((char*)&distancia,sizeof(distancia));
    f->write((char*)&tangente,sizeof(tangente));
    f->write((char*)&altura,sizeof(altura));
}

void CDatoAnguloCierre::leer(QFile *f)
{
    f->read((char*)&angulo,sizeof(angulo));
    f->read((char*)&distancia,sizeof(distancia));
    f->read((char*)&tangente,sizeof(tangente));
    f->read((char*)&altura,sizeof(altura));
}
