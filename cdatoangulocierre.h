#ifndef CDATOANGULOCIERRE_H
#define CDATOANGULOCIERRE_H

#include <QFile>
class CDatoAnguloCierre
{
public:
    CDatoAnguloCierre();
    CDatoAnguloCierre(QFile *f);
    double angulo{};
    double distancia{};
    double tangente{};
    double altura{};

    void guardar(QFile *f);
    void leer(QFile *f);
};

#endif // CDATOANGULOCIERRE_H
