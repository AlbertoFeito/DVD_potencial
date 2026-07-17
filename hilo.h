#ifndef HILO_H
#define HILO_H

#include <QThread>

#include "ccalculos.h"

class hilo: public QThread
{
    Q_OBJECT
public:
    explicit hilo(QObject *parent =0);

    CCalculos calculos;
    int gradlong;
    int gradlat;
    int minlong;
    int minlat;
    int seglong;
    int seglat;
    float AlAntena;
    QString file;
    int cooperar;
    int medio{};
    int arma{};
    int camara{};
    float calibre{};
    QString cohete;
    int angulositio;
    void run();

    void recibirDatos(int Rcooperar, int Rgradlong, int Rminlong, int Rseglong, int Rgradlat, int Rminlat, int Rseglat, float RAlAntena, int grupo);//declarar el slot como una funcion normal con tipos de datos y parametros (add refactor)

};

#endif // HILO_H





